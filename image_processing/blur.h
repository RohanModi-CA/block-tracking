#ifndef BLUR_H
#define BLUR_H

#include <stdbool.h>

struct IP_blur_options
{
	bool save_intermediates;
};

struct IP_scalar_ppm IP_blur(struct IP_scalar_ppm thresholded, struct IP_blur_options opt);

#endif
