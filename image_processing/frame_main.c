#include "frame_main.h"
#include "IP_types.h"
#include "blur.h"
#include "../tracking/region_handling.h"
#include "../lib/ppm_lib/ppm.h"
#include "normalize_image.h"
#include <stdbool.h>
#include <stdio.h>
#include "closing.h"
#include "flood_fill.h"
#include "../config.h"



struct int_xy frame_to_centroid(const char *fn)
{

	PPM image = PPM_snew(fn);

	//printf("PPM width: %d, height: %d\n", image->width, image->height);
	
	// Now let's normalize and see. Well first we need options.
	struct NORMALIZE_IMAGE_options options;

	struct rgb DGREEN = {43, 160, 22};
	struct rgb BLUE = {43, 84, 173};
	//struct rgb WHITE = {224, 224, 224};
	struct rgb GBLACK = {55,55,55};
	struct rgb BLACK = {0,0,0};
	

	NORMALIZE_IMAGE_linearize_RGB(&DGREEN);
	NORMALIZE_IMAGE_linearize_RGB(&BLUE);
	// NORMALIZE_IMAGE_linearize_RGB(&WHITE);
	NORMALIZE_IMAGE_linearize_RGB(&GBLACK);
	// NORMALIZE_IMAGE_linearize_RGB(&BLACK);
	
	NORMALIZE_IMAGE_linearize_PPM(image);

	PPM_swrite("resources/linearizedInput.ppm", image);

	struct rgb accepted_colors[] = {DGREEN, BLUE, GBLACK};
	size_t accepted_colors_length = sizeof(accepted_colors)/sizeof(accepted_colors[0]);

	options.accepted_colors = accepted_colors;
	options.accepted_colors_length = accepted_colors_length;

	struct IP_scalar_ppm normalized = NORMALIZE_IMAGE_scalar(image, options);

	IP_scalar_ppm_save("resources/normalized.ppm", normalized, false);

	struct IP_blur_options blur_opt;
	blur_opt.save_intermediates = false;
	struct IP_scalar_ppm blurred = IP_blur(normalized, blur_opt);

	//Now let's do the morphological closing.
	struct CLOSING_options closing_opt;
	closing_opt.structuring_radius = 3;
	int target_color = IP_map_to_index(blurred.map, GBLACK);
	closing_opt.target_color = target_color;

	struct IP_scalar_ppm eroded = CLOSING_erosion(blurred, closing_opt);
	IP_scalar_ppm_save("resources/eroded.ppm", eroded, false);	
	struct IP_scalar_ppm dilated = CLOSING_dilation(eroded, closing_opt);
	IP_scalar_ppm_save("resources/closed.ppm", dilated, false);


	// Now we will call the flood fill and just look at the largest region.
	struct FLOOD_FILL_region *regions;
	int num_regions=0;

	bool (*region_validator)(struct FLOOD_FILL_region_validator_options, void *ctx);
	region_validator = CONFIG_region_validator;
	struct FLOOD_FILL_FAR_options FAR_opt;
	FAR_opt.region_validator = region_validator;
	// We need to give it the color we're interested in, which is Blue.
	// And the containing color, which is green.
	int blue_index = IP_map_to_index(blurred.map, BLUE);
	int green_index = IP_map_to_index(blurred.map, DGREEN);
	struct CONFIG_region_validator_options CRV_opt;
	CRV_opt.color_interested_in = blue_index;
	CRV_opt.containing_color = green_index;
	FAR_opt.ctx = &CRV_opt;

	regions = FLOOD_FILL_find_all_regions(dilated, &num_regions, FAR_opt);


	if(num_regions == 0)
	{
		printf("No regions found.\n");
		return (struct int_xy){-1,-1};
	}

	struct int_xy centroid = REGION_HANDLING_centroid(regions[0], blurred.width);

	return centroid;
}
