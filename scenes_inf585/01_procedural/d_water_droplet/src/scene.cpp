#include "scene.hpp"


using namespace cgp;

// The fixed max number of water droplet
//  Note that if you modify this number, you need to adapt the water shader
#define DROPLET_NUMBER 5


// Create a new water droplet
void scene_structure::new_water_droplet()
{
	vec4& droplet = droplet_param[droplet_index];

	// create random coordinates for initil (x0, y0)
	float x0 = (rand_uniform()-0.5)*3;
	float y0 = (rand_uniform()-0.5)*3;
	droplet[0] = x0;
	droplet[1] = y0;

	//Initial time
	droplet[2] = timer.t;

	// Indicate the droplet is active
	droplet[3] = 1;

	// Update current droplet index
	droplet_index = (droplet_index+1)%DROPLET_NUMBER;

}

void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// Specific parameters of the scene
	// ***************************************** //

	camera_control.camera_model.look_at({-2.0f,-3.0f,0.7f},{0,0,0}); // initial position of the camera
	environment.background_color = {0.9f, 0.9f, 0.9f}; // a foggy sky

	// Resolution of the water surface is Water_surface_sampling x Water_surface_sampling
	//  Change this number to adapt the resolution
	int Water_surface_sampling = 800;

	// Create the water surface as a uniform grid
	water.initialize_data_on_gpu(mesh_primitive_grid({0,0,0},{1,0,0},{1,1,0},{0,1,0},Water_surface_sampling,Water_surface_sampling));
	water.material.color = { 0.5f,0.6f,0.8f };
	water.material.phong = { 0.7f, 0.3f, 0.4f, 128 };

	// Load the dedicated shader for the water
	water.shader.load(project::path+"shaders/water/water.vert.glsl",project::path+"shaders/water/water.frag.glsl");

	// Set a skybox (for the reflection in the water)
	image_structure image_skybox_template = image_load_file(project::path+"assets/skybox.jpg");
	std::vector<image_structure> image_grid = image_split_grid(image_skybox_template, 4, 3);
	water.supplementary_texture["image_skybox"].initialize_cubemap_on_gpu(image_grid[1], image_grid[7], image_grid[5], image_grid[3], image_grid[10], image_grid[4]);
	environment.uniform_generic.uniform_mat3["skybox_rotation"] = rotation_transform::from_axis_angle({1,0,0},-3.14f/2.0f).matrix();

	// Set the color of the fog
	environment.uniform_generic.uniform_vec3["background_color"] = environment.background_color;

	// Prepare the water drop
	//  The max number of water drop is fixed
	droplet_param.resize(DROPLET_NUMBER);
	droplet_index = 0;
	
	// Preparation of the falling sphere
	falling_sphere.initialize_data_on_gpu(mesh_primitive_sphere(0.05f));
	falling_sphere.material.color = water.material.color;

}


void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();

	
	if (gui.display_frame)
		draw(global_frame, environment);


	// Update current time
	timer.update();


	// Send the current uniform values
	// ******************************* //

	// Current time
	environment.uniform_generic.uniform_float["time"] = timer.t; 
	// Height of the default wave using Perlin noise
	environment.uniform_generic.uniform_float["height_perlin_noise"] = gui.height_perlin_noise;

	// Additional controlers for the shading
	environment.uniform_generic.uniform_int["is_environment_map"] = gui.is_environment_map;
	environment.uniform_generic.uniform_int["is_fog"] = gui.is_fog;

	// Send the water droplet parameters to the shader as uniform (encoded as vec4)
	for(int k=0; k<DROPLET_NUMBER; ++k) {
		environment.uniform_generic.uniform_vec4["droplet_"+str(k)] = droplet_param[k];
	}

	// Dimension of the water surface
	water.model.scaling = gui.water_size;
	water.model.translation = -gui.water_size/2.0f*vec3(1,1,0);


	// Draw the water surface
	draw(water, environment);
	if(gui.display_wireframe) {
		draw_wireframe(water, environment);
	}



}


void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

	ImGui::SliderFloat("Height perlin noise", &gui.height_perlin_noise, 0.0f, 0.05f);
	ImGui::SliderFloat("Water surface size", &gui.water_size, 1.0f, 80.0f);
	ImGui::Checkbox("Environment map", &gui.is_environment_map);
	ImGui::Checkbox("Fog", &gui.is_fog);

	if(ImGui::Button("Water drop")) {
		new_water_droplet();
	}

	// Display all the droplet current "lifetime"
	for(int k=0; k<DROPLET_NUMBER; ++k) {
		std::string s = "droplet "+str(k);
		vec4 const& droplet = droplet_param[k];
		float v = clamp(timer.t-droplet[2],0.0f,5.0f)*droplet[3];
		ImGui::SliderFloat(s.c_str(),&v,0,5);
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

	// Create a new droplet if pressing on space
	if(inputs.keyboard.last_action.is_pressed(GLFW_KEY_SPACE)) {
		new_water_droplet();
	}
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
	
}