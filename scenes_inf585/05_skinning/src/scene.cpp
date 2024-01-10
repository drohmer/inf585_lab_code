#include "scene.hpp"

using namespace cgp;


numarray<numarray<float> > compute_skinning_weights_generic(cgp::mesh const& m, skeleton_structure const& skeleton, float power_factor) 
{
	currently_unused(power_factor);

	int N_joint = skeleton.size();	
	int N_vertex = m.position.size();

	numarray<numarray<float> > weights; // weights stores their value for each vertex and each joint: value = weights[k_vertex][k_joint]
	weights.resize(N_vertex);
	for(int k_vertex=0; k_vertex<N_vertex; ++k_vertex) {
		weights[k_vertex].resize(N_joint);
	}
	// at this stage, the skinning weights are all zeros

	// ************************************************************** //
	// TO DO: Compute some skinning weights for an arbitrary skeleton structure and a mesh in its bind pose.
	// ...
	// ************************************************************** //
	//
	// Help:
	//   - skeleton.child(k_joint) returns an array that contains the indices of the children of the joint k_joint
	//   - skeleton.joint_matrix_global_bind_pose[k_joint] returns the mat4 corresponding to the joint frame in global reference frame (in the bind pose)
	//   - {mat4}.get_block_translation() return a vec3 corresponding to the translation part of the affine transformation: vec3(mat4[0][3],mat4[1][3],mat4[2][3])
	//   - std::pow(x, p) computes x^p (x to the power of p). p can be an arbitrary float value.
	//   - You can use the syntax sum({numarray}) to compute the sum over all the values cgp::numarray
	//
	
	// The following create a skinning weights that rigidly attach all the vertices to the first (root) joint and need to be changed.
	for(int k_vertex=0; k_vertex<N_vertex; ++k_vertex) {
		weights[k_vertex][0] = 1.0f; // rigid attachement (value=1) to the root joint (index 0)
	}

	return weights;

}


numarray<numarray<float> > compute_skinning_weights_cylinder(cgp::mesh const& m, skeleton_structure const& skeleton, float power_factor)
{
	int N_vertex = m.position.size();
	numarray<numarray<float> > skinning_weight;
	skinning_weight.resize(N_vertex);
	
	for(int k=0; k<N_vertex; ++k) {
		skinning_weight[k].resize(skeleton.parent_index.size());

		float x = m.position[k].x;
		float w0, w1;

		if (x < 0.5f) {
			w1 = 0.5f*std::pow(std::abs(x/0.5f), power_factor);
			w0 = 1-w1;
		}
		else {
			w0 = 0.5f*std::pow(std::abs(1-(x-0.5f)/0.5f), power_factor);
			w1 = 1-w0;
		}

		skinning_weight[k][0] = w0;
		skinning_weight[k][1] = w1;
		skinning_weight[k][2] = 0;
	}
	return skinning_weight;
}


animated_model_structure create_cylinder();
animated_model_structure create_spot(); 
animated_model_structure create_bar();

void scene_structure::initialize_model()
{
	if(model_type==cylinder) {
		model = create_cylinder();
	}
	if(model_type==bar) {
		model = create_bar();
	}
	if(model_type==spot) {
		model = create_spot();
	}

	set_skinning_weights();

	model_drawable.clear();
	model_drawable.initialize_data_on_gpu(model.rigged_mesh.mesh_deformed);
	sk_drawable.clear();
	sk_drawable = skeleton_drawable(model.skeleton);

	if(model_type==spot) {
		model_drawable.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/spot/texture.png");
	}
}


void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_y();
	camera_control.look_at({ 3.0f, 2.0f, 2.0f }, {0,0,0}, {0,0,1});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());
	
	model_type = cylinder;

	initialize_model();

}

void scene_structure::set_skinning_weights()
{
	if(model_type==cylinder || model_type==bar) {
		model.rigged_mesh.skinning_weight = compute_skinning_weights_cylinder(model.rigged_mesh.mesh_bind_pose, model.skeleton, gui.power_factor_skinning_weight);
	}
	if(model_type==spot) {
		model.rigged_mesh.skinning_weight = compute_skinning_weights_generic(model.rigged_mesh.mesh_bind_pose, model.skeleton, gui.power_factor_skinning_weight);
	}
}

