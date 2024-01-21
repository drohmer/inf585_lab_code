#include "asset_loader.hpp"

using namespace cgp;

void filename_loader_structure::set_skeleton(std::string skeleton_root_path)
{
    loader_skeleton.skeleton_joint_name = skeleton_root_path+"skeleton_joint_name.txt";
    loader_skeleton.skeleton_parent_index = skeleton_root_path+"skeleton_parent_index.txt";
    loader_skeleton.skeleton_joint_matrix = skeleton_root_path+"skeleton_joint_matrix.txt";
}

void filename_loader_structure::add_animation(std::string anim_name, std::string animation_root_path)
{
    filename_loader_animation_structure& animation_loader = loader_animation[anim_name];

    animation_loader.animation_timing = animation_root_path+"skeleton_animation_timing.txt";
    animation_loader.animation_joint_index = animation_root_path+"skeleton_animation_joint_index.txt";
    animation_loader.animation_matrix = animation_root_path+"skeleton_animation_matrix.txt";
}



void filename_loader_structure::add_rigged_mesh(std::string mesh_name, std::string rigged_mesh_root_path, std::string texture)
{
    filename_loader_rigged_mesh_structure& rigged_loader = loader_rigged_mesh[mesh_name];

    rigged_loader.mesh = rigged_mesh_root_path+"mesh.obj";
    rigged_loader.controller_skinning_global_bind_matrix = rigged_mesh_root_path+"skinning_global_bind_matrix.txt";
	rigged_loader.controller_skinning_inverse_bind_matrix = rigged_mesh_root_path+"skinning_inverse_bind_matrix.txt";
	rigged_loader.controller_skinning_joint = rigged_mesh_root_path+"skinning_joint.txt";
	rigged_loader.controller_skinning_weight = rigged_mesh_root_path+"skinning_weight.txt";
	rigged_loader.controller_skinning_rig_to_skeleton_joint_index = rigged_mesh_root_path+"skinning_joint_rig_index_to_skeleton_index.txt";
	rigged_loader.texture = texture;
}

void filename_loader_structure::clear()
{
    loader_rigged_mesh.clear();
    loader_animation.clear();
    loader_skeleton = filename_loader_skeleton_structure();
}


animated_model_structure mesh_skinning_loader(filename_loader_structure param, affine_rts const& transform)
{
    animated_model_structure animated_model;
    affine_rt T; 
    T.set_rotation(transform.rotation).set_translation(transform.translation);
    

    for(auto& entry : param.loader_rigged_mesh) {

        std::string const& name = entry.first;
        auto const& param_mesh = entry.second;
        rigged_mesh_structure& rigged_mesh = animated_model.rigged_mesh[name];

        numarray<numarray<int> > vertex_correspondance;
        rigged_mesh.mesh_bind_pose = mesh_load_file_obj(param_mesh.mesh, vertex_correspondance);
        rigged_mesh.mesh_bind_pose.apply_transform(T);
        rigged_mesh.mesh_bind_pose.fill_empty_field();
        rigged_mesh.mesh_deformed = rigged_mesh.mesh_bind_pose;

        read_from_file(param_mesh.controller_skinning_inverse_bind_matrix, rigged_mesh.controller_skinning.inverse_bind_matrices);
        for(int k=0; k<rigged_mesh.controller_skinning.inverse_bind_matrices.size(); ++k) {
           rigged_mesh.controller_skinning.inverse_bind_matrices[k] = rigged_mesh.controller_skinning.inverse_bind_matrices[k]*inverse(T);
           rigged_mesh.controller_skinning.inverse_bind_matrices[k].apply_scaling(transform.scaling);
        }
        cgp::numarray<cgp::numarray<int> > joint_index;
        cgp::numarray<cgp::numarray<float> > joint_weight;
        read_from_file(param_mesh.controller_skinning_joint, joint_index);
        read_from_file(param_mesh.controller_skinning_weight, joint_weight);
        joint_weight = map_correspondance(joint_weight, vertex_correspondance);
        joint_index = map_correspondance(joint_index, vertex_correspondance);

        cgp::numarray< cgp::numarray<skinning_weight_info> > vertex_to_joint_dependence;
        vertex_to_joint_dependence.resize(joint_index.size());
        for(int kv=0; kv<joint_index.size(); ++kv) {
            vertex_to_joint_dependence[kv].resize(joint_index[kv].size());
            for(int kd=0; kd<joint_index[kv].size(); ++kd) {
                vertex_to_joint_dependence[kv][kd].joint_index = joint_index[kv][kd];
                vertex_to_joint_dependence[kv][kd].weight = joint_weight[kv][kd];
            }
        }
        rigged_mesh.controller_skinning.vertex_to_joint_dependence = vertex_to_joint_dependence;
        read_from_file(param_mesh.controller_skinning_rig_to_skeleton_joint_index, rigged_mesh.controller_skinning.rig_index_to_skeleton_index);
        read_from_file(param_mesh.controller_skinning_global_bind_matrix, rigged_mesh.controller_skinning.global_bind_matrix);
    }



    read_from_file(param.loader_skeleton.skeleton_joint_name, animated_model.skeleton.joint_name);
    read_from_file(param.loader_skeleton.skeleton_parent_index, animated_model.skeleton.parent_index);
    read_from_file(param.loader_skeleton.skeleton_joint_matrix, animated_model.skeleton.joint_matrix_local);
    animated_model.skeleton.joint_matrix_local[0] = T * animated_model.skeleton.joint_matrix_local[0];
    for(int k=0; k<animated_model.skeleton.joint_matrix_local.size(); ++k) {
        animated_model.skeleton.joint_matrix_local[k].apply_scaling_to_block_translation(transform.scaling);
    }
    animated_model.skeleton.update_joint_matrix_local_to_global();
    

    for(auto const& entry_anim : param.loader_animation){
        std::string const& name = entry_anim.first;
        auto const& param_anim = entry_anim.second;

        read_from_file(param_anim.animation_timing, animated_model.animation[name].times);
        read_from_file(param_anim.animation_joint_index, animated_model.animation[name].joint_index);
        read_from_file(param_anim.animation_matrix, animated_model.animation[name].matrix);

        for(int k1=0; k1<animated_model.animation[name].matrix.size(); ++k1) {
            for(int k2=0; k2<animated_model.animation[name].matrix[0].size(); ++k2) {
                animated_model.animation[name].matrix[k1][k2].apply_scaling_to_block_translation(transform.scaling);
            }
        }

        for(int k2=0; k2<animated_model.animation[name].matrix[0].size(); ++k2) {
            animated_model.animation[name].matrix[0][k2] = T * animated_model.animation[name].matrix[0][k2];
        }


        animated_model.animation[name].update_time_max();

    }


    return animated_model;
}
