#include "lib/ppm_lib/ppm.h"
#include "image_processing/normalize_image.h"
#include <stdio.h>

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

	// Now let's do the normalizing.
	NORMALIZE_IMAGE_go(image, options);

	// And let's save it.
	PPM_swrite("resources/normalized.ppm", image);
}
