#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"

#include "animated_model/animated_model.hpp"
#include "skeleton_drawable/skeleton_drawable.hpp"

using cgp::mesh_drawable;


struct gui_parameters {
	bool display_frame = true;
	bool display_wireframe = false;
	bool display_surface = true;
	bool display_skeleton = true;
	float power_factor_skinning_weight = 3.0f;
	bool is_dual_quaternion = false;

	bool is_bending_motion = true;
	bool is_twisting_motion = false;
};

enum surface_model {cylinder, bar, spot};


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

	surface_model model_type;
	animated_model_structure model;
	mesh_drawable model_drawable;
	skeleton_drawable sk_drawable;
	cgp::timer_basic timer;

	void set_skinning_weights();
	void apply_rotation_to_joint(int joint, vec3 const& rotation_axis, float rotation_angle);
	void animate_skeleton();
	void initialize_model();

	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop


	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

};





