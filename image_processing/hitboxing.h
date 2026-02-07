#ifndef HITBOXING_H
#define HITBOXING_H

#include <stdbool.h>

struct HITBOXING_SAT_options 
{
	// The rewards per scalar ppm type. This could be calloc'ed.
	int *rewards;
	int rewards_length;
	int density_square_radius;
};

struct IP_scalar_ppm HITBOXING_SAT(struct IP_scalar_ppm inp, struct HITBOXING_SAT_options options);

struct IP_scalar_ppm HITBOXING_reward_ppm(struct IP_scalar_ppm inp, struct HITBOXING_SAT_options options);

struct IP_scalar_ppm HITBOXING_DENSITY_MAP(struct IP_scalar_ppm inp_SAT, struct HITBOXING_SAT_options options);

#endif
