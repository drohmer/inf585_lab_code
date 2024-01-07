#include "scene.hpp"


using namespace cgp;



// The deformation is applied when the user move the mouse
void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);

	// The picking and deformation is only applied when pressing the shift key
	if (inputs.keyboard.shift)
	{
		// Current position of the mouse
		vec2 const& p = inputs.mouse.position.current;

		// If the mouse is not clicked, compute a picking on the vertices of the mesh
		if (!inputs.mouse.click.left)
			picking = picking_mesh_vertex_as_sphere(p, deforming_shape.shape.position, deforming_shape.shape.normal, 0.03f, camera_control.camera_model, camera_projection);


		// Apply Deformation: press on shift key + left click on the mouse when a vertex is already selected
		if (inputs.mouse.click.left && picking.active) {

			// Current translation in 2D window coordinates
			vec2 const translation_screen = p - picking.screen_clicked;

			// Apply the deformation on the surface
			apply_deformation(deforming_shape.shape, deforming_shape.position_saved, translation_screen, picking.position, picking.normal, camera_control.camera_model.orientation(), gui.deformer_parameters);

			// Update the visual model
			deforming_shape.visual.vbo_position.update(deforming_shape.shape.position);
			deforming_shape.require_normal_update = true;
		}

	}
	else
		picking.active = false; // Unselect picking when shift is released
}



void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.look_at({ 3.0f, 2.0f, 2.0f }, {0,0,0}, {0,0,1});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	timer_update_normal.event_period = 0.15f;
	deforming_shape.new_shape();
	picking_visual.initialize();
}



void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	// Display the deformed shape
	draw(deforming_shape.visual, environment);
	if (gui.display_wireframe)
		draw_wireframe(deforming_shape.visual, environment, { 0,0,0 });

	// Periodically update the normal
	//  Doesn't do it all the time as the computation is costly
	timer_update_normal.update();
	if (timer_update_normal.event && deforming_shape.require_normal_update == true)
		deforming_shape.update_normal();

	// Display of the circle of influence oriented along the local normal of the surface
	if (picking.active)
		picking_visual.draw(environment, deforming_shape.shape.position[picking.index], picking.normal, gui.deformer_parameters.falloff);

}


void scene_structure::display_gui()
{
	// Display the gui and update the new shape if requested
	bool const is_new_surface = gui.display();
	if (is_new_surface)
		deforming_shape.new_shape(gui.surface_type);
}


void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);

	if (inputs.mouse.click.last_action == last_mouse_cursor_action::release_left)
	{
		// Releasing the left click indicates the end of the deformation: disable the picking, save the new position and update the normals
		picking.active = false;
		deforming_shape.position_saved = deforming_shape.shape.position;
		deforming_shape.update_normal();
	}
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}
void scene_structure::mouse_scroll_event()
{
	// Increase/decrease falloff distance when scrolling the mouse
	if (picking.active)
		gui.deformer_parameters.falloff = std::max(gui.deformer_parameters.falloff + float(inputs.mouse.scroll) * 0.01f, 1e-6f);
}

void deforming_shape_structure::new_shape(surface_type_enum type_of_surface)
{

	// Create a new shape based on the gui indication type_of_surface
	// The detail of the initialization functions are in the file initialization.cpp
	switch (type_of_surface)
	{
	case surface_plane:
		shape = initialize_plane();
		break;
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

	
	// Clear previous surface before seting the values of the new one
	visual.clear();
	visual.initialize_data_on_gpu(shape);

	position_saved = shape.position;
	require_normal_update = false;


}

void deforming_shape_structure::update_normal()
{
	shape.normal_update();
	visual.vbo_normal.update(shape.normal);
	require_normal_update = false;
}