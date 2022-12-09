#include "scene.hpp"

#include "helpers/grid_helper.hpp"
#include "helpers/initialization.hpp"
#include "ffd/ffd.hpp"


using namespace cgp;




void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_y();
	camera_control.look_at({ 3.0f, 2.0f, 2.0f }, {0,0,0}, {0,0,1});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// First initialization of the grid
	int const Nx = 4, Ny = 4, Nz = 4;
	grid = initialize_grid(Nx, Ny, Nz);
	update_grid_segments(grid_segments, grid);

	// Initialization of the visual elements - grid edges and spheres
	grid_segments_visual.display_type = curve_drawable_display_type::Segments;
	grid_segments_visual.initialize_data_on_gpu(grid_segments);
	grid_vertex_sphere.initialize_data_on_gpu(mesh_primitive_sphere(0.02f));

	// Initialize the surface shape
	create_new_surface();

	// Update delay between two consecutive computation of FFD
	timer_update_ffd.event_period = 0.05f;
	update_required = false;
}


void scene_structure::create_new_surface()
{
	// The detail of the initialization functions are in the helper file initialization.cpp
	switch (gui.surface_type)
	{
	case surface_cylinder:
		shape = initialize_cylinder();
		break;
	case surface_sphere:
		shape = initialize_sphere();
		break;
	case surface_cube:
		shape = initialize_cube();
		break;
	case surface_mesh:
		shape = initialize_mesh();
		break;
	}

	shape_visual.clear();
	shape_visual.initialize_data_on_gpu(shape);

	grid = initialize_grid(grid.dimension.x, grid.dimension.y, grid.dimension.z);

	weights.clear();
	// TO DO: Fill this function
	weights = precompute_weights(shape.position, grid.dimension.x, grid.dimension.y, grid.dimension.z);

	update_required = true;
}



void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);


	// Update of elements and FFD computation
	// **************************************** //

	// Update the grid position if a vertex has been displaced
	if (update_required) {
		update_grid_segments(grid_segments, grid);
		grid_segments_visual.vbo_position.update(grid_segments);
	}

	// Update the FFD too (but at limited rate to avoid too much computation)
	timer_update_ffd.update();
	if (timer_update_ffd.event == true && update_required)
	{
		// TO DO: Fill this function
		ffd_deform(shape.position, grid, weights);

		shape.normal_update();
		shape_visual.vbo_position.update(shape.position);
		shape_visual.vbo_normal.update(shape.normal);

		update_required = false;
	}

	display_grid();

	if (gui.surface)
		draw(shape_visual, environment);
	if (gui.wireframe)
		draw_wireframe(shape_visual, environment, { 0.8f,0.2f,0.1f });
}

void scene_structure::display_gui()
{
	bool new_surface = display_interface(gui);
	if (new_surface)
		create_new_surface();
}

void scene_structure::display_grid()
{
	if (gui.display_grid_sphere) {
		int const Nx = grid.dimension.x;
		int const Ny = grid.dimension.y;
		int const Nz = grid.dimension.z;
		for (int kx = 0; kx < Nx; ++kx) {
			for (int ky = 0; ky < Ny; ++ky) {
				for (int kz = 0; kz < Nz; ++kz) {
					grid_vertex_sphere.model.translation = grid(kx, ky, kz);

					if (picking.active && grid.index_to_offset(kx, ky, kz) == picking.index)
						grid_vertex_sphere.material.color = { 1,0,0 };
					else
						grid_vertex_sphere.material.color = { 0,0,1 };

					draw(grid_vertex_sphere, environment);
				}
			}
		}
	}

	if (gui.display_grid_edge)
		draw(grid_segments_visual, environment);
}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);

	// The picking and deformation is only applied when pressing the shift key
	if (inputs.keyboard.shift)
	{
		// Current position of the mouse
		vec2 const& p = inputs.mouse.position.current;

		// If the mouse is not clicked, compute a picking on the vertices of the grid
		if (!inputs.mouse.click.left)
			picking = picking_spheres(p, grid.data, 0.03f, camera_control.camera_model, camera_projection);

		// Grid point translation
		if (inputs.mouse.click.left && picking.active)
		{
			vec3 const new_grid_position = picking_plane_orthogonal_to_camera(p, picking.position, camera_control.camera_model, camera_projection).position;
			grid.data[picking.index] = new_grid_position;
			update_required = true;
		}
	}
	else
		picking.active = false;

}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}

