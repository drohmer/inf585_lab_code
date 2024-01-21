#pragma once

#include "cgp/cgp.hpp"

#include "../controller_skinning/controller_skinning.hpp"
#include "../skeleton_structure/skeleton_structure.hpp"
#include "../skeleton_animation/skeleton_animation.hpp"

struct rigged_mesh_structure {
    cgp::mesh mesh_bind_pose;  // Bind pose (/un-deformed) mesh
    cgp::mesh mesh_deformed;   // Deformed mesh
    controller_skinning_structure controller_skinning;   // skinning weights dependence
};

struct animated_model_structure {

    skeleton_structure skeleton; // A skeleton that stores its structure/hierarchy, and the local and global joint frames
    std::map<std::string, rigged_mesh_structure> rigged_mesh; // an animated model may be linked to multiple rigged meshes
    std::map<std::string, skeleton_animation_structure> animation; // storage for all the possible animation of this skeleton

    // Compute the joint position corresponding to the given animation_name at time t from the animation structure. Then update the skeleton structure in filling the local joint matrix. Finally update the global matrices of the skeleton.
    //  - t can be an arbitrary float values, the animation matrix are interpolated between the frames
    //  - This function doesn't call the Skinning deformation on the rigged_mesh
    void set_skeleton_from_animation(std::string const& animation_name, float t);
    

    // Compute the Linear Blend Skinning deformation on the designated rigged mesh
    void skinning_lbs(std::string const& mesh_name);

    // Apply a tranlation, rotation, and scaling to all the skeleton structure (current skeleton and all animation)
    void apply_transformation(cgp::vec3 const& translation, cgp::rotation_transform rotation= cgp::rotation_transform(), float scaling=1.0f);
};