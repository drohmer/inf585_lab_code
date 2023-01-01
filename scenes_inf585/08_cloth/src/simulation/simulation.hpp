#pragma once

#include "cgp/cgp.hpp"
#include "../cloth/cloth.hpp"
#include "../constraint/constraint.hpp"


struct simulation_parameters
{
    float dt = 0.005f;        // time step for the numerical integration
    float mass_total = 0.5f; // total mass of the cloth
    float K = 5.0f;         // stiffness parameter
    float mu = 15.0f;        // damping parameter

    //  Wind magnitude and direction
    struct {
        float magnitude = 0.0f;
        cgp::vec3 direction = { 0,-1,0 };
    } wind;
};


// Fill the forces in the cloth given the position and velocity
void simulation_compute_force(cloth_structure& cloth, simulation_parameters const& parameters);

// Perform 1 step of a semi-implicit integration with time step dt
void simulation_numerical_integration(cloth_structure& cloth, simulation_parameters const& parameters, float dt);

// Apply the constraints (fixed position, obstacles) on the cloth position and velocity
void simulation_apply_constraints(cloth_structure& cloth, constraint_structure const& constraint);

// Helper function that tries to detect if the simulation diverged 
bool simulation_detect_divergence(cloth_structure const& cloth);