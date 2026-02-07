#ifndef IP_FLOOD_FILL_H
#define IP_FLOOD_FILL_H


#include <stdbool.h>
#include "IP_types.h"

struct FLOOD_FILL_region
{
	int color;
	struct FLOOD_FILL_growing_int_array boundary_colors;
	struct FLOOD_FILL_growing_int_array members_i;
	struct FLOOD_FILL_growing_int_array edge_members;
	bool touches_edge;
};

struct FLOOD_FILL_FAR_options
{
	bool (*region_validator)(struct FLOOD_FILL_FAR_options);
};

struct FLOOD_FILL_region_validator_options
{
	struct FLOOD_FILL_region region;
};



struct FLOOD_FILL_region *FLOOD_FILL_find_all_regions(struct IP_scalar_ppm inp, int *num_regions);

struct FLOOD_FILL_growing_int_array
{
	int capacity;
	int length;
	int *arr;
};


void FLOOD_FILL_append_to_GIA(struct FLOOD_FILL_growing_int_array *gia, int V);

struct FLOOD_FILL_growing_int_array FLOOD_FILL_new_gia(int len);



#endif
