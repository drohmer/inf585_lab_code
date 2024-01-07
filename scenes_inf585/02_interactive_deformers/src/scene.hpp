#pragma once
/**
	Objective: Implement the Blend Shape deformation on the character face
	- Observe the relation between the change of weights by the sliders (blend_shapes_sliders) and the call of the function update_blend_shape
	- Add the necessary precomputation in the function initialize, and the update of the deformed face in update_blend_shape.
*/


#include "cgp/cgp.hpp"

#include "environment.hpp"

#include "helpers/gui.hpp"
#include "helpers/initialization.hpp"
#include "helpers/picking_visual.hpp"

#include "deformers/deformers.hpp"

using cgp::mesh_drawable;


// Helping structure that contains the deforming shape elements
struct deforming_shape_structure
{
	cgp::mesh shape;                         // Mesh structure of the deformed shape
	cgp::numarray<cgp::vec3> position_saved; // Extra storage of the shape position before the current deformation
	cgp::mesh_drawable visual;               // Visual representation of the deformed shape

	bool require_normal_update;              // indicator if the normals need to be updated

	void update_normal();
	void new_shape(surface_type_enum type_of_surface = surface_plane);
};




// The structure of the custom scene
struct scene_structure : scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment; // Standard environment controler
	input_devices inputs; // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                       // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	deforming_shape_structure deforming_shape;

	cgp::picking_structure picking;
	picking_visual_parameters picking_visual;
	cgp::timer_event_periodic timer_update_normal; // timer with periodic events used to update the normals



	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();  // Standard initialization to be called before the animation loop
	void display_frame();     // The frame display to be called within the animation loop
	void display_gui(); // The display of the GUI, also called within the animation loop


	void mouse_move_event();
	void mouse_click_event();
	void mouse_scroll_event();
	void keyboard_event();
	void idle_frame();

};





