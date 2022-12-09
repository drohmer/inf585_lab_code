#include "scene.hpp"

#include "loader/skinning_loader.hpp"

using namespace cgp;




void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_y();
	camera_control.look_at({ 3.0f, 2.0f, 2.0f }, {0,0,0}, {0,0,1});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());


	mesh shape;
	load_cylinder(skeleton_data, rig, shape);
	load_animation_bend_zx(skeleton_data.animation_geometry_local,
		skeleton_data.animation_time,
		skeleton_data.parent_index);
	update_new_content(shape, mesh_drawable::default_texture);
}

void scene_structure::compute_deformation()
{
	float const t = timer.t;

	skinning_data.skeleton_current = skeleton_data.evaluate_global(t);
	visual_data.skeleton_current.update(skinning_data.skeleton_current, skeleton_data.parent_index);

	// Compute skinning deformation
	skinning_LBS_compute(skinning_data.position_skinned, skinning_data.normal_skinned,
		skinning_data.skeleton_current, skinning_data.skeleton_rest_pose,
		skinning_data.position_rest_pose, skinning_data.normal_rest_pose,
		rig);
	visual_data.surface_skinned.vbo_position.update(skinning_data.position_skinned);
	visual_data.surface_skinned.vbo_normal.update(skinning_data.normal_skinned);

}

void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	timer.update();

	compute_deformation();

	if (gui.surface_skinned)
		draw(visual_data.surface_skinned, environment);
	if (gui.wireframe_skinned)
		draw_wireframe(visual_data.surface_skinned, environment, { 0.5f, 0.5f, 0.5f });

	draw(visual_data.skeleton_current, environment);

	if (gui.surface_rest_pose)
		draw(visual_data.surface_rest_pose, environment);
	if (gui.wireframe_rest_pose)
		draw_wireframe(visual_data.surface_rest_pose, environment, { 0.5f, 0.5f, 0.5f });

	draw(visual_data.skeleton_rest_pose, environment);

}


