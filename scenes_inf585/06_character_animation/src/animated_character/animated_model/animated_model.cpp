#include "animated_model.hpp"

using namespace cgp;

void animated_model_structure::skinning_lbs(std::string const& mesh_name)
{
    cgp::mesh const& mesh_bind_pose = rigged_mesh[mesh_name].mesh_bind_pose;
    cgp::mesh& mesh_deformed = rigged_mesh[mesh_name].mesh_deformed;
    controller_skinning_structure const& controller_skinning = rigged_mesh[mesh_name].controller_skinning;


    // Prepare the transformation matrix for all the joints that impact the current mesh
    int N_impacting_joints = controller_skinning.inverse_bind_matrices.size(); // only a subset of the skeleton joints may impact the current mesh
    cgp::numarray<cgp::mat4> transformation_matrix;
    transformation_matrix.resize(N_impacting_joints);
    for(int k=0; k<N_impacting_joints; ++k) {
        mat4 const& inv_bind_pose = controller_skinning.inverse_bind_matrices.at(k); // the inverse of the bind pose (precomputed)
        int joint_index_in_skeleton = controller_skinning.rig_index_to_skeleton_index.at(k); // need to find the corresponding index of this joint into the global skeleton
        mat4 const& joint_current = skeleton.joint_matrix_global.at(joint_index_in_skeleton); // retrieve the current joint pose in the global position

        transformation_matrix.at(k) = joint_current * inv_bind_pose;
    }

    // Compute skinning deformation
    int N_vertex = mesh_bind_pose.position.size();
    for(int k_vertex=0; k_vertex<N_vertex; ++k_vertex) {
        int N_dependence = controller_skinning.vertex_to_joint_dependence.at(k_vertex).size();
        auto const& vertex_to_joint_dependence = controller_skinning.vertex_to_joint_dependence.at(k_vertex);
        vec3 const& p0 = mesh_bind_pose.position.at(k_vertex);
        vec3 const& n0 = mesh_bind_pose.normal.at(k_vertex);

        vec3 p_accumulated;
        vec3 n_accumulated;
        for(int k=0; k<N_dependence; ++k) {
            auto const& dependence = vertex_to_joint_dependence.at(k);
            int joint_index = dependence.joint_index;
            float weight = dependence.weight;

            mat4 const& T = transformation_matrix.at(joint_index);

            vec3 p_tmp = T.transform_position(p0);
            vec3 n_tmp = T.transform_vector(n0);
            p_tmp *= weight;
            n_tmp *= weight;
            p_accumulated += p_tmp;
            n_accumulated += n_tmp;
        }
        mesh_deformed.position[k_vertex] = p_accumulated;
        mesh_deformed.normal[k_vertex] = n_accumulated;
    }
}

void animated_model_structure::set_skeleton_from_animation(std::string const& animation_name, float t)
{
    auto const& current_animation = animation[animation_name];
    int N_joint = current_animation.joint_index.size();
    for(int k=0; k<N_joint; ++k) {
        int joint = current_animation.joint_index[k];
        mat4 M = current_animation.evaluate(k, t);
        skeleton.joint_matrix_local[joint] = M;
    }
    skeleton.update_joint_matrix_local_to_global();
}

void animated_model_structure::apply_transformation(vec3 const& translation, rotation_transform rotation, float scaling)
{
    if(skeleton.joint_matrix_local.size()>0) {
        skeleton.joint_matrix_local[0] = rotation * skeleton.joint_matrix_local[0];
        skeleton.joint_matrix_local[0].apply_scaling_to_block_linear(scaling);
        skeleton.joint_matrix_local[0].apply_translation(translation);
        skeleton.update_joint_matrix_local_to_global();
    }

    for(auto& anim_entry : animation) {
        auto& animation_matrices = anim_entry.second.matrix;
        for(int kt=0; kt<animation_matrices[0].size(); ++kt) {
            mat4& M = animation_matrices[0][kt];
            M = rotation*M;
            M.apply_scaling_to_block_linear(scaling);
            M.apply_translation(translation);
        }
    }

}