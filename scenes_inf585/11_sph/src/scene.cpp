#include "scene.hpp"


using namespace cgp;

void update_field_color(grid_2D<vec3>& field, numarray<particle_element> const& particles);


void scene_structure::initialize()
{
	camera_projection = camera_projection_orthographic{ -1.1f, 1.1f, -1.1f, 1.1f, -10, 10, window.aspect_ratio() };
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.look_at({ 0.0f, 0.0f, 2.0f }, {0,0,0}, {0,1,0});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	field.resize(30, 30);
	field_quad.initialize_data_on_gpu(mesh_primitive_quadrangle({ -1,-1,0 }, { 1,-1,0 }, { 1,1,0 }, { -1,1,0 }) );
	field_quad.material.phong = { 1,0,0 };
	field_quad.texture.initialize_texture_2d_on_gpu(field);

	initialize_sph();
	sphere_particle.initialize_data_on_gpu(mesh_primitive_sphere(1.0,{0,0,0},10,10));
	sphere_particle.model.scaling = 0.01f;
	curve_visual.color = { 1,0,0 };
	curve_visual.initialize_data_on_gpu(curve_primitive_circle());
}

void scene_structure::initialize_sph()
{
	// Initial particle spacing (relative to h)
	float const c = 0.7f;
	float const h = sph_parameters.h;


	// Fill a square with particles
	particles.clear();
	for (float x = h; x < 1.0f - h; x = x + c * h)
	{
		for (float y = -1.0f + h; y < 1.0f - h; y = y + c * h)
		{
			particle_element particle;
			particle.p = { x + h / 8.0 * rand_uniform(),y + h / 8.0 * rand_uniform(),0 }; // a zero value in z position will lead to a 2D simulation
			particles.push_back(particle);
		}
	}

}

void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	timer.update(); // update the timer to the current elapsed time
	float const dt = 0.005f * timer.scale;
	simulate(dt, particles, sph_parameters);


	if (gui.display_particles) {
		for (int k = 0; k < particles.size(); ++k) {
			vec3 const& p = particles[k].p;
			sphere_particle.model.translation = p;
			draw(sphere_particle, environment);
		}
	}

	if (gui.display_radius) {
		curve_visual.model.scaling = sph_parameters.h;
		for (int k = 0; k < particles.size(); k += 10) {
			curve_visual.model.translation = particles[k].p;
			draw(curve_visual, environment);
		}
	}

	if (gui.display_color) {
		update_field_color(field, particles);
		field_quad.texture.update(field);
		draw(field_quad, environment);
	}

}

void scene_structure::display_gui()
{
	ImGui::SliderFloat("Timer scale", &timer.scale, 0.01f, 4.0f, "%0.2f");

	bool const restart = ImGui::Button("Restart");
	if (restart)
		initialize_sph();

	ImGui::Checkbox("Color", &gui.display_color);
	ImGui::Checkbox("Particles", &gui.display_particles);
	ImGui::Checkbox("Radius", &gui.display_radius);
}

void update_field_color(grid_2D<vec3>& field, numarray<particle_element> const& particles)
{
	field.fill({ 1,1,1 });
	float const d = 0.1f;
	int const Nf = int(field.dimension.x);
	for (int kx = 0; kx < Nf; ++kx) {
		for (int ky = 0; ky < Nf; ++ky) {

			float f = 0.0f;
			vec3 const p0 = { 2.0f * (kx / (Nf - 1.0f) - 0.5f), 2.0f * (ky / (Nf - 1.0f) - 0.5f), 0.0f };
			for (size_t k = 0; k < particles.size(); ++k) {
				vec3 const& pi = particles[k].p;
				float const r = norm(pi - p0) / d;
				f += 0.25f * std::exp(-r * r);
			}
			field(kx, Nf - 1 - ky) = vec3(clamp(1 - f, 0, 1), clamp(1 - f, 0, 1), 1);
		}
	}
}

void scene_structure::mouse_move_event()
{
	// Do not mode the camera
	/* 
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
		*/
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

