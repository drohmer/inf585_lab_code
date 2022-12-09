#pragma once

#include "cgp/cgp.hpp"
#include "../environment.hpp"

// Stores the buffers representing the cloth vertices
struct cloth_structure
{    
    // Buffers are stored as 2D grid that can be accessed as grid(ku,kv)
    cgp::grid_2D<cgp::vec3> position;  
    cgp::grid_2D<cgp::vec3> velocity;  
    cgp::grid_2D<cgp::vec3> force;
    cgp::grid_2D<cgp::vec3> normal;

    // Also stores the triangle connectivity used to update the normals
    cgp::numarray<cgp::uint3> triangle_connectivity;

    
    void initialize(int N_samples_edge);  // Initialize a square flat cloth
    void update_normal();       // Call this function every time the cloth is updated before its draw
    int N_samples_edge() const; // Number of vertex along one dimension of the grid
};


// Helper structure and functions to draw a cloth
// ********************************************** //
struct cloth_structure_drawable
{
    cgp::mesh_drawable drawable;

    void initialize(int N_sample_edge);
    void update(cloth_structure const& cloth);
};

void draw(cloth_structure_drawable const& cloth_drawable, environment_generic_structure const& environment);
void draw_wireframe(cloth_structure_drawable const& cloth_drawable, environment_generic_structure const& environment);
