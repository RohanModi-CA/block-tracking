#include <stdio.h>
#include <stdlib.h>
#include "IP_types.h"
#include "../lib/ppm_lib/ppm.h"
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>




int IP_index_from_row_col(int row, int col, int image_width)
{
	return row*(image_width) + col;
}

// Col is x, Row is y.
struct int_xy IP_col_row_from_index(int n, int image_width)
{
	struct int_xy out;

	out.y = (n/image_width);
	out.x = n - (out.y*image_width);
	return out;
}



void IP_scalar_ppm_free(struct IP_scalar_ppm scalar)
{
	free(scalar.pixel_values);
}

PPM IP_scalar_to_normal_PPM(struct IP_scalar_ppm scalar)
{
	// Will convert a scalar_ppm to big ppm, handling the case that there is no map length.
	PPM out;
	
	out = PPM_dnew(scalar.width, scalar.height); // this mallocs the RGB for us.
													 
	if (scalar.map.colors_length <= 0)
	{
		// We will find the minimum and maximum pixel and normalize accordingly.
		int min_pixel = INT_MAX;
		int max_pixel = INT_MIN;
		for (int i=0; i<scalar.width*scalar.height; ++i)
		{
			int this_pixel = scalar.pixel_values[i];
			min_pixel = this_pixel < min_pixel ? this_pixel : min_pixel;
			max_pixel = this_pixel > max_pixel ? this_pixel : max_pixel;
		}

		// Make sure they are not the same:
		max_pixel = (max_pixel == min_pixel) ? max_pixel+1 : max_pixel;

		// Now normalize the pixels according to this.

		for (int i=0; i<scalar.width*scalar.height; ++i)
		{
			int this_pixel = scalar.pixel_values[i];
	
			float p = ((float)this_pixel - (float)min_pixel)/((float)max_pixel-(float)min_pixel);

			int this_intensity = (int) (255.0f * (0.1*p + 0.3*(p*p) + 0.6*(p*p*p)));

			out->data[i] = (struct rgb) {this_intensity, this_intensity, this_intensity};
		}

	}
	else
	{
		for (int i=0; i<scalar.width*scalar.height; ++i)
		{
			out->data[i] = scalar.map.colors[scalar.pixel_values[i]];
			
		}
	}

	return out;
}

/* Creates and allocates a new scalar ppm, intiializing the map color length to zero. Does not allocate a map, since we say the length is zero.
 */
struct IP_scalar_ppm IP_new_scalar_ppm(int height, int width)
{
	struct IP_scalar_ppm out;
	out.height = height;
	out.width = width;

	out.pixel_values = (int *) malloc(sizeof(int)*height*width);
	
	struct IP_scalar_ppm_map map;
	map.colors_length = 0;
	out.map = map;

	return out;
}

// clear_map sets the length of the map to zero if you want to stick to traditional
// fractionals.
void IP_scalar_ppm_save(const char *fn, struct IP_scalar_ppm scalar, bool clear_map)
{
	if (clear_map)
	{
		scalar.map.colors_length = 0;
	}

	PPM temp = IP_scalar_to_normal_PPM(scalar);
	PPM_swrite(fn, temp);
	PPM_free(temp);
}


int IP_map_to_index(struct IP_scalar_ppm_map map, struct rgb RGB)
{
	for(int i=0; i<map.colors_length; ++i)
	{
		if (map.colors[i].R == RGB.R && map.colors[i].B == RGB.B 
				&& map.colors[i].G == RGB.G)
		{
			return i;
		}
	}
		// or we failed.
		printf("map_to_index: color not found in map.\n");
		exit(1);
}


int IP_least_squares_map_to_index(struct IP_scalar_ppm_map map, struct rgb RGB)
{
	int min_error = INT_MAX;
	int i_min_error = -1;
	for (int i=0; i<map.colors_length; ++i)
	{
		int Re = RGB.R - map.colors[i].R;
		int Ge = RGB.G - map.colors[i].G;
		int Be = RGB.B - map.colors[i].B;

		int this_error = Re*Re + Be*Be + Ge*Ge;
		if (this_error < min_error)
		{
			min_error = this_error;
			i_min_error = i;
		}
	}
	return i_min_error;
}
