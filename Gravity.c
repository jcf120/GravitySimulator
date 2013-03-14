/*
 *  Gravity.c
 *  ThreeBodyGravity
 *
 *  Created by Jocelyn Clifford-Frith on 07/11/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Gravity.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define GRAV_CONST 4*M_PI*M_PI

// Vector Maths
Vector3d vector_add(Vector3d a, Vector3d b)
{Vector3d c; c.x = a.x+b.x; c.y = a.y+b.y; c.z = a.z+b.z; return c;}
Vector3d vector_subtract(Vector3d a, Vector3d b)
{Vector3d c; c.x = a.x-b.x; c.y = a.y-b.y; c.z = a.z-b.z; return c;}
double vector_magnitude(Vector3d v)
{return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);}
Vector3d vector_normal(Vector3d v)
{double m=vector_magnitude(v); v.x/=m; v.y/=m; v.z/=m; return v;}
Vector3d vector_scale(Vector3d v, double s)
{Vector3d r; r.x=v.x*s; r.y=v.y*s; r.z=v.z*s; return r;}
Vector3d vector_cross(Vector3d a, Vector3d b)
{Vector3d c; c.x=a.y*b.z-a.z*b.y; c.y=a.z*b.x-a.x*b.z; c.z=a.x*b.y-a.y*b.x; return c;}
double vector_dot(Vector3d a, Vector3d b)
{return a.x*b.x+a.y*b.y+a.z*b.z;}

Animation simulate(Body *bodies, unsigned body_count, double dt, double duration)
{
	// Allocate animation
	Animation ani;
	ani.frame_count = duration/dt;
	ani.body_count = body_count;
	alloc_animation(&ani);
	
	// Copy Bodies
	Body *_bodies = calloc(sizeof(Body), body_count);
	for (int n=0; n<body_count; n++)
		_bodies[n]=bodies[n];	
	
	
	// First plot
	make_frame(&ani, 0, _bodies, body_count);
	
	// Cycle through frames
	double time = 0.0;
	for (int f=1; f<ani.frame_count; f++) {
		
		// Step time
		time += dt;
		
		// Cycle through bodies
		for (int n=0; n<body_count; n++) {
			
			// Effect of each body
			for (int m=0; m<body_count; m++) {
				
				// Don't compare same body
				if (m==n) continue;
				
				// Find m relative to n
				Vector3d rel = vector_subtract(_bodies[n].p, _bodies[m].p);
				
				// Find new velocity under gravity
				double mag = vector_magnitude(rel);
				_bodies[n].v = vector_subtract(_bodies[n].v,
											   vector_scale(vector_normal(rel),
															GRAV_CONST
															*_bodies[m].mass
															*dt/mag/mag));
			}
			
			//Find new positions
			_bodies[n].newp = vector_add(_bodies[n].p, vector_scale(_bodies[n].v, dt));
		}
		
		// Update positions
		for (int n=0; n<body_count; n++)
			_bodies[n].p = _bodies[n].newp;
		
		// Record frame
		make_frame(&ani, f, _bodies, body_count);
		ani.time[f] = time;
	}
	
	// Free local bodies
	free(_bodies);
	
	return ani;
}


Animation simulate_average(Body *bodies, unsigned body_count, double dt, double duration)
{
	// Allocate animation
	Animation ani;
	ani.frame_count = duration/dt;
	ani.body_count = body_count;
	alloc_animation(&ani);
	
	// Copy Bodies
	Body *_bodies = calloc(sizeof(Body), body_count);
	for (int n=0; n<body_count; n++)
		_bodies[n]=bodies[n];	
	
	
	// First plot
	make_frame(&ani, 0, _bodies, body_count);
	
	// Cycle through frames
	double time = 0.0;
	for (int f=1; f<ani.frame_count; f++) {
		
		// Step time
		time += dt;
		
		// Cycle through bodies
		for (int n=0; n<body_count; n++) {
			
			// Find Acceleration
			Vector3d accel1;
			accel1.x = accel1.y = accel1.z = 0.0;
			
			// Effect of each body
			for (int m=0; m<body_count; m++) {
				
				// Don't compare same body
				if (m==n) continue;
				
				// Find m relative to n
				Vector3d rel = vector_subtract(_bodies[n].p, _bodies[m].p);
				
				// Find new velocity under gravity
				double mag = vector_magnitude(rel);
				accel1 = vector_subtract(accel1,
										 vector_scale(vector_normal(rel),
													  GRAV_CONST
													  *_bodies[m].mass
													  /mag/mag));
			}
			
			// Find new position
			_bodies[n].newp = vector_add(_bodies[n].p,
										 vector_scale(vector_add(_bodies[n].v,
																 vector_scale(accel1,
																			  dt)),
													  dt));
			
			// Find acceleration at new position
			Vector3d accel2;
			accel2.x = accel2.y = accel2.z = 0.0;
			for (int m=0; m<body_count; m++) {
				
				// Don't compare same body
				if (m==n) continue;
				
				// Find m relative to n
				Vector3d rel = vector_subtract(_bodies[n].newp, _bodies[m].p);
				
				// Find new velocity under gravity
				double mag = vector_magnitude(rel);
				accel2 = vector_subtract(accel2,
										 vector_scale(vector_normal(rel),
													  GRAV_CONST
													  *_bodies[m].mass
													  /mag/mag));
			}
			
			// Find average acceleration
			Vector3d accel_avg;
			accel_avg.x = accel_avg.y = accel_avg.z = 0.0;
			accel_avg = vector_scale(vector_add(accel1, accel2), 0.5);
			
			// Find new velocity
			_bodies[n].v = vector_add(_bodies[n].v, vector_scale(accel_avg, dt));
			
			// Find new position
			_bodies[n].newp = vector_add(_bodies[n].p,
										 vector_scale(_bodies[n].v,dt));
			
		}
		
		// Update positions
		for (int n=0; n<body_count; n++)
			_bodies[n].p = _bodies[n].newp;
		
		// Record frame
		make_frame(&ani, f, _bodies, body_count);
		ani.time[f] = time;
	}
	
	// Free local bodies
	free(_bodies);
	
	return ani;
}

double highest_acceleration(Body *bodies, int body_count)
{
	double high_accel = 0.0;
	for (int n=0; n<body_count; n++) {
		
		Vector3d force;
		force.x = force.y = force.z = 0.0;
		for (int m=0; m<body_count; m++) {
			
			// Don't compare same body
			if (m==n) continue;
			
			// Find m relative to n
			Vector3d rel = vector_subtract(bodies[n].p, bodies[m].p);
			double relmag = vector_magnitude(rel);
			
			// Find force
			force = vector_subtract(force,
									vector_scale(vector_normal(rel),
												 GRAV_CONST
												 *bodies[m].mass
												 /relmag/relmag));
		}
		
		// Store if new highest
		double force_mag = vector_magnitude(force);
		if (force_mag > high_accel) high_accel = force_mag;
		
	}
	return high_accel;
}

Animation simulate_adaptive(Body *bodies,
							unsigned body_count,
							double dt_a,
							double duration)
{
	// Allocate animation
	Animation ani;
	// Rough estimate of number of frames needed
	double combined_square_mass = 0.0;
	for (int n=0; n<body_count; n++) combined_square_mass+=bodies[n].mass*bodies[n].mass;
	ani.frame_count = duration/dt_a*highest_acceleration(bodies,body_count);
	printf("estimate:%i\n",ani.frame_count);
	ani.body_count = body_count;
	alloc_animation(&ani);
	
	// Copy Bodies
	Body *_bodies = calloc(sizeof(Body), body_count);
	for (int n=0; n<body_count; n++)
		_bodies[n]=bodies[n];	
	
	
	// First plot
	make_frame(&ani, 0, _bodies, body_count);
	ani.time[0] = 0.0;
	
	
	// Loop till done
	double time = 0.0;
	int f = 1;
	while (time < duration) {
		
		// If not enough frames allocate more
		if (f>=ani.frame_count) {
			ani.frame_count += (duration-time)*f/time+1;
			realloc_animation(&ani);
		}
		
		// Find highest force
		double high_accel = highest_acceleration(_bodies,body_count);
		
		// Find adaptive dt
		//printf("force:%f\n",high_force);
		double dt = dt_a/high_accel;
		
		// Step time
		time += dt;
		
		// Cycle through bodies
		for (int n=0; n<body_count; n++) {
			
			// Effect of each body
			for (int m=0; m<body_count; m++) {
				
				// Don't compare same body
				if (m==n) continue;
				
				// Find m relative to n
				Vector3d rel = vector_subtract(_bodies[n].p, _bodies[m].p);
				
				// Find new velocity under gravity
				double mag = vector_magnitude(rel);
				_bodies[n].v = vector_subtract(_bodies[n].v,
											   vector_scale(vector_normal(rel),
															GRAV_CONST
															*_bodies[m].mass
															*dt/mag/mag));
			}
			
			//Find new positions
			_bodies[n].newp = vector_add(_bodies[n].p, vector_scale(_bodies[n].v, dt));
		}
		
		// Update positions
		for (int n=0; n<body_count; n++)
			_bodies[n].p = _bodies[n].newp;
		
		// Record frame
		make_frame(&ani, f, _bodies, body_count);
		ani.time[f] = time;
		f++;
	}
	
	// Trim spare memory
	ani.frame_count = f;
	realloc_animation(&ani);
	
	// Free local bodies
	free(_bodies);
	
	return ani;
}
/*
double highest_accel_gradient_dot_velocity(Body *bodies, int body_count)
{
	double high_grad_dot_vel = 0.0;
	for (int n=0; n<body_count; n++) {
		
		Vector3d accel_grad;
		accel_grad.x = accel_grad.y = accel_grad.z = 0.0;
		for (int m=0; m<body_count; m++) {
			
			// Don't compare same body
			if (m==n) continue;
			
			// Find m relative to n
			Vector3d rel = vector_subtract(bodies[n].p, bodies[m].p);
			double relmag = vector_magnitude(rel);
			
			// Find accel
			accel_grad = vector_add(accel_grad,
							   vector_scale(vector_normal(rel),
											2*GRAV_CONST
											*bodies[m].mass
											/relmag/relmag/relmag));
		}
		
		// Store if new highest
		double accel_grad_dot_vel = fabs(vector_dot(accel_grad, bodies[n].v));
		if (accel_grad_dot_vel > high_grad_dot_vel) high_grad_dot_vel = accel_grad_dot_vel;
		
	}
	return high_grad_dot_vel;
}*/
double highest_accel_gradient_cross_velocity(Body *bodies, int body_count)
{
	double high_grad_cross_vel = 0.0;
	for (int n=0; n<body_count; n++) {
		
		Vector3d accel_grad;
		accel_grad.x = accel_grad.y = accel_grad.z = 0.0;
		for (int m=0; m<body_count; m++) {
			
			// Don't compare same body
			if (m==n) continue;
			
			// Find m relative to n
			Vector3d rel = vector_subtract(bodies[n].p, bodies[m].p);
			double relmag = vector_magnitude(rel);
			
			// Find accel
			accel_grad = vector_add(accel_grad,
									vector_scale(rel,
												 2*GRAV_CONST
												 *bodies[m].mass
												 /relmag/relmag));
		}
		
		// Store if new highest
		double accel_grad_cross_vel = fabs(vector_magnitude(vector_cross(accel_grad,bodies[n].v)));
		if (accel_grad_cross_vel > high_grad_cross_vel) high_grad_cross_vel = accel_grad_cross_vel;
		
	}
	return high_grad_cross_vel;
}

