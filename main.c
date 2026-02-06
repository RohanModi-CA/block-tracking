#include "lib/ppm_lib/ppm.h"
#include "image_processing/IP_types.h"
#include "image_processing/normalize_image.h"
#include <stdio.h>
#include "image_processing/hitboxing.h"

int main() 
{ 
	PPM image = PPM_snew("resources/chair.ppm");
	printf("PPM width: %d, height: %d\n", image->width, image->height);
	
	// Now let's normalize and see. Well first we need options.
	struct NORMALIZE_IMAGE_options options;

	struct rgb DGREEN = {43, 160, 22};
	struct rgb BLUE = {43, 84, 173};
	struct rgb WHITE = {224, 224, 224};
	struct rgb BLACK = {55,55,55};

	struct rgb accepted_colors[] = {DGREEN, BLUE, BLACK};
	size_t accepted_colors_length = sizeof(accepted_colors)/sizeof(accepted_colors[0]);

	options.accepted_colors = accepted_colors;
	options.accepted_colors_length = accepted_colors_length;

	struct IP_scalar_ppm scalar = NORMALIZE_IMAGE_scalar(image, options);

	PPM normalized = IP_scalar_to_normal_PPM(scalar);
	PPM_swrite("resources/normalized.ppm", normalized);


	struct HITBOXING_SAT_options SAT_options;
	int rewards[] = {-20,1,0};
	SAT_options.rewards = rewards;
	SAT_options.rewards_length = sizeof(rewards)/sizeof(rewards[0]);
	SAT_options.density_square_radius = 10;
	
	
	struct IP_scalar_ppm scalar2 = HITBOXING_reward_ppm(scalar, SAT_options);

	PPM out = IP_scalar_to_normal_PPM(scalar2);

	PPM_swrite("resources/scalar.ppm", out);


	struct IP_scalar_ppm SAT = HITBOXING_SAT(scalar2, SAT_options);
	PPM out2 = IP_scalar_to_normal_PPM(SAT);
	PPM_swrite("resources/sat.ppm", out2);


	struct IP_scalar_ppm DENSITY_MAP = HITBOXING_DENSITY_MAP(SAT, SAT_options);
	PPM out3 = IP_scalar_to_normal_PPM(DENSITY_MAP);
	PPM_swrite("resources/density.ppm", out3);

	
	struct NORMALIZE_IMAGE_options threshold_options;
	threshold_options.accepted_colors = (struct rgb[]){WHITE, BLACK};
	threshold_options.accepted_colors_length = 2;

	struct IP_scalar_ppm thresholded = NORMALIZE_IMAGE_scalar(out3, threshold_options);
	PPM thresholded_out = IP_scalar_to_normal_PPM(thresholded);
	PPM_swrite("resources/thresholded.ppm", thresholded_out);



}
