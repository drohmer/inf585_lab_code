#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"

#include "skeleton/skeleton.hpp"
#include "skeleton/skeleton_drawable.hpp"
#include "skinning/skinning.hpp"

using cgp::mesh_drawable;


// The element of the GUI that are not already stored in other structures
struct gui_parameters {
	bool display_frame = false;

	bool surface_skinned = true;
	bool wireframe_skinned = false;
	bool surface_rest_pose = false;
	bool wireframe_rest_pose = false;

	bool skeleton_current_bone = true;
	bool skeleton_current_frame = false;
	bool skeleton_current_sphere = false;

	bool skeleton_rest_pose_bone = false;
	bool skeleton_rest_pose_frame = false;
	bool skeleton_rest_pose_sphere = false;
};

struct visual_shapes_parameters
{
	cgp::mesh_drawable surface_skinned;
	cgp::mesh_drawable surface_rest_pose;
	cgp::skeleton_drawable skeleton_current;
	cgp::skeleton_drawable skeleton_rest_pose;
};

struct skinning_current_data
{
	cgp::numarray<cgp::vec3> position_rest_pose;
	cgp::numarray<cgp::vec3> position_skinned;
	cgp::numarray<cgp::vec3> normal_rest_pose;
	cgp::numarray<cgp::vec3> normal_skinned;

	cgp::numarray<cgp::affine_rt> skeleton_current;
	cgp::numarray<cgp::affine_rt> skeleton_rest_pose;
};



// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit_euler camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	cgp::timer_interval timer;
	visual_shapes_parameters visual_data;
	cgp::skeleton_animation_structure skeleton_data;
	cgp::rig_structure rig;
	skinning_current_data skinning_data;


	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop

	void compute_deformation();
	void update_new_content(cgp::mesh const& shape, cgp::opengl_texture_image_structure texture_id);

	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

};





