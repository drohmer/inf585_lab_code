#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"


#include "deformation.hpp"


// The element of the GUI that are not already stored in other structures
struct gui_parameters {
	bool display_frame = true;
	bool display_wireframe = false;
	bool arap = false;
};

struct constraint_selection_structure {
	std::set<int> temporary_idx;   // Indices of position currently in the selection cursor (before releasing the mouse)
	cgp::vec2 p0;                  // Screen position of the first corner of the selection
	cgp::vec2 p1;                  // Screen position of the second corner of the selection
	bool selection_mode = false;   // Switch between selection mode and displacement
	bool active;
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

	// Surface data
	cgp::mesh shape;
	cgp::numarray<cgp::vec3> initial_position;
	cgp::numarray<cgp::numarray<int>> one_ring;

	// Least-square data
	//linear_system_structure linear_system;

	// Constraints applied to the vertices
	constraint_structure constraints;

	// Visual helper
	cgp::curve_drawable curve_selection;
	cgp::mesh_drawable visual;
	cgp::mesh_drawable sphere;

	constraint_selection_structure constraint_selection;
	linear_system_structure linear_system;
	bool surface_need_update;
	cgp::timer_event_periodic timer_shape_update;


	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop

	void display_constraints();
	void display_selection_rectangle();

	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

};





