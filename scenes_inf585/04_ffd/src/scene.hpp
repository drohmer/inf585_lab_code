/**
	Objective: Implement a Free-Form-Deformation (using Bezier functions) on an abitrary mesh embedded in a grid that can be interactively manipulated
	- The grid structure and interactive manipulation is already coded
	- The deformation of the mesh in relation of the grid still have to be coded in the function ffd
	- Remarks:
	--- The actual FFD deformation is to be implemented in the file ffd/ffd.cpp
	--- The grid is set to be in [0,1] in its initial configuration. The initial mesh is also supposed to have coordinates between [0,1]. We can therefore assimilate the parameters (u,v,w)\in[0,1] of the Bezier polynomials to the global coordinates of the mesh positions in its initial configuration.
	--- FFD computation can be computationally costly - an automatic scheduling system limits the number of computation per second while manipulating the grid (see timer_update_shape)
	--- The FFD can be seen as a set of weights (depending on the initial mesh coordinates) multiplied by the current position of the grid. These weights can be stored as a numarray<grid_3D<float>> and accessed as weights[k_vertex](kx,ky,kz)
*/

#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"

#include "helpers/gui.hpp"


using cgp::mesh_drawable;



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

	// The 3D grid - accessed using grid(kx,ky,kz)
	cgp::grid_3D<cgp::vec3> grid;

	// The mesh deformed by the FFD
	cgp::mesh shape;

	//The mesh_drawable corresponding to shape
	cgp::mesh_drawable shape_visual;

	// The weights used to compute the FFD
	//   weights[k_vertex](i,j,k) = b_i[k_vertex]*b_j[k_vertex]*b_k[k_vertex]
	//   To be filled to complete the exercise
	cgp::numarray<cgp::grid_3D<float> > weights;

	// The storage of the edges representing the 3D grid
	cgp::numarray<cgp::vec3> grid_segments;
	// The visual representation of the edges of the grid
	cgp::curve_drawable grid_segments_visual;
	// The visual represention of the vertices of the grid
	cgp::mesh_drawable grid_vertex_sphere;

	// Boolean indicating of the FFD must be computed (and the shape updated)
	bool update_required;

	// An helping timer to limit the maximum frequency of the update (to limit heavy computation)
	cgp::timer_event_periodic timer_update_ffd;

	// The helper picking structure to manipulate the vertex of the grid
	cgp::picking_structure picking;



	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop

	void display_grid();       // Display the 3D grid
	void create_new_surface(); // Reinitialize a new surface

	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

};





