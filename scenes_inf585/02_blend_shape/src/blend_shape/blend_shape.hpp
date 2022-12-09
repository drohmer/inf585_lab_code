#pragma once


#include "cgp/cgp.hpp"

struct blend_shape_structure {

	cgp::numarray<cgp::mesh> faces_storage; // Store all initial key-frame faces
	cgp::mesh_drawable face;              // Face currently displayed
	cgp::numarray<float> weights;           // Blend Shapes weights

	void initialize();                    // Load the faces from file and initialize the mesh_drawable
	void update();                        // Update new face geometry corresponding to the weights values

	
};
