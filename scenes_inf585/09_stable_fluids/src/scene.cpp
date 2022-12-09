#include "scene.hpp"


#include "helper/helper.hpp"
#include "simulation/simulation.hpp"

using namespace cgp;




void scene_structure::initialize()
{
	camera_projection = camera_projection_orthographic{ -1.1f, 1.1f, -1.1f, 1.1f, -10, 10, window.aspect_ratio()};
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.look_at({ 0.0f, 0.0f, 3.0f }, {0,0,0}, {0,1,0});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// Initialize the shapes of the scene
	// ***************************************** //
	initialize_fields(gui.density_type);
	int const N = velocity.dimension.x;
	initialize_density_visual(density_visual, N);
	density_visual.texture.initialize_texture_2d_on_gpu(density);
	initialize_grid(grid_visual, N);
	grid_visual.color = { 0,0,0.5 };

	velocity_grid_data.resize(2 * N * N);
	velocity_visual.initialize_data_on_gpu(velocity_grid_data);
	velocity_visual.color = vec3(0, 0, 0);

	grid_visual.display_type = curve_drawable_display_type::Segments;
	velocity_visual.display_type = curve_drawable_display_type::Segments;
}

void scene_structure::simulate(float dt)
{
	velocity_previous = velocity;
	density_previous = density;

	// velocity
	diffuse(velocity, velocity_previous, gui.diffusion_velocity, dt, reflective); velocity_previous = velocity;
	divergence_free(velocity, velocity_previous, divergence, gradient_field); velocity_previous = velocity;
	advect(velocity, velocity_previous, velocity_previous, dt);

	// density
	if (gui.density_type != view_velocity_curl) {
		diffuse(density, density_previous, gui.diffusion_density, dt, copy); density_previous = density;
		advect(density, density_previous, velocity, dt);
	}
	else // in case you directly look at the velocity curl (no density advection in this case)
		density_to_velocity_curl(density, velocity);
}

void scene_structure::initialize_density(density_type_structure density_type, size_t N)
{
	if (density_type == density_color) {
		initialize_density_color(density, N);
	}

	if (density_type == density_texture) {
		convert(image_load_png(project::path+"assets/texture.png"), density);
	}

	if (density_type == view_velocity_curl) {
		density.resize(N, N); density.fill({ 1,1,1 });
	}

	density_previous = density;
}

void scene_structure::initialize_fields(density_type_structure density_type)
{
	size_t const N = 60;
	velocity.resize(N, N); velocity.fill({ 0,0 }); velocity_previous = velocity;
	initialize_density(density_type, N);
	divergence.clear(); divergence.resize(N, N);
	gradient_field.clear(); gradient_field.resize(N, N);

}

void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	timer.update();
	float const dt = 0.2f * timer.scale;
	simulate(dt);
	density_visual.texture.update(density);
	update_velocity_visual(velocity_visual, velocity_grid_data, velocity, gui.velocity_scaling);

	draw(density_visual, environment);

	if (gui.display_grid)
		draw(grid_visual, environment);

	if (gui.display_velocity)
		draw(velocity_visual, environment);

}

void scene_structure::display_gui()
{
	ImGui::SliderFloat("Timer scale", &timer.scale, 0.01f, 4.0f, "%0.2f");
	ImGui::SliderFloat("Diffusion Density", &gui.diffusion_density, 0.001f, 0.2f, "%0.3f", 2.0f);
	ImGui::SliderFloat("Diffusion Velocity", &gui.diffusion_velocity, 0.001f, 0.2f, "%0.3f", 2.0f);
	ImGui::Checkbox("Grid", &gui.display_grid); ImGui::SameLine();
	ImGui::Checkbox("Velocity", &gui.display_velocity);
	ImGui::SliderFloat("Velocity scale", &gui.velocity_scaling, 0.1f, 10.0f, "0.2f");

	bool const cancel_velocity = ImGui::Button("Cancel Velocity"); ImGui::SameLine();
	bool const restart = ImGui::Button("Restart");

	bool new_density = false;
	int* ptr_density_type = reinterpret_cast<int*>(&gui.density_type);
	new_density |= ImGui::RadioButton("Density color", ptr_density_type, density_color); ImGui::SameLine();
	new_density |= ImGui::RadioButton("Density texture", ptr_density_type, density_texture); ImGui::SameLine();
	new_density |= ImGui::RadioButton("Velocity Curl", ptr_density_type, view_velocity_curl);
	if (new_density || restart)
		initialize_density(gui.density_type, velocity.dimension.x);
	if (cancel_velocity || restart)
		velocity.fill({ 0,0 });
}

void scene_structure::mouse_move_event()
{
	vec2 const& p = inputs.mouse.position.current;
	if (inputs.mouse.click.left) {
		velocity_track.add(vec3(p, 0.0f), timer.t);
		mouse_velocity_to_grid(velocity, velocity_track.velocity.xy(), camera_projection.matrix_inverse(), p);
	}
	else {
		velocity_track.set_record(vec3(p, 0.0f), timer.t);
	}
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

