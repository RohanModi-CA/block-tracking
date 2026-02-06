#ifndef IP_TYPES_H
#define IP_TYPES_H

#include <stdint.h>
#include "../lib/ppm_lib/ppm.h"

struct IP_scalar_ppm_map
{
	struct rgb *colors;
	int colors_length; // MUST BE SET TO ZERO IF *COLORS IS NOT SET. 
};

struct IP_scalar_ppm
{
	int height;
	int width;
	int *pixel_values;
	struct IP_scalar_ppm_map map;
};

// And let's just make a function that converts between the two.
PPM IP_scalar_to_normal_PPM(struct IP_scalar_ppm scalar);

struct IP_scalar_ppm IP_new_scalar_ppm(int height, int width);


#endif
