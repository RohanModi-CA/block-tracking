#include "normalize_image.h"
#include "../lib/ppm_lib/ppm.h"
#include <stdio.h>



void best_color_match(struct rgb *pixel, struct NORMALIZE_IMAGE_options options)
{
	/* Takes a single pixel and replaces it by its closest match in options.accepted_colors. In-place. Least-squares.
	 */

	int current_best_error;
	int current_best_color_index;
	uchar R;
	uchar B;
	uchar G;

	R = pixel->R;
	B = pixel->B;
	G = pixel->G;

	current_best_error = 3 * (1<<16) + 1; // Maximum theoretical error, lol.
	current_best_color_index = 1;


	for(int i=0; i<options.accepted_colors_length; ++i)
	{
		int R_diff = R - options.accepted_colors[i].R;
		int G_diff = G - options.accepted_colors[i].G;
		int B_diff = B - options.accepted_colors[i].B;

		int i_error = R_diff*R_diff + G_diff*G_diff + B_diff*B_diff;

		if (i_error < current_best_error) 
		{
			current_best_color_index = i;
			current_best_error = i_error;
		}
	}

	*pixel = options.accepted_colors[current_best_color_index];
	return;
}




void NORMALIZE_IMAGE_go(PPM image, struct NORMALIZE_IMAGE_options options)
{
	/* Takes a PPM image, and in-place will convert all of its pixels 
	 * to ones with colors matching accepted_colors in options. */

	int pixels;

	pixels = image->height * image->width;

	for (int i=0; i<pixels; ++i)
	{
		best_color_match(&image->data[i], options);
	}
	
	return;
}






