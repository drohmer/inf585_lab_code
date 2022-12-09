#pragma once

#include "cgp/cgp.hpp"

#include "environment.hpp"
#include "particles/particles.hpp"

using cgp::mesh_drawable;



struct gui_parameters {
	bool display_frame      = true;
	bool display_wireframe  = false;
	bool display_trajectory = true;
};



// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit_euler camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;         // The standard global frame
	environment_structure environment;  // Standard environment controler
	input_devices inputs;               // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                 // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	cgp::timer_event_periodic timer;

	particle_system_structure particle_system;     // The container of the active particles
	mesh_drawable sphere;     // Sphere used to represent the particle
	mesh_drawable ground;     // Visual representation of the ground

	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();  // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui(); // The display of the GUI, also called within the animation loop


	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

};





