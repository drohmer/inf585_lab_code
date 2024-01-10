#include "skeleton_drawable.hpp"

using namespace cgp;

skeleton_drawable::skeleton_drawable()
		:segments(), joint_frame(), joint_sphere(), data()
{}

skeleton_drawable::skeleton_drawable(skeleton_structure const& skeleton)
    : segments(), joint_frame(), joint_sphere(), data(skeleton.joint_matrix_global)
{
    size_t const N = skeleton.parent_index.size();
    cgp::numarray<cgp::vec3> edges;
    for (size_t k = 1; k < N; ++k){
        int parent = skeleton.parent_index[k];
        assert_cgp_no_msg(parent>=0 && parent<N);
        edges.push_back(skeleton.joint_matrix_global[k].get_block_translation());
        edges.push_back(skeleton.joint_matrix_global[parent].get_block_translation());
    }
    
    segments.display_type = curve_drawable_display_type::Segments;
    segments.initialize_data_on_gpu(edges);
    joint_frame.initialize_data_on_gpu(mesh_primitive_frame());
    joint_sphere.initialize_data_on_gpu(mesh_primitive_sphere());
}

void skeleton_drawable::update(skeleton_structure const& skeleton){
    data = skeleton.joint_matrix_global;

    size_t const N = skeleton.size();
    cgp::numarray<cgp::vec3> edges;
    for (size_t k = 1; k < N; ++k){
        int parent = skeleton.parent_index[k];
        assert_cgp_no_msg(parent>=0 && parent<N);
        edges.push_back(skeleton.joint_matrix_global[k].get_block_translation());
        edges.push_back(skeleton.joint_matrix_global[parent].get_block_translation());
    }

    segments.vbo_position.update(edges);
}

void skeleton_drawable::clear()
{
    segments.clear();
    joint_frame.clear();
    joint_sphere.clear();
    data.clear();
}