Animation simulate_adaptive_by_force_gradient(Body *bodies,
											  unsigned body_count,
											  double dt_gradA_cross_v,
											  double min_dt,
											  double max_dt,
											  double duration)
{
	// Allocate animation
	Animation ani;
	// Rough estimate of number of frames needed
	double combined_square_mass = 0.0;
	for (int n=0; n<body_count; n++) combined_square_mass+=bodies[n].mass*bodies[n].mass;
	double initial_dt = duration/dt_gradA_cross_v*highest_accel_gradient_cross_velocity(bodies,body_count);
	if (initial_dt < min_dt) initial_dt = min_dt;
	else if (initial_dt > max_dt) initial_dt = max_dt;
	ani.frame_count = duration/initial_dt;
	printf("estimate:%i\n",ani.frame_count);
	ani.body_count = body_count;
	alloc_animation(&ani);
	
	// Copy Bodies
	Body *_bodies = calloc(sizeof(Body), body_count);
	for (int n=0; n<body_count; n++)
		_bodies[n]=bodies[n];	
	
	
	// First plot
	make_frame(&ani, 0, _bodies, body_count);
	ani.time[0] = 0.0;
	
	
	// Loop till done
	double time = 0.0;
	int f = 1;
	while (time < duration) {
		
		// If not enough frames allocate more
		if (f>=ani.frame_count) {
			ani.frame_count += (duration-time)*f/time+1;
			realloc_animation(&ani);
		}
		
		// Find highest force
		double high_gradA_cross_vel = highest_accel_gradient_cross_velocity(_bodies,body_count);
		//printf("high:%f\n",high_gradA_dot_vel);
		
		// Find adaptive dt
		double dt = dt_gradA_cross_v/high_gradA_cross_vel;
		if (dt < min_dt) {
			dt = min_dt;
			ani.min_hits++;
			printf("hit min dt at frame %i\n",f);
		}
		else if (dt > max_dt) {
			dt = max_dt;
			ani.max_hits++;
			printf("hit max dt at frame %i\n",f);
		}
		
		
		// Step time
		time += dt;
		
		// Cycle through bodies
		for (int n=0; n<body_count; n++) {
			
			// Effect of each body
			for (int m=0; m<body_count; m++) {
				
				// Don't compare same body
				if (m==n) continue;
				
				// Find m relative to n
				Vector3d rel = vector_subtract(_bodies[n].p, _bodies[m].p);
				
				// Find new velocity under gravity
				double mag = vector_magnitude(rel);
				_bodies[n].v = vector_subtract(_bodies[n].v,
											   vector_scale(vector_normal(rel),
															GRAV_CONST
															*_bodies[m].mass
															*dt/mag/mag));
			}
			
			//Find new positions
			_bodies[n].newp = vector_add(_bodies[n].p, vector_scale(_bodies[n].v, dt));
		}
		
		// Update positions
		for (int n=0; n<body_count; n++)
			_bodies[n].p = _bodies[n].newp;
		
		// Record frame
		make_frame(&ani, f, _bodies, body_count);
		ani.time[f] = time;
		f++;
	}
	
	// Trim spare memory
	ani.frame_count = f;
	realloc_animation(&ani);
	
	// Free local bodies
	free(_bodies);
	
	return ani;
}

