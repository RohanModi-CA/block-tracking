#include "region_handling.h"
#include "../image_processing/IP_types.h"
#include "../image_processing/flood_fill.h"



struct int_xy REGION_HANDLING_centroid(struct FLOOD_FILL_region region, int image_width)
{
	int area = region.members_i.length;
	int centroid_x = 0;
	int centroid_y = 0;

	for (int i=0; i<area; ++i)
	{
		int pixel_n = region.members_i.arr[i];
		struct int_xy pixel_col_row = IP_col_row_from_index(pixel_n, image_width);
		int pixel_row = pixel_col_row.y;
		int pixel_col = pixel_col_row.x;

		centroid_x += pixel_col;
		centroid_y += pixel_row;
	}

	struct int_xy out;
	out.x = centroid_x/area;
	out.y = centroid_y/area;

	return out;
}

