#include <stdlib.h>
#include "flood_fill.h"
#include "IP_types.h"
#include "stdbool.h"


static int index_from_row_col(int row, int col, int image_width)
{
	return row*(image_width) + col;
}


struct flood_fill_recursive_call_options
{
	struct IP_scalar_ppm inp;
	int i_start;
	bool *visited_yet;
	struct FLOOD_FILL_region *region;
	int which_color;
	int i_caller;
};




// Appends V to gia
void FLOOD_FILL_append_to_GIA(struct FLOOD_FILL_growing_int_array *gia, int V)
{
	if(gia->length == gia->capacity)
	{
		int new_cap = 2 * gia->capacity;
		gia->arr = (int *) realloc(gia->arr, sizeof(int)*new_cap);
		gia->capacity = new_cap;
	}
		gia->arr[gia->length] = V;
		++gia->length;
	return;
}

struct FLOOD_FILL_growing_int_array FLOOD_FILL_new_gia(int cap)
{
	struct FLOOD_FILL_growing_int_array out;
	out.length = 0;

	out.capacity = cap;
	out.arr = (int *) malloc(sizeof(int)*out.capacity);
	return out;
}




static void initialize_region(struct FLOOD_FILL_region *r)
{
	r->touches_edge = false;
	r->edge_members = FLOOD_FILL_new_gia(1<<8);
	r->boundary_colors = FLOOD_FILL_new_gia(1<<8);
	r->members_i = FLOOD_FILL_new_gia(1<<8);
	return;
}



void flood_fill_iterative(struct flood_fill_recursive_call_options opt)
{
	// we make a stack that keeps track of what's the next pixel
	// to look at because otherwise we stack overflow.
	int *stack_i = malloc(opt.inp.height * opt.inp.width * sizeof(int));
	int *stack_caller = malloc(opt.inp.height * opt.inp.width * sizeof(int));
	int stack_size = 0;
	
	// push the initial pixel
	stack_i[stack_size] = opt.i_start;
	stack_caller[stack_size] = opt.i_caller;
	stack_size++;
	
	int image_height = opt.inp.height;
	int image_width = opt.inp.width;
	
	while (stack_size > 0)
	{
		// Pop from stack
		stack_size--;
		int i_current = stack_i[stack_size];
		int i_caller = stack_caller[stack_size];
		
		if (opt.visited_yet[i_current])
			continue;
			
		int pixel_color = opt.inp.pixel_values[i_current];
		
		if (pixel_color != opt.which_color)
		{
			FLOOD_FILL_append_to_GIA(&opt.region->edge_members, i_caller);
			FLOOD_FILL_append_to_GIA(&opt.region->boundary_colors, pixel_color);
			continue;
		}
		
		opt.visited_yet[i_current] = true;

		FLOOD_FILL_append_to_GIA(&opt.region->members_i, i_current);

		
		struct int_xy my_xy = IP_col_row_from_index(i_current, image_width);
		int col = my_xy.x;
		int row = my_xy.y;
		


		if (row != 0)
		{
			int neighbor = index_from_row_col(row-1, col, image_width);
			if (!opt.visited_yet[neighbor]) {  
				stack_i[stack_size] = neighbor;
				stack_caller[stack_size] = i_current;
				stack_size++;
			}
		}
		else opt.region->touches_edge = true;
		
		if (row != image_height-1)
		{
			stack_i[stack_size] = index_from_row_col(row+1, col, image_width);
			stack_caller[stack_size] = i_current;
			stack_size++;
		}
		else opt.region->touches_edge = true;
		
		if (col != 0)
		{
			stack_i[stack_size] = index_from_row_col(row, col-1, image_width);
			stack_caller[stack_size] = i_current;
			stack_size++;
		}
		else opt.region->touches_edge = true;
		
		if (col != image_width-1)
		{
			stack_i[stack_size] = index_from_row_col(row, col+1, image_width);
			stack_caller[stack_size] = i_current;
			stack_size++;
		}
		else opt.region->touches_edge = true;
	}
	
	free(stack_i);
	free(stack_caller);
}


/* Puts the number of regions into num_regions.
 */ 
struct FLOOD_FILL_region *FLOOD_FILL_find_all_regions(struct IP_scalar_ppm inp, int *num_regions, struct FLOOD_FILL_FAR_options opt)
{
	int total_pixels = inp.height * inp.width;
	bool *visited = calloc(total_pixels, sizeof(bool));
	
	// worst case is that each pixel is its own region. i thought somehow
	// four color theorem could mean that the worst case was better
	// but now that I think of it it implies the opposite!
	struct FLOOD_FILL_region *regions = malloc(total_pixels * sizeof(struct FLOOD_FILL_region));
	*num_regions = 0;
	
	for (int i = 0; i < total_pixels; i++)
	{
		if (visited[i])
			continue;
			
		// Initialize new region


		struct FLOOD_FILL_region *r = &regions[*num_regions];
		initialize_region(r);

		
		// then flood fill this region
		struct flood_fill_recursive_call_options RCO_opt;
		RCO_opt.inp = inp;
		RCO_opt.i_start = i;
		RCO_opt.visited_yet = visited;
		RCO_opt.region = r;
		RCO_opt.which_color = inp.pixel_values[i];
		RCO_opt.i_caller = -1;
		r->color = inp.pixel_values[i];
		
		flood_fill_iterative(RCO_opt);  

		struct FLOOD_FILL_region_validator_options RVO_opt;
		RVO_opt.region = *RCO_opt.region;

		bool valid_region = opt.region_validator(RVO_opt, opt.ctx);

		if (valid_region)
		{
			(*num_regions)++;
		}
		else
		{ // Clean up.
			free(RCO_opt.region->boundary_colors.arr);
			free(RCO_opt.region->edge_members.arr);
			free(RCO_opt.region->members_i.arr);
		}
	}
	
	free(visited);
	return regions;
}
