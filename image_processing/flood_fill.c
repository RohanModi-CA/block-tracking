#include <stdlib.h>
#include <string.h>
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

	// This should be pre-allocated but the recursive call will zero it.
	bool *visited_yet_by_current_region;

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



// llm refactor, logic is good.
void flood_fill_iterative(struct flood_fill_recursive_call_options opt)
{
    int height = opt.inp.height;
    int width = opt.inp.width;
    int *pixels = opt.inp.pixel_values;

    // Stack optimization: pre-allocate worst case
    int *stack = malloc(height * width * sizeof(int));
    int stack_size = 0;
    
    // Push start
    stack[stack_size++] = opt.i_start;
    opt.visited_yet[opt.i_start] = true; 

    // Directions: Up, Down, Left, Right
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    while (stack_size > 0)
    {
        int curr_idx = stack[--stack_size];
        
        // Add current pixel to region members
        FLOOD_FILL_append_to_GIA(&opt.region->members_i, curr_idx);

        struct int_xy xy = IP_col_row_from_index(curr_idx, width);
        int r = xy.y;
        int c = xy.x;

        bool is_edge_pixel = false;

        // Check all 4 neighbors
        for(int i = 0; i < 4; i++) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            // 1. Image Boundary Check
            if (nr < 0 || nr >= height || nc < 0 || nc >= width) {
                opt.region->touches_edge = true;
                is_edge_pixel = true;
                continue;
            }

            int n_idx = index_from_row_col(nr, nc, width);
            int n_color = pixels[n_idx];

            // 2. Color Check
            if (n_color != opt.which_color) {
                // It is a boundary.
                // We append it EVERY time we see it. 
                // This gives us the total 'perimeter contact' with this color.
                FLOOD_FILL_append_to_GIA(&opt.region->boundary_colors, n_color);
                is_edge_pixel = true;
            } 
            else {
                // Same color - Traverse if not visited
                if (!opt.visited_yet[n_idx]) {
                    opt.visited_yet[n_idx] = true; 
                    stack[stack_size++] = n_idx;
                }
            }
        }

        if (is_edge_pixel) {
            FLOOD_FILL_append_to_GIA(&opt.region->edge_members, curr_idx);
        }
    }
    
    free(stack);
}



/* Puts the number of regions into num_regions.
 */ 
struct FLOOD_FILL_region *FLOOD_FILL_find_all_regions(struct IP_scalar_ppm inp, int *num_regions, struct FLOOD_FILL_FAR_options opt)
{
	int total_pixels = inp.height * inp.width;
	bool *visited = calloc(total_pixels, sizeof(bool));
	
	// This will be zeroed each iteration by the iterator.
	bool *visited_yet_by_current_region = malloc(sizeof(bool)*total_pixels);


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
		RCO_opt.visited_yet_by_current_region = visited_yet_by_current_region;
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
	free(visited_yet_by_current_region); 
	return regions;
}
