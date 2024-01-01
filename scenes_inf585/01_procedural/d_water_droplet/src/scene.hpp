#pragma once


#include "cgp/cgp.hpp"

#include "environment.hpp"

using cgp::mesh_drawable;



struct gui_parameters {
	bool display_frame     = true;
	bool display_wireframe = false;

	float height_perlin_noise = 0.015f;
	float water_size = 25.0f;
	bool is_environment_map = true;
	bool is_fog = true;
};

// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_first_person_euler camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment; // Standard environment controler
	input_devices inputs; // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                       // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	cgp::timer_basic timer;

	// Create a new water droplet
	void new_water_droplet();

	// The surface of water
	mesh_drawable water;

	// The structure storing the initial parameter for the droplet effects
	//  The vec4 structure is used as follows:
	//    .x: the initial position in x-coord
	//    .y: the initial position in y-coord
	//    .z: the initial time of the droplet creation
	//    .w: indicate if the droplet has been created. Equals 0 initially, then 1 once created
	std::vector<vec4> droplet_param;
	// The current droplet index in the array of droplet_param
	int droplet_index;

	// A sphere used to represent the falling droplet
	mesh_drawable falling_sphere;


	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();  // Standard initialization to be called before the animation loop
	void display_frame();     // The frame display to be called within the animation loop
	void display_gui(); // The display of the GUI, also called within the animation loop

	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

};





