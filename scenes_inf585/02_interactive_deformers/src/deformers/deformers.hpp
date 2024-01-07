#pragma once


#include "cgp/cgp.hpp"


enum deformer_type_enum {
	deform_translate,
	deform_twist,
	deform_scale
};

enum deformer_direction_enum {
	direction_view_space,
	direction_surface_normal
};

struct deformer_parameters_structure {
	deformer_type_enum type           = deform_translate;      // Type of deformation type
	deformer_direction_enum direction = direction_view_space;  // Type of deformation direction
	float falloff                     = 1 / 5.0f;              // Falloff/influence distance (can be adjusted from the GUI or with mouse scroll)
};



void apply_deformation(cgp::mesh& shape, cgp::numarray<cgp::vec3> const& position_before_deformation, cgp::vec2 const& translate_screen, cgp::vec3 const& picked_position, cgp::vec3 const& picked_normal, cgp::rotation_transform const& camera_orientation, deformer_parameters_structure const& deformer_parameters);