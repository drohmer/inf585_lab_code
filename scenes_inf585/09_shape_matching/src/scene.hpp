#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"
#include "simulation/simulation.hpp"
#include "deformable/deformable.hpp"

using cgp::mesh_drawable;



// struct shape_deformable_structure {
// 	//cgp::mesh shape_mesh_reference;
	

// 	vec3 com;
// 	vec3 com_reference;


// 	numarray<vec3> position;
// 	numarray<vec3> position_predict;
// 	numarray<vec3> position_reference;
// 	numarray<vec3> velocity;

// 	numarray<uint3> connectivity;
// 	numarray<vec3> normal;
// 	cgp::mesh_drawable drawable;

// 	void initialize(cgp::mesh const& shape);

	

// 	int size() const{return position.size();}

// 	void update_drawable() {
// 		drawable.vbo_position.update(position);
// 		normal_per_vertex(position, connectivity, normal);
// 		drawable.vbo_normal.update(normal);
// 	}
// };

enum primitive_type_enum {primitive_cube, primitive_cylinder, primitive_cone, primitive_bunny, primitive_spot};

struct gui_parameters {
	bool display_frame = true;
	bool display_wireframe = true;
	bool display_collision_sphere = false;
	bool display_walls = true;
	primitive_type_enum primitive_type;
	float throwing_speed = 10.0f;
	
};

// The structure of the custom scene
struct scene_structure : scene_inputs_generic {
	
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
	cgp::timer_basic timer;

	simulation_parameter param;
	std::vector<shape_deformable_structure> deformables;
	void add_new_deformable_shape(vec3 const& center, vec3 const& velocity, vec3 const& angular_velocity, vec3 const& color);

	mesh_drawable sphere;
	mesh_drawable wall;
	void throw_new_deformable_shape();


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





