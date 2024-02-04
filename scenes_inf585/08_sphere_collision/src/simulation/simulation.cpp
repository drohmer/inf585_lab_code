#include "simulation.hpp"

using namespace cgp;




void simulate(std::vector<particle_structure>& particles, float dt)
{
	vec3 const g = { 0,0,-9.81f };
	size_t const N = particles.size();

	// Update velocity with gravity force and friction
	for (size_t k = 0; k < N; ++k)
	{
		particle_structure& particle = particles[k];
		vec3 const f = particle.m * g;

		particle.v = (1 - 0.9f * dt) * particle.v + dt * f / particle.m;
	}

	// **************************************** //
	// TO DO: Collision Handling
	// 
	// The global pseudo code may be the following:
	//  (you need to implement steps 2 and 3)
	// 
	//   1: Velocity update with force fields (already implemented)
	//   - For all particles:
	//     Update velocity using gravity and friction 
	//  
	//   2: Impulse response for bouncing effect
	//   - For all particles:
	//     Check collision between particles 
	//      Then update their velocities using impulse response
	//   - For all particles:
	//     Check collision between cube faces 
	//      Then update their velocities using impulse response
	//
	//   3: Cancel velocity components in penetration
	//   - For all particles:
	//     Check collision between particles 
	//      Then cancel velocity components going inside other particle
	//   - For all particles:
	//     Check collision between cube faces 
	//      Then cancel velocity components going outside the cube
	//
	//   4: Position update (already implemented)
	// 	 - For all particles:
	//     Update position using the velocity
	//
	//  Hints:
	//    - The cube is by default centered around zero, faces are between [-1,1]
	//      To compute the intersection between sphere and cube faces, you may store
	//      the normals, and face center positions of the cube in a vectors.
	//
	//    - Implement and check first the collision between the spheres and the cube
	//      before the collision between spheres
	//
	// **************************************** //

	// Update position from velocity
	for (size_t k = 0; k < N; ++k)
	{
		particle_structure& particle = particles[k];
		particle.p = particle.p + dt * particle.v;
	}

}
