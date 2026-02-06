#include <stdbool.h>
#include "hitboxing.h"
#include "stdlib.h"
#include "IP_types.h"
#include <limits.h>
#include <string.h>


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




struct IP_scalar_ppm HITBOXING_reward_ppm(struct IP_scalar_ppm inp, struct HITBOXING_SAT_options options)
{
	// ASSUMES YOU'VE PROPERLY SET REWARDS AND THRESHOLDED TO PIXEL_VALUES
	struct IP_scalar_ppm out;	

	struct IP_scalar_ppm_map out_map;
	out_map.colors_length = 0;
	out.map = out_map;



	out.height = inp.height;
	out.width = inp.width;
	out.pixel_values = malloc(sizeof(int) * out.height * out.width);

	for (int i=0; i<out.height*out.width; ++i)
	{
		out.pixel_values[i] = options.rewards[inp.pixel_values[i]];
	}
	return out;
}


struct IP_scalar_ppm HITBOXING_SAT(struct IP_scalar_ppm inp_rewards, struct HITBOXING_SAT_options options)
{
	// This takes the scalar_ppm that contains the REWARDS.

	int padded_rows = inp_rewards.height + 1;
	int padded_cols = inp_rewards.width + 1;

	int **grid = calloc(padded_rows, sizeof(int*));

	for (int row=0;row<padded_rows; ++row)
	{
		grid[row] = calloc(padded_cols,sizeof(int));
	}

	// Now let's do our pass.
	for (int row=1; row<padded_rows; ++row)
	{
		for (int col=1; col<padded_cols; ++col)
		{
			int original_term = inp_rewards.pixel_values[(row-1)*inp_rewards.width + (col-1)];
			int left_term = grid[row][col-1];
			int left_above_term = grid[row-1][col-1];
			int above_term = grid[row-1][col];

			grid[row][col] = original_term + left_term + above_term - left_above_term;
		}
	}

	// Now that we have this, let's fix the shape of this.
	
	struct IP_scalar_ppm out;
	out.height = inp_rewards.height;
	out.width = inp_rewards.width;
	out.pixel_values = malloc(out.height*out.width*sizeof(int));
	struct IP_scalar_ppm_map out_map;
	out_map.colors_length = 0;
	out.map = out_map;

	for (int out_row=0; out_row<out.height; ++out_row)
	{
		memcpy(&out.pixel_values[out_row*out.width], &grid[out_row+1][1], sizeof(int)*out.width);
	}

	for (int row=0; row<padded_rows; ++row)
	{
		free(grid[row]);
	}
	free(grid);


	return out;
}


/* Takes a summed area table and density maps based on square radius in the options.
 */
struct IP_scalar_ppm HITBOXING_DENSITY_MAP(struct IP_scalar_ppm inp_SAT,
                                          struct HITBOXING_SAT_options options)
{
	struct IP_scalar_ppm out = IP_new_scalar_ppm(inp_SAT.height, inp_SAT.width);

	int rad = options.density_square_radius;

	for (int i = 0; i < out.height * out.width; ++i)
	{
		int row = i / out.width;
		int col = i - (row * out.width);


		int dist_from_left   = col;
		int dist_from_right  = out.width  - (col + 1);
		int dist_from_top    = row;
		int dist_from_bottom = out.height - (row + 1);

		/* so from wikipedia I think things are implemented from the inequality as
		   (x0, y0) = padding corner (may be -1), so we need to read 0 from it?
		   (x1, y1) = bottom-right inclusive */
		int x1 = (dist_from_right  >= rad) ? col + rad : out.width  - 1;
		int y1 = (dist_from_bottom >= rad) ? row + rad : out.height - 1;

		int x0 = (dist_from_left >= rad) ? col - rad - 1 : -1;
		int y0 = (dist_from_top  >= rad) ? row - rad - 1 : -1;

		// so out of bounds read is zero, we can macro this:
		#define SAT(x,y) (((x) < 0 || (y) < 0) ? 0 : \
			inp_SAT.pixel_values[index_from_row_col((y),(x),out.width)])

		int A = SAT(x0, y0);
		int B = SAT(x1, y0);
		int C = SAT(x0, y1);
		int D = SAT(x1, y1);

		int square_content = D - B - C + A;

		int xl = (dist_from_left   >= rad) ? col - rad : 0;
		int xr = (dist_from_right  >= rad) ? col + rad : out.width  - 1;
		int yt = (dist_from_top    >= rad) ? row - rad : 0;
		int yb = (dist_from_bottom >= rad) ? row + rad : out.height - 1;

		int square_geom_area = (xr - xl + 1) * (yb - yt + 1);

