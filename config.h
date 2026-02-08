#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include "image_processing/flood_fill.h"

struct CONFIG_region_validator_options
{
	int color_interested_in;
	int containing_color;
};

bool CONFIG_region_validator(struct FLOOD_FILL_region_validator_options opt, void *ctx);

#endif
