#include "scene.hpp"


using namespace cgp;


static int offset(int ku, int kv, int Nv)
{
	return kv + Nv * ku;
}

void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({ 3.0f, 2.0f, 2.0f }, {0,0,0}, {0,0,1});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());


	sphere.initialize_data_on_gpu(mesh_primitive_sphere());
	curve_selection.initialize_data_on_gpu({ vec3{0,0,0},vec3{0,0,0},vec3{0,0,0},vec3{0,0,0},vec3{0,0,0} } );

	const int N = 15;
	shape = mesh_primitive_grid({ -1,-1,0 }, { 1,-1,0 }, { 1,1,0 }, { -1,1,0 }, N, N);
	initial_position = shape.position;
	one_ring = connectivity_one_ring(shape.connectivity);

	constraints.fixed[offset(0, 0, N)] = shape.position[offset(0, 0, N)];
	constraints.fixed[offset(N - 1, 0, N)] = shape.position[offset(N - 1, 0, N)];
	constraints.target[offset(0, N - 1, N)] = shape.position[offset(0, N - 1, N)];
	constraints.target[offset(N - 1, N - 1, N)] = shape.position[offset(N - 1, N - 1, N)];
	visual.initialize_data_on_gpu(shape);

	build_matrix(linear_system, constraints, shape, initial_position, one_ring);
	update_deformation(linear_system, constraints, shape, visual, initial_position, one_ring);

	timer_shape_update.event_period = 0.3f;
}



void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	// Update surface
	// ***************************************** //
	if (surface_need_update) {
		timer_shape_update.update(); // used to limit the frequency of the update (to limit the computations)
		if (timer_shape_update.event) {
			update_deformation(linear_system, constraints, shape, visual, initial_position, one_ring);
			surface_need_update = false;
		}
	}



	display_constraints();

	draw(visual, environment);
	if (gui.display_wireframe)
		draw_wireframe(visual, environment);

}

void scene_structure::display_constraints()
{
	// Display fixed positional constraints
	sphere.model.scaling = 0.05f;
	sphere.material.color = { 0,0,1 };
	for (const auto& c : constraints.fixed) {
		const int k = c.first;
		sphere.model.translation = shape.position[k];
		draw(sphere, environment);
	}

	// Display temporary constraints in yellow
	sphere.material.color = { 1,1,0 };
	for (int idx : constraint_selection.temporary_idx) {
		sphere.model.translation = shape.position[idx];
		draw(sphere, environment);
	}

	if (constraint_selection.selection_mode)
		display_selection_rectangle();

	// Display target constraints
	for (const auto& c : constraints.target) {
		const int k = c.first;
		const vec3& p = c.second;

		// The real target position in white
		sphere.material.color = { 1,1,1 };
		sphere.model.scaling = 0.04f;
		sphere.model.translation = p;
		draw(sphere, environment);

		// The real position in red
		sphere.material.color = { 1,0,0 };
		sphere.model.scaling = 0.05f;
		sphere.model.translation = shape.position[k];
		draw(sphere, environment);
	}
}

