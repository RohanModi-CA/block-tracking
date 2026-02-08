#include "IP_types.h"
#include "stdbool.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "blur.h"
#include "hitboxing.h"

// So we're going to implement the three step blur.
// 0. Start with a pre-thresholded PPM, with a color_map.
// 1. Create 3 scalar PPMs for each channel.
// 2. SAT each of them. 
// 3. Density map each of them (per-channel blur)
// 4. Recombine into PPM
// 5. Threshold back to color_map.

int map_to_index(struct IP_scalar_ppm_map map, struct rgb RGB)
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


int least_squares_map_to_index(struct IP_scalar_ppm_map map, struct rgb RGB)
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


struct scalar_ppm_array
{
	struct IP_scalar_ppm *ppms;
	int ppms_length;
};


static struct scalar_ppm_array thresholded_to_ppm_array(struct IP_scalar_ppm thresholded)
{
	// This must have a color map.
	struct IP_scalar_ppm_map color_map = thresholded.map;
	if(color_map.colors_length <= 0)
	{
		printf("thresholded_to_ppm_array MUST take a color map.");
		exit(1);
	}

	int height = thresholded.height;
	int width = thresholded.width;

	// One array for each R,G,B.
	struct IP_scalar_ppm *ppms = (struct IP_scalar_ppm *) malloc(3*sizeof(struct IP_scalar_ppm));

	for(int i=0; i<3; ++i)
	{
		ppms[i].pixel_values = malloc(height*width*sizeof(int));
		ppms[i].height = height;
		ppms[i].width = width;
		ppms[i].map = color_map;
	}


	for(int i=0; i<height*width; ++i)
	{
		int pixel_val = thresholded.pixel_values[i];
		int R = color_map.colors[pixel_val].R;
		int G = color_map.colors[pixel_val].G;
		int B = color_map.colors[pixel_val].B;
		ppms[0].pixel_values[i]=R;
		ppms[1].pixel_values[i]=G;
		ppms[2].pixel_values[i]=B;
	}
	
	struct scalar_ppm_array out;
	out.ppms = ppms;
	out.ppms_length = 3;
	return out;
}

// ASSUMES THEY ARE ALL THE SAME SIZE AND COLORMAP. IF NOT, WELL, ENJOY THE UB!
static struct IP_scalar_ppm channel_PPMs_to_scalar(struct IP_scalar_ppm R, struct IP_scalar_ppm G, struct IP_scalar_ppm B)
{
	int height = R.height;
	int width  = R.width;
	struct IP_scalar_ppm_map map = R.map;
	int *Rs = R.pixel_values;
	int *Gs = G.pixel_values;
	int *Bs = B.pixel_values;

	struct IP_scalar_ppm out = IP_new_scalar_ppm(height, width);
	out.map = map;

	for (int i=0;i<height*width; ++i)
	{
		int R = Rs[i];
		int G = Bs[i];
		int B = Gs[i];

		//out.pixel_values[i] = map_to_index(map, (struct rgb){R,G,B});
		out.pixel_values[i] = least_squares_map_to_index(map, (struct rgb){R,G,B});
	}

	return out;
}



struct IP_scalar_ppm IP_blur(struct IP_scalar_ppm thresholded, struct IP_blur_options opt)
{
	struct IP_scalar_ppm thresholded_R;
	struct IP_scalar_ppm thresholded_G;
	struct IP_scalar_ppm thresholded_B;

	struct IP_scalar_ppm SAT_R;
	struct IP_scalar_ppm SAT_G;
	struct IP_scalar_ppm SAT_B;

	struct IP_scalar_ppm DENSITY_R;
	struct IP_scalar_ppm DENSITY_G;
	struct IP_scalar_ppm DENSITY_B;

	struct IP_scalar_ppm recombined_blurred;
	struct scalar_ppm_array thresholded_channels_ppms = thresholded_to_ppm_array(thresholded);

	thresholded_R = thresholded_channels_ppms.ppms[0];
	thresholded_G = thresholded_channels_ppms.ppms[1];
	thresholded_B = thresholded_channels_ppms.ppms[2];

	struct IP_scalar_ppm_map inp_map = thresholded_R.map;

	struct HITBOXING_SAT_options SAT_options;
	SAT_options.density_square_radius = 10;

	SAT_R = HITBOXING_SAT(thresholded_R, SAT_options);
	SAT_G = HITBOXING_SAT(thresholded_B, SAT_options);
	SAT_B = HITBOXING_SAT(thresholded_G, SAT_options);

	DENSITY_R = HITBOXING_DENSITY_MAP(SAT_R, SAT_options);
	DENSITY_G = HITBOXING_DENSITY_MAP(SAT_G, SAT_options);
	DENSITY_B = HITBOXING_DENSITY_MAP(SAT_B, SAT_options);

	DENSITY_R.map = inp_map;
	DENSITY_G.map = inp_map;
	DENSITY_B.map = inp_map;


	recombined_blurred = channel_PPMs_to_scalar(DENSITY_R, DENSITY_G,DENSITY_B);

	if (opt.save_intermediates) {
		IP_scalar_ppm_save("resources/thresholded_R.ppm", thresholded_R, true);
		IP_scalar_ppm_save("resources/thresholded_G.ppm", thresholded_G, true);
		IP_scalar_ppm_save("resources/thresholded_B.ppm", thresholded_B, true);

		IP_scalar_ppm_save("resources/SAT_R.ppm", SAT_R, true);
		IP_scalar_ppm_save("resources/SAT_G.ppm", SAT_G, true);
		IP_scalar_ppm_save("resources/SAT_B.ppm", SAT_B, true);

		IP_scalar_ppm_save("resources/DENSITY_R.ppm", DENSITY_R, true);
		IP_scalar_ppm_save("resources/DENSITY_G.ppm", DENSITY_G, true);
		IP_scalar_ppm_save("resources/DENSITY_B.ppm", DENSITY_B, true);

		IP_scalar_ppm_save("resources/recombined_blurred.ppm", recombined_blurred, false);
	}

	return recombined_blurred;
}



