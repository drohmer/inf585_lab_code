#pragma once

#include "cgp/cgp.hpp"

#include "../animated_model/animated_model.hpp"

struct filename_loader_rigged_mesh_structure {
    std::string mesh;
    std::string texture;
    std::string controller_skinning_inverse_bind_matrix;
    std::string controller_skinning_joint;
    std::string controller_skinning_weight;
    std::string controller_skinning_rig_to_skeleton_joint_index;
    std::string controller_skinning_global_bind_matrix;
};
struct filename_loader_animation_structure {
    std::string animation_name;
    std::string animation_timing;
    std::string animation_joint_index;
    std::string animation_matrix;
};
struct filename_loader_skeleton_structure {
    std::string skeleton_joint_name;
    std::string skeleton_parent_index;
    std::string skeleton_joint_matrix;
};

struct filename_loader_structure {
    filename_loader_skeleton_structure loader_skeleton;
    std::map<std::string, filename_loader_rigged_mesh_structure> loader_rigged_mesh;
    std::map<std::string, filename_loader_animation_structure> loader_animation;

    void set_skeleton(std::string skeleton_root_path);
    void add_animation(std::string anim_name, std::string animation_root_path);
    void add_rigged_mesh(std::string mesh_name, std::string rigged_mesh_root_path, std::string texture);
    void clear();
};

animated_model_structure mesh_skinning_loader(filename_loader_structure filename_parameter, cgp::affine_rts const& transform=cgp::affine_rts ());


//Map correspondance between skinning weights and vertices (that have been duplicated to load the texture coordinates)
template <typename T>
cgp::numarray<T> map_correspondance(cgp::numarray<T> value, cgp::numarray<cgp::numarray<int> > const& correspondance)
{
    // find the maximal index used for destination
    int max_index = 0;
	for (int k1 = 0; k1 < correspondance.size(); ++k1) {
		for (int k2 = 0; k2 < correspondance[k1].size(); ++k2) {
			if (max_index < correspondance[k1][k2]) {
				max_index = correspondance[k1][k2];
			}
		}
	}

	cgp::numarray<T> new_value;
    new_value.resize(max_index+1);

    // Apply correspondance (copy value[k] in its destination)
	for (int k = 0; k < correspondance.size(); ++k) {
		for (int k2 = 0; k2 < correspondance[k].size(); ++k2) {
			new_value[correspondance[k][k2]] = value[k];
		}
	}
    return new_value;
}

