#include "config.h"
#include "image_processing/flood_fill.h"
#include <stdbool.h>


// THIS NEEDS THE COLOR TO EXTRACT PASSED TO IT
bool CONFIG_region_validator(struct FLOOD_FILL_region_validator_options opt, void *ctx)
{

	// For now we'll accept every array that is bigger than 10 pixels.
	
	int members = opt.region.members_i.length;

	if (members < 100)
	{
		return false;
	}

	int color_interested_in = *((int *)ctx); 
	if (opt.region.color != color_interested_in)
	{
		return false;
	}


	return true;
}











