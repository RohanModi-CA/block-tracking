#ifndef NORMALIZE_IMAGE_H
#define NORMALIZE_IMAGE_H

#include "../lib/ppm_lib/ppm.h"
#include "IP_types.h"

struct NORMALIZE_IMAGE_options 
{
  struct rgb *accepted_colors;
  int accepted_colors_length;
};



struct IP_scalar_ppm NORMALIZE_IMAGE_scalar(PPM image, struct NORMALIZE_IMAGE_options options);


void NORMALIZE_IMAGE_go(PPM image, struct NORMALIZE_IMAGE_options options);

#endif
