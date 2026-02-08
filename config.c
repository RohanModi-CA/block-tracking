#include "config.h"
#include "image_processing/flood_fill.h"
#include <stdbool.h>



// THIS NEEDS THE COLOR TO EXTRACT PASSED TO IT
bool CONFIG_region_validator(struct FLOOD_FILL_region_validator_options opt, void *ctx)
{

	struct CONFIG_region_validator_options CRV_opt = *((struct CONFIG_region_validator_options *) ctx);
	
	int color_interested_in = CRV_opt.color_interested_in;
	int containing_color = CRV_opt.containing_color;

	// For now we'll accept every array that is bigger than 10 pixels.
	int members = opt.region.members_i.length;

	if (members < 100)
	{
		return false;
	}

	if (opt.region.color != color_interested_in)
	{
		return false;
	}


	// We'll do a check. We want over 85% of the border to be with green.
	int containing_color_boundary_count = 0;
	for (int i=0; i<opt.region.boundary_colors.length; ++i)
	{
		if(opt.region.boundary_colors.arr[i] == containing_color)
		{
			++containing_color_boundary_count;
		}
	}

	if (((float)containing_color_boundary_count)/((float)opt.region.boundary_colors.length) <= 0.85)
	{
		return false;
	}

	return true;
}











