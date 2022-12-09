#pragma once

#include "cgp/cgp.hpp"

// Particle structure 
struct particle_structure
{
	cgp::vec3 p0;  // Initial position
	cgp::vec3 v0;  // Initial velocity
	float t0;      // Time of birth

	// Create a particle at its initial position
	particle_structure(float creation_time=0);
	// Evaluate the current position of the particle at the current time passed as argument
	cgp::vec3 evaluate_position(float absolute_time) const;
};

struct particle_system_structure
{
	// Storage of the particles
	std::vector<particle_structure> particles; 

	// Store the trajectory of the particle used for visualization purpose
	std::vector<cgp::trajectory_drawable> trajectory;


	// Create and add a new particle in the vector
	void create_new_particle(float current_time);

	// Remove particles that are too old from the vector
	void remove_old_particles(float current_time);
};