void scene_structure::update_new_content(mesh const& shape, opengl_texture_image_structure texture_id)
{
	visual_data.surface_skinned.clear();
	visual_data.surface_skinned.initialize_data_on_gpu(shape);
	visual_data.surface_skinned.texture = texture_id;

	visual_data.surface_rest_pose.clear();
	visual_data.surface_rest_pose.initialize_data_on_gpu(shape);
	visual_data.surface_rest_pose.texture = texture_id;

	skinning_data.position_rest_pose = shape.position;
	skinning_data.position_skinned = skinning_data.position_rest_pose;
	skinning_data.normal_rest_pose = shape.normal;
	skinning_data.normal_skinned = skinning_data.normal_rest_pose;

	skinning_data.skeleton_current = skeleton_data.rest_pose_global();
	skinning_data.skeleton_rest_pose = skinning_data.skeleton_current;

	visual_data.skeleton_current.clear();
	visual_data.skeleton_current = skeleton_drawable(skinning_data.skeleton_current, skeleton_data.parent_index);

	visual_data.skeleton_rest_pose.clear();
	visual_data.skeleton_rest_pose = skeleton_drawable(skinning_data.skeleton_rest_pose, skeleton_data.parent_index);

	timer.t_min = skeleton_data.animation_time[0];
	timer.t_max = skeleton_data.animation_time[skeleton_data.animation_time.size() - 1];
	timer.t = skeleton_data.animation_time[0];
}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Display frame", &gui.display_frame);
	ImGui::Spacing(); ImGui::Spacing();

	ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max, "%.2f s");
	ImGui::SliderFloat("Time Scale", &timer.scale, 0.05f, 2.0f, "%.2f s");

	ImGui::Spacing(); ImGui::Spacing();

	ImGui::Text("Deformed ");
	ImGui::Text("Surface: "); ImGui::SameLine();
	ImGui::Checkbox("Plain", &gui.surface_skinned); ImGui::SameLine();
	ImGui::Checkbox("Wireframe", &gui.wireframe_skinned);

	ImGui::Text("Skeleton: "); ImGui::SameLine();
	ImGui::Checkbox("Bones", &gui.skeleton_current_bone); ImGui::SameLine();
	ImGui::Checkbox("Frame", &gui.skeleton_current_frame); ImGui::SameLine();
	ImGui::Checkbox("Sphere", &gui.skeleton_current_sphere);

	ImGui::Spacing(); ImGui::Spacing();

	ImGui::Text("Rest Pose");
	ImGui::Text("Surface: "); ImGui::SameLine();
	ImGui::Checkbox("Plain##Rest", &gui.surface_rest_pose); ImGui::SameLine();
	ImGui::Checkbox("Wireframe##Rest", &gui.wireframe_rest_pose);

	ImGui::Text("Skeleton: "); ImGui::SameLine();
	ImGui::Checkbox("Bones##Rest", &gui.skeleton_rest_pose_bone); ImGui::SameLine();
	ImGui::Checkbox("Frame##Rest", &gui.skeleton_rest_pose_frame); ImGui::SameLine();
	ImGui::Checkbox("Sphere##Rest", &gui.skeleton_rest_pose_sphere);

	ImGui::Spacing(); ImGui::Spacing();


	visual_data.skeleton_current.display_segments = gui.skeleton_current_bone;
	visual_data.skeleton_current.display_joint_frame = gui.skeleton_current_frame;
	visual_data.skeleton_current.display_joint_sphere = gui.skeleton_current_sphere;
	visual_data.skeleton_rest_pose.display_segments = gui.skeleton_rest_pose_bone;
	visual_data.skeleton_rest_pose.display_joint_frame = gui.skeleton_rest_pose_frame;
	visual_data.skeleton_rest_pose.display_joint_sphere = gui.skeleton_rest_pose_sphere;

	mesh new_shape;
	bool update = false;
	ImGui::Text("Cylinder"); ImGui::SameLine();
	bool const cylinder_bend_z = ImGui::Button("Bend z###CylinderBendZ");  ImGui::SameLine();
	if (cylinder_bend_z) {
		update = true;
		load_cylinder(skeleton_data, rig, new_shape);
		load_animation_bend_z(skeleton_data.animation_geometry_local,
			skeleton_data.animation_time,
			skeleton_data.parent_index);
	}
	bool const cylinder_bend_zx = ImGui::Button("Bend zx###CylinderBendZX");
	if (cylinder_bend_zx)std::cout << cylinder_bend_zx << std::endl;
	if (cylinder_bend_zx) {
		update = true;
		load_cylinder(skeleton_data, rig, new_shape);
		load_animation_bend_zx(skeleton_data.animation_geometry_local,
			skeleton_data.animation_time,
			skeleton_data.parent_index);
	}

	ImGui::Text("Rectangle"); ImGui::SameLine();
	bool const rectangle_bend_z = ImGui::Button("Bend z###RectangleBendZ");  ImGui::SameLine();
	if (rectangle_bend_z) {
		update = true;
		load_rectangle(skeleton_data, rig, new_shape);
		load_animation_bend_z(skeleton_data.animation_geometry_local,
			skeleton_data.animation_time,
			skeleton_data.parent_index);
	}
	bool const rectangle_bend_zx = ImGui::Button("Bend zx###RectangleBendZX");
	if (rectangle_bend_zx) {
		update = true;
		load_rectangle(skeleton_data, rig, new_shape);
		load_animation_bend_zx(skeleton_data.animation_geometry_local,
			skeleton_data.animation_time,
			skeleton_data.parent_index);
	}
	bool const rectangle_twist_x = ImGui::Button("Twist x###RectangleTwistX");
	if (rectangle_twist_x) {
		update = true;
		load_rectangle(skeleton_data, rig, new_shape);
		load_animation_twist_x(skeleton_data.animation_geometry_local,
			skeleton_data.animation_time,
			skeleton_data.parent_index);
	}

	ImGui::Text("Marine"); ImGui::SameLine();
	bool const marine_run = ImGui::Button("Run"); ImGui::SameLine();
	bool const marine_walk = ImGui::Button("Walk"); ImGui::SameLine();
	bool const marine_idle = ImGui::Button("Idle");

	opengl_texture_image_structure texture_id = mesh_drawable::default_texture;
	if (marine_run || marine_walk || marine_idle) load_skinning_data(project::path+"assets/marine/", skeleton_data, rig, new_shape, texture_id);
	if (marine_run) load_skinning_anim(project::path+"assets/marine/anim_run/", skeleton_data);
	if (marine_walk) load_skinning_anim(project::path + "assets/marine/anim_walk/", skeleton_data);
	if (marine_idle) load_skinning_anim(project::path + "assets/marine/anim_idle/", skeleton_data);
	if (marine_run || marine_walk || marine_idle) {
		update = true;
		normalize_weights(rig.weight);
		float const scaling = 0.005f;
		for (auto& p : new_shape.position) p *= scaling;
		skeleton_data.scale(scaling);
	}

	if (update)
		update_new_content(new_shape, texture_id);


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

