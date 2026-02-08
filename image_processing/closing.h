#ifndef CLOSING_H
#define CLOSING_H
#include "IP_types.h"


struct CLOSING_options
{
	int structuring_radius;
	int target_color;
	
};


struct IP_scalar_ppm CLOSING_erosion(struct IP_scalar_ppm inp, struct CLOSING_options opt);

struct IP_scalar_ppm CLOSING_dilation(struct IP_scalar_ppm inp, struct CLOSING_options opt);


#endif
