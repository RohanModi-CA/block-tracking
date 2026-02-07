#include "closing.h"
#include "IP_types.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// internet
static int cmp(const void *a, const void *b)
{
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);
}

// len >0;
static int mode(int *arr, int len)
{
	qsort(arr, len, sizeof(int), cmp);
	int best = arr[0];
	int best_n = 1;
	int prev = arr[0];
	int current_n = 1;

	for (int i=1; i<len; ++i)
	{
		if (arr[i] == prev)
		{
			++current_n;
			if (current_n > best_n)
			{
				best_n = current_n;
				best = arr[i];
			}
		}
		else
		{
			current_n = 1;
			prev = arr[i];
		}
		
	}
	return best;
}


struct IP_scalar_ppm CLOSING_erosion(struct IP_scalar_ppm inp, struct CLOSING_options opt)
{
	struct IP_scalar_ppm out;
	int height = inp.height;
	int width = inp.width;
	struct IP_scalar_ppm_map map = inp.map;
	int *pixels = inp.pixel_values;
	int rad = opt.structuring_radius;

	out = IP_new_scalar_ppm(height, width);
	out.map = map;

	int square_size = (1+2*rad)*(1+2*rad);
	int *surrounding_colors = (int *) malloc(sizeof(int) * square_size);
	int surrounding_colors_length = 0;

	for (int i=0; i<height*width; ++i)
	{
		if (pixels[i] == opt.target_color)
		{
			struct int_xy col_row = IP_col_row_from_index(i, width);
			int col = col_row.x;
			int row = col_row.y;
			surrounding_colors_length = 0;


			bool survives = true;
			for (int j=-rad; j<=rad; ++j)
			{
				if(row+j < 0 || row+j >= height) {continue;}
				for (int k=-rad; k<=rad; ++k)
				{
					if(col+k<0|| col+k >= width){continue;}
					int t_i = IP_index_from_row_col(row+j, col+k, width);
					if (pixels[t_i]!=opt.target_color)
					{
						survives = false;
						surrounding_colors[surrounding_colors_length] = pixels[t_i];
						++surrounding_colors_length;
					}

				}
			} // structuring element loop.
			if (survives)
			{
				out.pixel_values[i] = pixels[i];
			}
			else
			{
				// take the mode of the surroundings as this pixel.
				out.pixel_values[i] = mode(surrounding_colors, surrounding_colors_length);
			}
		}
		else
		{
			out.pixel_values[i] = pixels[i];
		}
	}

	free(surrounding_colors);
	return out;
}

struct IP_scalar_ppm CLOSING_dilation(struct IP_scalar_ppm inp, struct CLOSING_options opt)
{
	struct IP_scalar_ppm out;
	int height = inp.height;
	int width = inp.width;
	int *pixels = inp.pixel_values;
	int rad = opt.structuring_radius;
	
	out = IP_new_scalar_ppm(height, width);
	out.map = inp.map;
	
	// we input to output first
	memcpy(out.pixel_values, pixels, sizeof(int) * height * width);
	
	for (int i=0; i<height*width; ++i)
	{
		if (pixels[i] == opt.target_color)
		{
			struct int_xy col_row = IP_col_row_from_index(i, width);
			int col = col_row.x;
			int row = col_row.y;
			
			for (int j=-rad; j<=rad; ++j)
			{
				if(row+j < 0 || row+j >= height) {continue;}
				for (int k=-rad; k<=rad; ++k)
				{
					if(col+k<0|| col+k >= width){continue;}
					int t_i = IP_index_from_row_col(row+j, col+k, width);
					out.pixel_values[t_i] = opt.target_color;
				}
			}
		}
	}
	return out;
}
