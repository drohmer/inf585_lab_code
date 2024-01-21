#pragma once

#include "cgp/cgp.hpp"

#include "../skeleton_structure/skeleton_structure.hpp"

struct skeleton_drawable
{
    skeleton_drawable();
    skeleton_drawable(skeleton_structure const& skeleton);
    void clear();
    void update(skeleton_structure const& skeleton);

    float size_frame = 0.05f;
    float size_sphere = 0.01f;
    bool display_segments = true;
    bool display_joint_frame = true;
    bool display_joint_sphere = true;

    cgp::curve_drawable segments;
    cgp::mesh_drawable joint_frame;
    cgp::mesh_drawable joint_sphere;
    cgp::numarray<cgp::mat4> data;
};

template <typename SCENE>
void draw(skeleton_drawable const& skeleton, SCENE const& scene);

template <typename SCENE>
void draw(skeleton_drawable const& skeleton, SCENE const& scene)
{
	if(skeleton.display_segments)
		draw(skeleton.segments, scene);
	
	size_t const N = skeleton.data.size();

	if(skeleton.display_joint_frame)
	{
		cgp::mesh_drawable joint_frame_temp = skeleton.joint_frame;
		joint_frame_temp.model.scaling = skeleton.size_frame;
		for (size_t k = 0; k < N; ++k)
		{
			
			joint_frame_temp.model = cgp::affine::from_matrix(skeleton.data[k]).set_scaling(skeleton.size_frame);
			draw(joint_frame_temp, scene);
		}
	}
	
	if (skeleton.display_joint_sphere)
	{
		cgp::mesh_drawable joint_sphere_temp = skeleton.joint_sphere;
		joint_sphere_temp.model.scaling = skeleton.size_sphere;
		for (size_t k = 0; k < N; ++k)
		{
			joint_sphere_temp.model.translation = skeleton.data[k].get_block_translation();
			draw(joint_sphere_temp, scene);
		}
	}

	
}