void make_frame(Animation *ani, unsigned frame, Body *bodies, unsigned body_count)
{
	// Linear momentum
	Vector3d lin_mom;
	// Not sure if initialisation is necessary
	lin_mom.x=0.0; lin_mom.y=0.0; lin_mom.z=0.0;
	for (int n=0; n<body_count; n++) {
		double m = bodies[n].mass;
		lin_mom.x += bodies[n].v.x*m;
		lin_mom.y += bodies[n].v.y*m;
		lin_mom.z += bodies[n].v.z*m;
	}
	ani->linear_momentum[frame] = lin_mom;
	
	// Angular momentum
	Vector3d ang_mom;
	// Not sure if initialisation is necessary
	ang_mom.x=0.0; ang_mom.y=0.0; ang_mom.z=0.0;
	for (int n=0; n<body_count; n++) {
		ang_mom = vector_add(ang_mom,
							 vector_cross(bodies[n].p,
												 vector_scale(bodies[n].v,
															  bodies[n].mass)));
	}
	ani->angular_momentum[frame] = ang_mom;
	
	// Kinetic energy
	double tot_kinetic = 0;
	for (int n=0; n<body_count; n++) {
		double vel = vector_magnitude(bodies[n].v);
		tot_kinetic += 0.5*bodies[n].mass*vel*vel;
	}
	ani->kinetic_energy[frame] = tot_kinetic;
	
	// Potential energy
	double tot_potential = 0;
	for (int n=0; n<body_count; n++) {
		for (int m=n+1; m<body_count; m++) {
			tot_potential -= GRAV_CONST*bodies[n].mass*bodies[m].mass
			*vector_magnitude(vector_subtract(bodies[n].p, bodies[m].p));
		}
	}
	ani->potential_energy[frame] = tot_potential;
	
	ani->orbital_energy[frame] = tot_kinetic-tot_potential;
	
	
	// Coordinates
	for (int n=0; n<body_count; n++) {
		Vector3d *b = ani->bodies[n];
		b[frame] = bodies[n].p;
	}
}

