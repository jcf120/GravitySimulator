/*
 *  Gravity.h
 *  ThreeBodyGravity
 *
 *  Created by Jocelyn Clifford-Frith on 07/11/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


struct vector3d {
	double x;
	double y;
	double z;
};
typedef struct vector3d Vector3d;

struct animation {
	double	 *time;
	unsigned frame_count;
	Vector3d **bodies;
	unsigned body_count;
	Vector3d *linear_momentum;
	Vector3d *angular_momentum;
	double	 *orbital_energy;
	double	 *kinetic_energy;
	double	 *potential_energy;
	int		 min_hits;
	int		 max_hits;
};
typedef struct animation Animation;

void alloc_animation(Animation *ani);
void realloc_animation(Animation *ani);
void free_animation(Animation *ani);

struct body {
	Vector3d p;
	Vector3d newp;
	Vector3d v;
	double mass;
};
typedef struct body Body;

// Main function
Animation simulate(Body *bodies,
				   unsigned body_count,
				   double dt,
				   double duration);
Animation simulate_average(Body *bodies,
						   unsigned body_count,
						   double dt,
						   double duration);
Animation simulate_adaptive(Body *bodies,
							unsigned body_count,
							double dt_a,
							double duration);
Animation simulate_adaptive_by_force_gradient(Body *bodies,
											  unsigned body_count,
											  double dt_gradA_cross_v,
											  double min_dt,
											  double max_dt,
											  double duration);

void make_frame(Animation *ani, unsigned frame, Body *bodies, unsigned body_count);