void scene_structure::apply_rotation_to_joint(int joint, vec3 const& rotation_axis, float rotation_angle) {
	rotation_transform R = rotation_axis_angle(normalize(rotation_axis), rotation_angle);
	mat4& T = model.skeleton.joint_matrix_local[joint];

	T.set_block_linear( R*mat3(T) );
}

void scene_structure::animate_skeleton() {
	model.skeleton.joint_matrix_local = model.skeleton.joint_matrix_local_bind_pose;
	if(model_type==cylinder || model_type==bar) {
		
		if(gui.is_twisting_motion) {
			apply_rotation_to_joint(1, {1,0,0}, Pi*(1+std::cos(4*timer.t))/2.0f);
		}

		if(gui.is_bending_motion) {
			apply_rotation_to_joint(1, {0,1,0}, Pi/2*std::cos(timer.t));
		}
	}

	if(model_type==spot) {
		// joint 3 = neck
		apply_rotation_to_joint(3, {0,1,0}, Pi/6.0f*std::cos(4*timer.t));
		apply_rotation_to_joint(3, {0,0,1}, Pi/6.0f*std::cos(timer.t));

		// joint 12 = tail
		apply_rotation_to_joint(12, {0,0,1}, Pi/12.0f*std::cos(10*timer.t));

		// joint 10, 11: front legs
		apply_rotation_to_joint(10, {0,0,1}, Pi/20.0f*std::sin(5*timer.t));
		apply_rotation_to_joint(11, {0,0,1}, Pi/20.0f*std::sin(5*timer.t));

		// joint 13, 14: back legs
		apply_rotation_to_joint(13, {0,0,1}, Pi/20.0f*std::cos(5*timer.t));
		apply_rotation_to_joint(14, {0,0,1}, Pi/20.0f*std::cos(5*timer.t));

		// For joint correspondance: check the file assets/spot/skeleton_joint_name.txt
	}

}

