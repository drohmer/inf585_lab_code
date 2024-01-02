#pragma once

#include "cgp/cgp.hpp"


struct particle_bubble
{
	cgp::vec3 p0;  // Initial position
	float t0;      // Time of birth
	cgp::vec3 color;
	float radius;

	particle_bubble(float creation_time = 0);               // Create a bubble at its initial position
	cgp::vec3 evaluate_position(float absolute_time) const; // Evaluate its position at a given time

};

struct particle_billboard
{
	cgp::vec3 p0; // Initial position
	float t0;     // Time of birth

	particle_billboard(float creation_time = 0);            // Create a bubble at its initial position
	cgp::vec3 evaluate_position(float absolute_time) const; // Evaluate its position at a given time
};


// Storage of all the particles
struct particle_system_structure
{
	std::vector<particle_bubble> bubbles;
	std::vector<particle_billboard> billboards;

	void remove_old_particles(float current_time);
};