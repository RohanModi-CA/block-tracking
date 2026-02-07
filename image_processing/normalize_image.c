#include "normalize_image.h"
#include <stdlib.h>
#include "../lib/ppm_lib/ppm.h"
#include "IP_types.h"
#include <math.h>


// linearizes in place
void NORMALIZE_IMAGE_linearize_RGB(struct rgb *RGB)
{
	float norm_R = ((float)RGB->R)/(255.0f);
	float norm_G = ((float)RGB->G)/(255.0f);
	float norm_B = ((float)RGB->B)/(255.0f);

	RGB->R = (int) (255*pow(norm_R, 2.2));
	RGB->G = (int) (255*pow(norm_G, 2.2));
	RGB->B = (int) (255*pow(norm_B, 2.2));

	return;
}


void NORMALIZE_IMAGE_linearize_PPM(PPM ppm)
{
	for (int i=0; i<ppm->height*ppm->width; ++i)
	{
		NORMALIZE_IMAGE_linearize_RGB(&ppm->data[i]);
	}
}



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
	current_best_color_index = 0;


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


int best_color_match_index(struct rgb *pixel, struct NORMALIZE_IMAGE_options options)
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
	current_best_color_index = 0;


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

	return current_best_color_index;
}


struct IP_scalar_ppm NORMALIZE_IMAGE_scalar(PPM image, struct NORMALIZE_IMAGE_options options)
{
	struct IP_scalar_ppm out;
	out.height = image->height;
	out.width = image->width;
	out.pixel_values = malloc(sizeof(int)*out.height*out.width);

	struct IP_scalar_ppm_map out_map;
	out_map.colors_length = options.accepted_colors_length;
	out_map.colors = options.accepted_colors;

	out.map = out_map;

	for (int i=0; i<out.height*out.width; ++i)
	{
		out.pixel_values[i] = best_color_match_index(&image->data[i], options);
	}

	return out;
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






