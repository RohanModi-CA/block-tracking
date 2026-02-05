#ifndef NORMALIZE_IMAGE_H
#define NORMALIZE_IMAGE_H

#include "../lib/ppm_lib/ppm.h"

struct NORMALIZE_IMAGE_options 
{
  struct rgb *accepted_colors;
  int accepted_colors_length;
};

void NORMALIZE_IMAGE_go(PPM image, struct NORMALIZE_IMAGE_options options);

#endif
