#pragma once

#include "cgp/cgp.hpp"
#include "../environment.hpp"

// Visual elements used to display the picked elements
struct picking_visual_parameters {
	cgp::mesh_drawable sphere;   // the sphere indicating which vertex is under the mouse
	cgp::curve_drawable circle;  // circle used to show the radius of influence of the deformation

	void initialize();
	void draw(environment_structure const& environment, cgp::vec3 const& position, cgp::vec3 const& normal, float radius);
};
