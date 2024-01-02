#include "scene.hpp"


using namespace cgp;

void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_y();
	camera_control.look_at({6,3,1},{0,0,0});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// Initialize the shapes of the scene
	// ***************************************** //

	sphere.initialize_data_on_gpu(mesh_primitive_sphere(1.0f));
	sphere.material.color = { 0.5f,0.8f,1.0f };

	float const L = 0.35f; //size of the quad
	billboard.initialize_data_on_gpu(mesh_primitive_quadrangle({ -L,-L,0 }, { L,-L,0 }, { L,L,0 }, { -L,L,0 }));
	billboard.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/smoke.png");

	// Load 3D model of pot
	cooking_pot.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/cauldron.obj"));
	cooking_pot.material.color = { 0.9f, 0.8f, 0.6f };
	cooking_pot.model.translation = { -0.1f, -0.3f, 0.0f };
	cooking_pot.model.scaling = 0.43f;

	// Load 3D model of spoon
	spoon.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/spoon.obj"));
	spoon.material.color = { 0.9f, 0.8f, 0.6f };
	spoon.model.translation = { -0.1f, -0.3f, 0 };
	spoon.model.scaling = 0.43f;

	// Create the flat surface representing the liquid in the pot
	liquid_surface.initialize_data_on_gpu(mesh_primitive_disc(0.73f, { 0,0,0 }, { 0,1,0 }, 60));
	liquid_surface.material.color = { 0.5f,0.6f,0.8f };
	liquid_surface.material.phong = { 0.7f, 0.3f, 0.0f, 128 };


	timer_bubble.event_period = 0.2f;
	timer_billboard.event_period = 0.05f;


}

void scene_structure::display_bubble()
{
	// Evaluate the positions and display the bubbles
	int const N = particle_system.bubbles.size();
	for (int k = 0; k < N; ++k)
	{
		// Current particle
		particle_bubble& particle = particle_system.bubbles[k];
		// Evaluate the current position of the particle
		vec3 const p = particle.evaluate_position(timer_bubble.t);

		sphere.model.translation = p;
		sphere.model.scaling = particle.radius;
		sphere.material.color = particle.color;

		draw(sphere, environment);
	}
}

void scene_structure::display_billboard()
{
	// Enable transparency using alpha blending (if display_transparent_billboard is true)	
	if (gui.display_transparent_billboard) {
		glEnable(GL_BLEND);
		glDepthMask(false);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
		glDisable(GL_BLEND);


	// Evaluate the positions and display the billboards
	int const N = particle_system.billboards.size();
	for (int k = 0; k < N; ++k)
	{
		// Display the sprites here
		//  Note : to make the sprite constantly facing the camera set
		//    billboard.model.rotation = camera_control.camera_model.orientation();
		//  ...
		/*  temporary code to remove or adapt */ billboard.model.translation = { 0.0, k * 0.6, 0.0f };
		draw(billboard, environment);
	}

	glDepthMask(true);
}



void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	// Static elements
	draw(spoon, environment);
	draw(cooking_pot, environment);
	draw(liquid_surface, environment);

	// Timers
	timer_bubble.update();
	timer_billboard.update();

	// Handling creation/remove of particles
	if (timer_bubble.event)
		particle_system.bubbles.push_back(particle_bubble(timer_bubble.t));
	if (timer_billboard.event)
		particle_system.billboards.push_back(particle_billboard(timer_billboard.t));

	particle_system.remove_old_particles(timer_bubble.t);

	display_bubble();
	display_billboard();
}


void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Transparent billboard", &gui.display_transparent_billboard);
	ImGui::SliderFloat("Bubble creation", &timer_bubble.event_period, 0.05f, 2.0f);
	ImGui::SliderFloat("Smoke creation", &timer_billboard.event_period, 0.01f, 0.5f);


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