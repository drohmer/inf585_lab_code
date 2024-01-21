#pragma once

#include "cgp/cgp.hpp"


struct skeleton_animation_structure {

    // Correspondance between the joint index of the animation and the joint index of the skeleton hierarchy
    //  index_in_skeleton_hierarchy = joint_index[k]
    cgp::numarray<int> joint_index;
    // Store the key times for each joint, and for each time index
    cgp::numarray< cgp::numarray<float> > times; // times[k_joint][k_time]
    // Store the joint matrix for each joint, and for each time index
    cgp::numarray< cgp::numarray<cgp::mat4> > matrix; // matrix[k_joint][k_time]

    // Evaluate the interpolated matrix for a given join and time \in [0, time_max]
    cgp::mat4 evaluate(int k_joint, float t) const;
    

    // Store the maximal time value of the animation (assume the starting time is 0)
    float time_max;
    // Update the time_max value such that time_max=max(times)
    void update_time_max();
};