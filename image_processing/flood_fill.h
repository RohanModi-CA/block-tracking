#ifndef IP_FLOOD_FILL_H
#define IP_FLOOD_FILL_H


#include <stdbool.h>
#include "IP_types.h"

struct region
{
	int boundary_colors_length;
	int *boundary_colors;
	bool touches_edge;
	int *members_i; 
	int members_i_length;
	int *edge_members;
	int edge_members_length;
};

struct region *find_all_regions(struct IP_scalar_ppm inp, int *num_regions);

#endif
