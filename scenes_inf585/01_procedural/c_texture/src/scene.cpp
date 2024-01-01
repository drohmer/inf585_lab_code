#include "scene.hpp"


using namespace cgp;




void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_y();
	camera_control.look_at({ 0.2f, 0.1f, 1.5f }, {0,0,0}, {0,0,1});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());
	gui.display_frame = false;

	environment.background_color = {0,0,0};
	quad.initialize_data_on_gpu(mesh_primitive_quadrangle());
	quad.model.translation = vec3(-0.5,-0.5,0.0);
	quad.shader.load(project::path+"shaders/animated_texture/animated_texture.vert.glsl",project::path+"shaders/animated_texture/animated_texture.frag.glsl");
	quad.material.phong.specular = 0;

}



void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	timer.update();
	environment.uniform_generic.uniform_float["time"] = timer.t;

	draw(quad, environment);

}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

	ImGui::Checkbox("DarkBackground", &gui.dark_background);
	if(gui.dark_background==true) {
		environment.background_color = vec3(0,0,0);
	}
	else {
		environment.background_color = vec3(1,1,1);
	}

	
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