void alloc_animation(Animation *ani)
{
	// frames for time
	ani->time = malloc(sizeof(double) * ani->frame_count);
	// frames for all bodies
	ani->bodies = malloc(sizeof(Vector3d*) * ani->body_count);
	for (int n=0; n<ani->body_count; n++)
		ani->bodies[n] = malloc(sizeof(Vector3d) * ani->frame_count);
	// frames for linear momentum
	ani->linear_momentum = malloc(sizeof(Vector3d) * ani->frame_count);
	// frames for angular momentum
	ani->angular_momentum = malloc(sizeof(Vector3d) * ani->frame_count);
	// frames for orbital energy
	ani->orbital_energy = malloc(sizeof(double) * ani->frame_count);
	// frames for kinetic energy
	ani->kinetic_energy = malloc(sizeof(double) * ani->frame_count);
	// frames for potential energy
	ani->potential_energy = malloc(sizeof(double) * ani->frame_count);
}

void realloc_animation(Animation *ani)
{
	printf("realloc:%i\n",ani->frame_count);
	// frames for time
	ani->time = realloc(ani->time, sizeof(double) * ani->frame_count);
	// frames for all bodies
	ani->bodies = realloc(ani->bodies, sizeof(Vector3d*) * ani->body_count);
	for (int n=0; n<ani->body_count; n++)
		ani->bodies[n] = realloc(ani->bodies[n], sizeof(Vector3d) * ani->frame_count);
	// frames for linear momentum
	ani->linear_momentum = realloc(ani->linear_momentum, sizeof(Vector3d) * ani->frame_count);
	// frames for angular momentum
	ani->angular_momentum = realloc(ani->angular_momentum, sizeof(Vector3d) * ani->frame_count);
	// frames for orbital energy
	ani->orbital_energy = realloc(ani->orbital_energy, sizeof(double) * ani->frame_count);
	// frames for kinetic energy
	ani->kinetic_energy = realloc(ani->kinetic_energy, sizeof(double) * ani->frame_count);
	// frames for potential energy
	ani->potential_energy = realloc(ani->potential_energy, sizeof(double) * ani->frame_count);
}

void free_animation(Animation *ani)
{
	free(ani->time);
	for (int n=0; n<ani->body_count; n++)
		free(ani->bodies[n]);
	free(ani->bodies);
	free(ani->linear_momentum);
	free(ani->angular_momentum);
	free(ani->orbital_energy);
	free(ani->kinetic_energy);
	free(ani->potential_energy);
}