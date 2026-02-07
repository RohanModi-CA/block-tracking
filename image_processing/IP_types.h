#ifndef IP_TYPES_H
#define IP_TYPES_H

#include <stdint.h>
#include "../lib/ppm_lib/ppm.h"
#include <stdbool.h>

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
void IP_scalar_ppm_save(const char *fn, struct IP_scalar_ppm scalar, bool clear_map);

struct IP_scalar_ppm IP_new_scalar_ppm(int height, int width);


struct int_xy IP_col_row_from_index(int n, int image_width);

int IP_index_from_row_col(int row, int col, int image_width);

struct int_xy
{
	int x;
	int y;
};


int IP_map_to_index(struct IP_scalar_ppm_map map, struct rgb RGB);
int IP_least_squares_map_to_index(struct IP_scalar_ppm_map map, struct rgb RGB);

#endif

