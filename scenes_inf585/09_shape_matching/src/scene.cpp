#include "scene.hpp"


using namespace cgp;




void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({ 3.0f, 2.0f, 2.0f }, {0,0,0}, {0,0,1});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());


	// A sphere used to display the collision model
	sphere.initialize_data_on_gpu(mesh_primitive_sphere(1.0f,{0,0,0},10,5));

	// Display the colliding walls (and the floor)
	float e = -0.001f;
	mesh mesh_wall = mesh_primitive_quadrangle({-1,-1,e},{3,-1,e},{3,3,e},{-1,3,e});
	mesh_wall.push_back(mesh_primitive_quadrangle({-1,-1,e},{-1,-1,3},{3,-1,3},{3,-1,e}));
	mesh_wall.push_back(mesh_primitive_quadrangle({-1,-1,e},{-1,-1,3},{-1,3,3},{-1,3,e}));
	wall.initialize_data_on_gpu(mesh_wall);
	

	// Add a default deformable model
	add_new_deformable_shape(
		{0,0,1}, /* initial position */
		{0,0,0}, /* initial velocity */
		{0,0,0}, /* initial angular velocity */
		{1,1,1}  /* initial color */
	);
	
}



void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	timer.update();

	// Compute the simulation
	if(param.time_step>1e-6f){
		simulation_step(deformables, param);
	}


	// Display all the deformable shapes
	for(int k=0; k<deformables.size(); ++k) {
		deformables[k].update_drawable();
		draw(deformables[k].drawable);
		if(gui.display_wireframe){
			draw_wireframe(deformables[k].drawable);
		}
	}

	// Display the walls and the ground
	if(gui.display_walls)
		draw(wall);

	// Display the vertices with their colliding spheres (slower the display for many vertices)
	if(gui.display_collision_sphere){
		for(int k=0; k<deformables.size(); ++k) {
			sphere.model.scaling = param.collision_radius;
			for(int kv=0; kv<deformables[k].position.size(); ++kv) {
				sphere.model.translation = deformables[k].position[kv];
				draw(sphere, environment);
			}
		}
	}

}





void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);

	
	ImGui::Checkbox("Wireframe",&gui.display_wireframe);
	ImGui::Checkbox("Walls", &gui.display_walls);
	
	ImGui::Spacing();
	ImGui::Checkbox("Display collision spheres", &gui.display_collision_sphere);
	ImGui::SliderFloat("Collision radius", &param.collision_radius, 0.001f,0.1f);	

	ImGui::Spacing();
	ImGui::Text("Primitive type:"); // Select surface to be created
	int* ptr_primitive_type  = reinterpret_cast<int*>(&gui.primitive_type); // trick - use pointer to select enum
	ImGui::RadioButton("Cube",ptr_primitive_type, primitive_cube); ImGui::SameLine();
	ImGui::RadioButton("Cylinder",ptr_primitive_type, primitive_cylinder); ImGui::SameLine();
	ImGui::RadioButton("Cone",ptr_primitive_type, primitive_cone); ImGui::SameLine();
	ImGui::RadioButton("Bunny",ptr_primitive_type, primitive_bunny); ImGui::SameLine();
	ImGui::RadioButton("Spot",ptr_primitive_type, primitive_spot);

	ImGui::Spacing();
	ImGui::SliderFloat("Time step", &param.time_step, 0,0.01f,"%.5f",2.0f);
	ImGui::SliderInt("Collision steps", &param.collision_steps, 1,5);	
	ImGui::SliderFloat("Friction with air", &param.friction, 0.001f, 0.1f, "%.4f", 2);
	ImGui::SliderFloat("Elasticity", &param.elasticity, 0,1);	
	ImGui::SliderFloat("Plasticity", &param.plasticity, 0,1);	

	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	ImGui::SliderFloat("Speed new shape",&gui.throwing_speed,0.0f, 40.0f);
	ImGui::PushStyleColor(ImGuiCol_Text,IM_COL32(255, 100, 100, 255));
	if(ImGui::Button("Add deformable shape (SPACE)")) {
		throw_new_deformable_shape();
	}
	ImGui::PopStyleColor();

}


// Generate a new deformable shape appearing in front of the camera with an initial velocity
void scene_structure::throw_new_deformable_shape()
{
	// Set its position in from of the camera
	vec3 p0 = camera_control.camera_model.position();
	// Give an initial linear velocity going in front of the camera. Its speed s is set via the gui.throwing speed parameter.
	float s = gui.throwing_speed;
	vec3 v0 = s * camera_control.camera_model.front();
	// Set a random initial angular velocity
	vec3 angular_velocity = vec3(rand_uniform(-s,s),rand_uniform(-s,s),rand_uniform(-s,s));

	// A lookup table for the color
	static std::vector<vec3> color_lut={{1,0.5,0.5},{0.5,1,0.5},{0.5,0.5,1},{1,1,0.5},{1,0.5,1},{0.5,1,1}};
	vec3 color = color_lut[int(rand_uniform(0,color_lut.size()))];

	// Create the new deformable shape
	add_new_deformable_shape(p0,v0,angular_velocity,color);
}

void scene_structure::add_new_deformable_shape(vec3 const& center, vec3 const& velocity, vec3 const& angular_velocity, vec3 const& color)
{
	
	// Initialize default primitive mesh
	mesh m;
	switch(gui.primitive_type){
		case primitive_cube: {
			float L = 0.1;
			int N_sample = 5;
			m = mesh_primitive_cubic_grid({-L,-L,-L},{L,-L,-L},{L,L,-L},{-L,L,-L},{-L,-L,L},{L,-L,L},{L,L,L},{-L,L,L}, N_sample,N_sample,N_sample);
			m.color.fill(color);
			break;
		}
		case primitive_cylinder: {
			int N_sample = 10;
			float L = 0.1f;
			float r = 0.1f;
			m = mesh_primitive_cylinder(r, {-L,0,0}, {L,0,0}, N_sample, 2*N_sample);
			m.color.fill(color);
			break;
		}
		case primitive_cone: {
			float L = 0.1;
			int N_sample = 8;
			m = mesh_primitive_cone(L,2*L,{0,0,0},{0,0,1},false,2*N_sample,8);
			m.color.fill(color);
			break;
		}
		case primitive_bunny: {
			m = mesh_load_file_obj(project::path+"assets/bunny.obj");
			m.scale(1.5f);
			m.flip_connectivity();
			m.color.fill(color);
			break;
		}
		case primitive_spot: {
			m = mesh_load_file_obj(project::path+"assets/spot.obj");
			m.scale(0.25f);
			break;
		}
	}

	// center the mesh in case it is not already
	m.centered(); 
	

	// Create a deformable structure from the mesh
	shape_deformable_structure deformable;
	deformable.initialize(m);
	deformable.set_position_and_velocity(center, velocity, angular_velocity);

	// Special case for spot: set the texture
	if(gui.primitive_type==primitive_spot) {
		deformable.drawable.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/spot_texture.png");
	}

	// Add the new deformable structure
	deformables.push_back(deformable);
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

	if(inputs.keyboard.last_action.is_pressed(GLFW_KEY_SPACE)) {
		throw_new_deformable_shape();
	}
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}