void scene_structure::display_selection_rectangle()
{
	mat4 C = camera_control.camera_model.matrix_frame();
	mat4 Pinv = camera_projection.matrix_inverse();

	vec3 const d0 = camera_ray_direction(C, Pinv, constraint_selection.p0);
	vec3 const d1 = camera_ray_direction(C, Pinv, vec2{ constraint_selection.p0.x, constraint_selection.p1.y });
	vec3 const d2 = camera_ray_direction(C, Pinv, constraint_selection.p1);
	vec3 const d3 = camera_ray_direction(C, Pinv, vec2{ constraint_selection.p1.x, constraint_selection.p0.y });

	vec3 const p = camera_control.camera_model.position();

	vec3 const p0 = p + d0;
	vec3 const p1 = p + d1;
	vec3 const p2 = p + d2;
	vec3 const p3 = p + d3;

	curve_selection.vbo_position.update( numarray<vec3>{ p0, p1, p2, p3, p0 } );
	draw(curve_selection, environment);
}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

	bool change_active_weight = ImGui::SliderFloat("Weight Fixed", &constraints.weight_fixed, 0.05f, 10.0f, "%.3f", 3);
	bool change_passive_weight = ImGui::SliderFloat("Weight Target", &constraints.weight_target, 0.05f, 10.0f, "%.3f", 3);
	ImGui::Checkbox("Select constraint", &constraint_selection.selection_mode);

	if (change_active_weight || change_passive_weight) {
		build_matrix(linear_system, constraints, shape, initial_position, one_ring);
		surface_need_update = false;
		update_deformation(linear_system, constraints, shape, visual, initial_position, one_ring);
	}


}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);

	if (inputs.keyboard.shift)
	{
		if (inputs.mouse.click.left || inputs.mouse.click.right) 
		{
			constraint_selection.p1 = inputs.mouse.position.current;

			if (constraint_selection.selection_mode) {
				constraint_selection.temporary_idx.clear();

				// Handle the squared selection
				float const x_min = std::min(constraint_selection.p0.x, constraint_selection.p1.x);
				float const x_max = std::max(constraint_selection.p0.x, constraint_selection.p1.x);
				float const y_min = std::min(constraint_selection.p0.y, constraint_selection.p1.y);
				float const y_max = std::max(constraint_selection.p0.y, constraint_selection.p1.y);

				mat4 const T = camera_projection.matrix() * camera_control.camera_model.matrix_view();
				int const N = shape.position.size();

				for (int k = 0; k < N; ++k)
				{
					// Compute projection coordinate of each vertex
					vec4 p_screen = T * vec4(shape.position[k], 1.0f);
					p_screen /= p_screen.w;

					// Check if the projected coordinates are within the screen box
					float const x = p_screen.x;
					float const y = p_screen.y;
					if (x > x_min && x < x_max && y > y_min && y < y_max)
						constraint_selection.temporary_idx.insert(k); // add a new possible constraint in the selection
				}
			}
			else
			{
				// Otherwise can displace constraints using shift + drag and drop
				vec2 const tr_2D = constraint_selection.p1 - inputs.mouse.position.previous; // translation in screen coordinates
				vec3 const tr = camera_control.camera_model.orientation() * vec3(tr_2D, 0.0f); // translation in 3D

				// Apply the translation to all target constraints
				for (auto& it : constraints.target) {
					vec3& p = it.second;
					p += tr;
				}
			}
			surface_need_update = true;
		}

	}
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);

	constraint_selection.p0 = inputs.mouse.position.current;
	constraint_selection.p1 = inputs.mouse.position.current;

	// Click in selection mode
	if (constraint_selection.selection_mode && inputs.keyboard.shift)
	{
		// Release left click add target constraints (convert constraints_temporary to target)
		if (inputs.mouse.click.last_action == last_mouse_cursor_action::release_left) {
			if (constraint_selection.temporary_idx.empty()) // Release the mouse with no position => clear all target constraints
				constraints.target.clear();
			else {
				// Otherwise add new target constraints
				for (int idx : constraint_selection.temporary_idx) {
					constraints.target[idx] = shape.position[idx];

					if (constraints.fixed.find(idx) != constraints.fixed.end())
						constraints.fixed.erase(idx); // Erase fixed constraints if it is now a target constraint
				}
			}
		}

		// Release right click add fixed constraints (convert constraints_temporary to fixed)
		if (inputs.mouse.click.last_action == last_mouse_cursor_action::release_right) {
			if (constraint_selection.temporary_idx.empty()) // Release the mouse with no position => clear all fixed constraints
				constraints.fixed.clear();
			else {
				// Otherwise add new fixed constraints
				for (int idx : constraint_selection.temporary_idx) {
					constraints.fixed[idx] = shape.position[idx];
					if (constraints.target.find(idx) != constraints.target.end())
						constraints.target.erase(idx); // Erase target constraints if it is now a fixed constraint
				}
			}
		}
	}

	// If mouse click/released in selection mode: rebuild the matrix
	if (inputs.keyboard.shift && constraint_selection.selection_mode) {
		build_matrix(linear_system, constraints, shape, initial_position, one_ring);
		surface_need_update = false;
		update_deformation(linear_system, constraints, shape, visual, initial_position, one_ring);
		constraint_selection.temporary_idx.clear();
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

