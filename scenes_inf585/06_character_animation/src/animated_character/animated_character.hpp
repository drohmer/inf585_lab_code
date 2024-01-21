#pragma once

#include "cgp/cgp.hpp"
#include "skeleton_structure/skeleton_structure.hpp"
#include "skeleton_drawable/skeleton_drawable.hpp"
#include "skeleton_animation/skeleton_animation.hpp"
#include "animated_model/animated_model.hpp"
#include "asset_loader/asset_loader.hpp"

// General container for a animated character
//  Contains the rigged mesh and its skeleton, as well as helper structure to animate and display it.
struct character_structure {

	// The core container for the mesh, rig, skeleton (and animation)
	animated_model_structure animated_model;

	// The timer associated to the animation cycle of the character
	cgp::timer_event_periodic timer;

	// The name of the current active animation
	std::string current_animation_name;

	// Stores a mesh_drawable for each mesh of the rigged_character
	std::map<std::string, cgp::mesh_drawable> drawable;
	// The drawable structure to display a skeleton
	skeleton_drawable sk_drawable;


	// Use this method to set a new animation 
	//  - Change the name of the current animation
	//  - Update the timer
	void set_current_animation(std::string const& animation_name);

	// Load a new character structure from files using the dedicated loader
	void load_and_initialize(filename_loader_structure const& param_loader, cgp::affine_rts const& transform=cgp::affine_rts());

};