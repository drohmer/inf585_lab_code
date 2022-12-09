#pragma once

#include "cgp/cgp.hpp"
#include "../skinning/skinning.hpp"
#include "../skeleton/skeleton.hpp"


void load_animation_bend_zx(cgp::numarray<cgp::numarray<cgp::affine_rt>>& animation_skeleton, cgp::numarray<float>& animation_time, cgp::numarray<int> const& parent_index);
void load_animation_bend_z(cgp::numarray<cgp::numarray<cgp::affine_rt>>& animation_skeleton, cgp::numarray<float>& animation_time, cgp::numarray<int> const& parent_index);
void load_animation_twist_x(cgp::numarray<cgp::numarray<cgp::affine_rt>>& animation_skeleton, cgp::numarray<float>& animation_time, cgp::numarray<int> const& parent_index);

void load_cylinder(cgp::skeleton_animation_structure& skeleton_data, cgp::rig_structure& rig, cgp::mesh& shape);
void load_rectangle(cgp::skeleton_animation_structure& skeleton_data, cgp::rig_structure& rig, cgp::mesh& shape);


void load_skinning_data(std::string const& directory, cgp::skeleton_animation_structure& skeleton_data, cgp::rig_structure& rig, cgp::mesh& shape, cgp::opengl_texture_image_structure& texture_id);
void load_skinning_anim(std::string const& directory, cgp::skeleton_animation_structure& skeleton_data);