		float norm_content = (float)square_content / (float)square_geom_area;

		out.pixel_values[i] = (int)(10000 * norm_content);
	}

	return out;
}



struct region
{
	int boundary_colors_length;
	int *boundary_colors;
	bool touches_edge;
	int *members_i; 
	int members_i_length;
	int *edge_members;
	int edge_members_length;
};

struct flood_fill_recursive_call_options
{
	struct IP_scalar_ppm inp;
	int i_start;
	bool *visited_yet;
	struct region *region;
	int which_color;
	int i_caller;
};



void flood_fill_recursive_call(struct flood_fill_recursive_call_options opt)
{
	if (opt.visited_yet[opt.i_start])
	{
		return;
	}

	int pixel_color = opt.inp.pixel_values[opt.i_start];
	
	if (pixel_color!=opt.which_color)
	{
		opt.region->edge_members[opt.region->edge_members_length] = opt.i_caller;
		++opt.region->edge_members_length;
		opt.region->boundary_colors[opt.region->boundary_colors_length] = pixel_color;
		++opt.region->boundary_colors_length;
		return;
	}


	opt.visited_yet[opt.i_start] = true;
	opt.region->members_i[opt.region->members_i_length] = opt.i_start;
	++opt.region->members_i_length;

	int image_height = opt.inp.height;
	int image_width = opt.inp.width;



	// My neighbours are up down left and right. Let's just get their indices. 
	struct int_xy my_xy = col_row_from_index(opt.i_start, image_width);
	int col = my_xy.x;
	int row = my_xy.y;
	
	// do this to handle edges so we don't recurse into the boundary.
	
	if(row!=0)
	{
		int up_n = index_from_row_col(row-1, col,image_width);
		struct flood_fill_recursive_call_options opt_new;
		opt_new.i_start = up_n;
		opt_new.i_caller = opt.i_start;
		opt_new.region = opt.region;
		opt_new.which_color = opt.which_color;
		opt_new.inp = opt.inp;
		opt_new.visited_yet = opt.visited_yet;
		flood_fill_recursive_call(opt_new);
	}
	else {opt.region->touches_edge = true;}

	if(row!=image_height-1)
	{
		int down_n = index_from_row_col(row+1, col,image_width);
		struct flood_fill_recursive_call_options opt_new;
		opt_new.i_start = down_n;
		opt_new.i_caller = opt.i_start;
		opt_new.region = opt.region;
		opt_new.which_color = opt.which_color;
		opt_new.inp = opt.inp;
		opt_new.visited_yet = opt.visited_yet;
		flood_fill_recursive_call(opt_new);
	}
	else {opt.region->touches_edge=true;}

	if (col!=0)
	{
		int left_n = index_from_row_col(row, col-1,image_width);
		struct flood_fill_recursive_call_options opt_new;
		opt_new.i_start = left_n;
		opt_new.i_caller = opt.i_start;
		opt_new.region = opt.region;
		opt_new.which_color = opt.which_color;
		opt_new.inp = opt.inp;
		opt_new.visited_yet = opt.visited_yet;
		flood_fill_recursive_call(opt_new);

	}
	else {opt.region->touches_edge=true;}

	if (col!=image_width-1)
	{
		int right_n = index_from_row_col(row, col+1, image_width);
		struct flood_fill_recursive_call_options opt_new;
		opt_new.i_start = right_n;
		opt_new.i_caller = opt.i_start;
		opt_new.region = opt.region;
		opt_new.which_color = opt.which_color;
		opt_new.inp = opt.inp;
		opt_new.visited_yet = opt.visited_yet;
		flood_fill_recursive_call(opt_new);
	}
	else {opt.region->touches_edge=true;}
}



/* 
 *
 */
struct IP_scalar_ppm HITBOXING_flood_fill(struct IP_scalar_ppm inp_thresholded, int num_of_colors)
{
	int height = inp_thresholded.height;
	int width = inp_thresholded.width;
	int N = height * width;
	int *pixels = inp_thresholded.pixel_values;

	bool *visited_yet = (bool *) malloc(N);
	
	int current_region=0;
	// Each pixel can only belong to one region.
	int *which_region = (int *) malloc(N);

	// For now worst case scenario each pixel is its own
	// region, just for ease, but that should be nowhere near true.
	struct region_boundary *boundaries = (struct region_boundary *) malloc(sizeof(struct region_boundary)*N);
	for (int i=0; i<N; ++i)
	{
		boundaries[i].colors = (int *) malloc(sizeof(int)*num_of_colors);
	}



	for (int i=0; i<N; ++i)
	{
		if(visited_yet[i])
		{
			continue;
		}

		

		


	}
}


