#pragma once

#include "cgp/cgp.hpp"

// Structure storing the data for the deformable structure simulation
//  The structure stores the current deformed model parameters (position, normal, velocity, etc.), and parameter of the reference/initial one in order to compute the shape matching.
struct shape_deformable_structure {

    // Center of mass of the deformed shape
	cgp::vec3 com;
    // Center of mass of the reference shape
	cgp::vec3 com_reference;

    // Positions of the deformed shape
	cgp::numarray<cgp::vec3> position;
    // Predicted positions of the deformed shape (used to apply the PPD constraints before updating the velocity)
	cgp::numarray<cgp::vec3> position_predict;
    // Positions of the reference shape
	cgp::numarray<cgp::vec3> position_reference;

    // Velocity of the deformed shape
	cgp::numarray<cgp::vec3> velocity;

    // Normals of the deformed shape
	cgp::numarray<cgp::vec3> normal;
    // Connectivity of the mesh (used to recompute the per-vertex normals)
	cgp::numarray<cgp::uint3> connectivity;
    // The drawable element representing the deformed shape
	cgp::mesh_drawable drawable;


    // Initialize a reference structure from a mesh
	void initialize(cgp::mesh const& shape);

    // Set an initial translation and velocity to the deformed shape and update the com
    void set_position_and_velocity(cgp::vec3 translation, cgp::vec3 linear_velocity, cgp::vec3 angular_velocity);

    // Returns the number of positions
	int size() const;

    // Update the position and normals to the vbo of the drawable structure
	void update_drawable();

};