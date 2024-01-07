#pragma once

#include "cgp/cgp.hpp"

using cgp::numarray;
using cgp::mesh_drawable;
using cgp::mesh;
using cgp::vec3;

struct blend_shape_structure {

	numarray<mesh> faces_storage; // Store all initial key-frame faces
	mesh_drawable face;              // Face currently displayed
	numarray<float> weights;           // Blend Shapes weights

	void initialize();                    // Load the faces from file and initialize the mesh_drawable
	void update();                        // Update new face geometry corresponding to the weights values

};
