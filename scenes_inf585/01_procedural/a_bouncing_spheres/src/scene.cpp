#include "scene.hpp"


using namespace cgp;

void scene_structure::initialize()
{

	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());
	

	// Initialize the shapes of the scene
	// ***************************************** //

	float sphere_radius = 0.05f;
	sphere.initialize_data_on_gpu(mesh_primitive_sphere(sphere_radius));
	sphere.material.color = { 0.5f,0.8f,1.0f };

	ground.initialize_data_on_gpu(mesh_primitive_disc(1.0f, { 0,0,-sphere_radius }, { 0,0,1 }, 60) );

	timer.event_period = 0.5f;

}



void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	// Handle elements of the scene
	// ***************************************** //
	timer.update(); // update the timer to the current elapsed time

	// Display the ground
	draw(ground, environment);
	if (gui.display_wireframe) 
		draw_wireframe(ground, environment);

	// if there is a periodic event, insert a new particle
	if (timer.event)
		particle_system.create_new_particle(timer.t);
	particle_system.remove_old_particles(timer.t);
	
	// Evaluate the positions and display the particles
	int const N = particle_system.particles.size();
	for (int k = 0; k < N; ++k)
	{
		// Current particle
		particle_structure& particle = particle_system.particles[k];

		// Evaluate the current position of the particle
		vec3 const p = particle.evaluate_position(timer.t);

		// Display the particle as a sphere
		sphere.model.translation = p;
		draw(sphere, environment);
		if (gui.display_wireframe)
			draw_wireframe(sphere, environment);

		// Update and display the trajectory if needed
		if (gui.display_trajectory) {
			particle_system.trajectory[k].add(p);
			draw(particle_system.trajectory[k], environment);
		}
	}
}


void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

	ImGui::Checkbox("Trajectory", &gui.display_trajectory);
	ImGui::SliderFloat("Creation interval", &timer.event_period, 0.05f, 1.0f);
}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
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