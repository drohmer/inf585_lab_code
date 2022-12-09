#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"


enum density_type_structure { density_color, density_texture, view_velocity_curl };

// The element of the GUI that are not already stored in other structures
struct gui_parameters {
	bool display_grid = false;
	bool display_velocity = false;
	float diffusion_velocity = 0.001f;
	float diffusion_density = 0.005f;
	float velocity_scaling = 1.0f;
	density_type_structure density_type = density_color;
};

// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit camera_control;
	camera_projection_orthographic camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	cgp::timer_basic timer;

	cgp::grid_2D<cgp::vec3> density, density_previous;
	cgp::grid_2D<cgp::vec2> velocity, velocity_previous;
	cgp::grid_2D<float> divergence;
	cgp::grid_2D<float> gradient_field;

	cgp::mesh_drawable density_visual;
	cgp::curve_drawable grid_visual;
	cgp::curve_drawable velocity_visual;
	cgp::numarray<cgp::vec3> velocity_grid_data;
	cgp::velocity_tracker velocity_track;


	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop

	void simulate(float dt);
	void initialize_density(density_type_structure density_type, size_t N);
	void initialize_fields(density_type_structure density_type);

	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

};





