#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"

#include "animated_character/animated_character.hpp"
#include "effects/effects.hpp"


using cgp::mesh_drawable;


struct gui_parameters {
	bool display_frame = false;
	bool display_wireframe = false;
	bool display_surface = true;
	bool display_ground = true;
	bool display_texture = true;
	bool display_skeleton = true;
	bool display_skeleton_joint_sphere = false;
	bool display_skeleton_joint_frame = false;
	bool display_skeleton_bone = true;
	bool rotate_head_effect_active = false;
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

	std::map<std::string, character_structure> characters;
	std::string current_active_character;


	std::map<std::string, effect_transition_structure> effect_transition;	
	effect_walking_structure effect_walk;
	effect_ik_structure effect_ik;


	mesh_drawable sphere_ik;
	mesh_drawable ground;

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





