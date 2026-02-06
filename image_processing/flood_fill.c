#include <stdlib.h>
#include "flood_fill.h"
#include "IP_types.h"
#include "stdbool.h"

struct int_xy
{
	int x;
	int y;
};

int index_from_row_col(int row, int col, int image_width)
{
	return row*(image_width) + col;
}

// Col is x, Row is y.
struct int_xy col_row_from_index(int n, int image_width)
{
	struct int_xy out;

	out.y = (n/image_width);
	out.x = n - (out.y - image_width);
	return out;
}





struct flood_fill_recursive_call_options
{
	struct IP_scalar_ppm inp;
	int i_start;
	bool *visited_yet;
	struct region *region;
	int which_color;
	int i_caller;
};



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
			opt.region->edge_members[opt.region->edge_members_length] = i_caller;
			++opt.region->edge_members_length;
			opt.region->boundary_colors[opt.region->boundary_colors_length] = pixel_color;
			++opt.region->boundary_colors_length;
			continue;
		}
		
		opt.visited_yet[i_current] = true;
		opt.region->members_i[opt.region->members_i_length] = i_current;
		++opt.region->members_i_length;
		
		struct int_xy my_xy = col_row_from_index(i_current, image_width);
		int col = my_xy.x;
		int row = my_xy.y;
		
		// Push neighbors onto stack
		if (row != 0)
		{
			stack_i[stack_size] = index_from_row_col(row-1, col, image_width);
			stack_caller[stack_size] = i_current;
			stack_size++;
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
struct region *find_all_regions(struct IP_scalar_ppm inp, int *num_regions)
{
	int total_pixels = inp.height * inp.width;
	bool *visited = calloc(total_pixels, sizeof(bool));
	
	// worst case is that each pixel is its own region. i thought somehow
	// four color theorem could mean that the worst case was better
	// but now that I think of it it implies the opposite!
	struct region *regions = malloc(total_pixels * sizeof(struct region));
	*num_regions = 0;
	
	for (int i = 0; i < total_pixels; i++)
	{
		if (visited[i])
			continue;
			
		// Initialize new region
		struct region *r = &regions[*num_regions];
		r->boundary_colors_length = 0;
		r->boundary_colors = malloc(total_pixels * sizeof(int));
		r->touches_edge = false;
		r->members_i = malloc(total_pixels * sizeof(int));
		r->members_i_length = 0;
		r->edge_members = malloc(total_pixels * sizeof(int));
		r->edge_members_length = 0;
		
		// then flood fill this region
		struct flood_fill_recursive_call_options opt;
		opt.inp = inp;
		opt.i_start = i;
		opt.visited_yet = visited;
		opt.region = r;
		opt.which_color = inp.pixel_values[i];
		opt.i_caller = -1;
		
		flood_fill_iterative(opt);  
		
		(*num_regions)++;
	}
	
	free(visited);
	return regions;
}
