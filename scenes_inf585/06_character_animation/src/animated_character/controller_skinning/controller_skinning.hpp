#pragma once

#include "cgp/cgp.hpp"


// A skinning weight entry: the local joint index, and the associated weight
struct skinning_weight_info {
    int joint_index;
    float weight;
};
std::string type_str(skinning_weight_info const& ); // for debug
std::ostream& operator<<(std::ostream& s, skinning_weight_info const& weight_info); // for debug


// Contain the information of the skinning weights, dependance, and inverse bind matrices
struct controller_skinning_structure {
    cgp::numarray< cgp::numarray<skinning_weight_info> > vertex_to_joint_dependence; // vertex_to_joint_dependence[k_vertex][k_dependance] 
    cgp::numarray<cgp::mat4> inverse_bind_matrices; // Inverse bind matrices corresponding to a given mesh
    cgp::numarray<int> rig_index_to_skeleton_index; // correspondance between the index of the local joint (for a given mesh), and the index in the global skeleton structure
    cgp::mat4 global_bind_matrix;                   // A global bind matrix (usually identity)
};