void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	timer.update();

	animate_skeleton();
	model.skeleton.update_joint_matrix_local_to_global();



	
	model.skinning_lbs();
	if(gui.is_dual_quaternion) {
		model.skinning_dqs();
	}

	model_drawable.vbo_position.update(model.rigged_mesh.mesh_deformed.position);
	model_drawable.vbo_normal.update(model.rigged_mesh.mesh_deformed.normal);

	sk_drawable.update(model.skeleton);
	
	if(gui.display_surface){
		draw(model_drawable, environment);
	}
	if(gui.display_wireframe){
		draw_wireframe(model_drawable, environment);
	}
	if(gui.display_skeleton){
		draw(sk_drawable, environment);
	}

}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe); 
	ImGui::Checkbox("Surface", &gui.display_surface); 
	ImGui::Checkbox("Skeleton", &gui.display_skeleton); 
	ImGui::Indent();
	ImGui::Checkbox("Frames",&sk_drawable.display_joint_frame); ImGui::SameLine();
	ImGui::Checkbox("Spheres",&sk_drawable.display_joint_sphere);ImGui::SameLine();
	ImGui::Checkbox("Bones",&sk_drawable.display_segments);
	ImGui::Unindent();
	ImGui::Checkbox("Dual Quaternion", &gui.is_dual_quaternion);
	
	ImGui::Text("Model: "); ImGui::SameLine();
	bool type = ImGui::RadioButton("Cylinder",(int*)&model_type,cylinder); ImGui::SameLine();
	type |= ImGui::RadioButton("Bar",(int*)&model_type,bar); ImGui::SameLine();
	type |= ImGui::RadioButton("Spot",(int*)&model_type,spot);
	if(type) {
		initialize_model();
	}

	ImGui::Text("Motion: "); ImGui::SameLine();
	ImGui::Checkbox("Bending", &gui.is_bending_motion); ImGui::SameLine();
	ImGui::Checkbox("Twisting", &gui.is_twisting_motion);


	ImGui::SliderFloat("Time scale", &timer.scale, 0.0f, 2.0f);
	bool change_power = ImGui::SliderFloat("Power factor skinning weight", &gui.power_factor_skinning_weight, 0.5f, 10.0f, "%.3f",2);
	if(change_power) {
		set_skinning_weights();
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



animated_model_structure create_cylinder()
{
	animated_model_structure model;

	// Create the mesh
	int N = 50;
	model.rigged_mesh.mesh_bind_pose = mesh_primitive_cylinder(0.1f, {0,0,0}, {1,0,0}, N,N);
	model.rigged_mesh.mesh_deformed = model.rigged_mesh.mesh_bind_pose;

	// Create the Skeleton
	model.skeleton.parent_index = {-1, 0, 1};
	model.skeleton.joint_matrix_global.resize(3);
	model.skeleton.joint_matrix_global[0].set_identity();
	model.skeleton.joint_matrix_global[1].set_identity().set_block_translation(0.5,0,0);
	model.skeleton.joint_matrix_global[2].set_identity().set_block_translation(1.0,0,0);
	model.skeleton.update_joint_matrix_global_to_local();
	model.skeleton.joint_matrix_global_bind_pose = model.skeleton.joint_matrix_global;
	model.skeleton.joint_matrix_local_bind_pose = model.skeleton.joint_matrix_local;

	return model;
} 

animated_model_structure create_bar()
{
	animated_model_structure model;

	float const a = 0.1f;
	model.rigged_mesh.mesh_bind_pose = mesh_primitive_cubic_grid({0,-a,-a}, {1,-a,-a}, {1,a,-a}, {0,a,-a}, {0,-a,a}, {1,-a,a}, {1,a,a}, {0,a,a}, 35, 10, 10);

	model.rigged_mesh.mesh_deformed = model.rigged_mesh.mesh_bind_pose;

	// Create the Skeleton
	model.skeleton.parent_index = {-1, 0, 1};
	model.skeleton.joint_matrix_global.resize(3);
	model.skeleton.joint_matrix_global[0].set_identity();
	model.skeleton.joint_matrix_global[1].set_identity().set_block_translation(0.5,0,0);
	model.skeleton.joint_matrix_global[2].set_identity().set_block_translation(1.0,0,0);
	model.skeleton.update_joint_matrix_global_to_local();
	model.skeleton.joint_matrix_global_bind_pose = model.skeleton.joint_matrix_global;
	model.skeleton.joint_matrix_local_bind_pose = model.skeleton.joint_matrix_local;

	return model;
}

animated_model_structure create_spot() {
	animated_model_structure model;
	
	model.rigged_mesh.mesh_bind_pose = mesh_load_file_obj(project::path+"assets/spot/mesh.obj");
	rotation_transform R_init = rotation_transform::from_frame_transform({0,1,0},{1,0,0},{1,0,0},{0,0,1});
	model.rigged_mesh.mesh_bind_pose.apply_transform(R_init.matrix());
	model.rigged_mesh.mesh_deformed = model.rigged_mesh.mesh_bind_pose;


	read_from_file(project::path+"assets/spot/skeleton_connectivity_parent.txt", model.skeleton.parent_index);
	numarray<vec3> joint_position;
	numarray<quaternion> joint_orientation;
	read_from_file(project::path+"assets/spot/skeleton_rest_pose_translation.txt", joint_position);
	read_from_file(project::path+"assets/spot/skeleton_rest_pose_rotation.txt", joint_orientation);
	model.skeleton.joint_matrix_local.resize(joint_position.size());
	for(int k=0; k<joint_position.size(); ++k) {
		model.skeleton.joint_matrix_local[k].set_identity()
		  .set_block_translation(joint_position[k])
		  .set_block_linear(rotation_transform(joint_orientation[k]).matrix());
	}
	model.skeleton.joint_matrix_local[0] = R_init * model.skeleton.joint_matrix_local[0];
	model.skeleton.update_joint_matrix_local_to_global();
	model.skeleton.joint_matrix_global_bind_pose = model.skeleton.joint_matrix_global;
	model.skeleton.joint_matrix_local_bind_pose = model.skeleton.joint_matrix_local;

	return model;

}