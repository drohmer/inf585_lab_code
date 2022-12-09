#include "scene.hpp"


using namespace cgp;

void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_y();
	camera_control.look_at({ 0.5f,7.0f,2.0f }, { 0,6.5,0 });
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// Initialize the shapes of the scene
	// ***************************************** //
	std::cout << " Load body ... " << std::endl;
	body.initialize_data_on_gpu(mesh_load_file_obj(project::path+"assets/body.obj"));

	blend_shape.initialize(); // Load the faces
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


	// Display the elements
	if (gui.display_face)
		draw(blend_shape.face, environment);
	if (gui.display_wireframe)
		draw_wireframe(blend_shape.face, environment, { 0,0,1 });
	if (gui.display_body)
		draw(body, environment);
}


void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	
	ImGui::Checkbox("Face", &gui.display_face); ImGui::SameLine();
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
	ImGui::Checkbox("Body", &gui.display_body);


	// Slider of the GUI
	bool const slider_01 = ImGui::SliderFloat("w1", &blend_shape.weights[0], 0.0f, 1.0f);
	bool const slider_02 = ImGui::SliderFloat("w2", &blend_shape.weights[1], 0.0f, 1.0f);
	bool const slider_03 = ImGui::SliderFloat("w3", &blend_shape.weights[2], 0.0f, 1.0f);
	bool const slider_04 = ImGui::SliderFloat("w4", &blend_shape.weights[3], 0.0f, 1.0f);
	bool const slider_05 = ImGui::SliderFloat("w5", &blend_shape.weights[4], 0.0f, 1.0f);
	// return values slider_0x are true if the slider is modified, otherwise they are false.

	// If one of the slider of the GUI is modified, then call the function update_blend_shape
	if (slider_01 || slider_02 || slider_03 || slider_04 || slider_05)
		blend_shape.update();

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