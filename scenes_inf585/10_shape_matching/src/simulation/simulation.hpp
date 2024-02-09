#include "../deformable/deformable.hpp"

struct simulation_parameter
{
    // Radius around each vertex considered as a colliding sphere
    float collision_radius = 0.04f;

    // Ratio considered for plastic deformation on the reference shape \in [0,1]	
	float plasticity = 0.0f;
    // Ratio considered for elastic deformation
    float elasticity = 0.0f;
    // Velocity reduction at each time step (* dt);
    float friction = 1.0f;
    // Numer of collision handling step for each numerical integration
    int collision_steps = 5;


    // Time step of the numerical time integration
	float time_step = 0.005f;
};

void simulation_step(std::vector<shape_deformable_structure>& deformables, simulation_parameter const& param);

