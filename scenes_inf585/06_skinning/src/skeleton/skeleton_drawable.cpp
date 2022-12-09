#include "skeleton_drawable.hpp"

namespace cgp
{
	skeleton_drawable::skeleton_drawable()
		:segments(), joint_frame(), joint_sphere(), data()
	{}

	skeleton_drawable::skeleton_drawable(numarray<affine_rt> const& skeleton, numarray<int> const& parent_index)
		: segments(), joint_frame(), joint_sphere(), data(skeleton)
	{
		size_t const N = parent_index.size();
		numarray<vec3> edges;
		for (size_t k = 1; k < N; ++k){
			size_t const parent = parent_index[k];
			assert_cgp_no_msg(parent>=0 && parent<N);
			edges.push_back(skeleton[k].translation);
			edges.push_back(skeleton[parent].translation);
		}
		
		segments.display_type = curve_drawable_display_type::Segments;
		segments.initialize_data_on_gpu(edges);
		joint_frame.initialize_data_on_gpu(mesh_primitive_frame());
		joint_sphere.initialize_data_on_gpu(mesh_primitive_sphere());
	}

	void skeleton_drawable::clear()
	{
		segments.clear();
		joint_frame.clear();
		joint_sphere.clear();
		data.clear();
	}

	void skeleton_drawable::update(numarray<affine_rt> const& skeleton, numarray<int> const& parent_index)
	{
		data = skeleton;

		size_t const N = skeleton.size();
		numarray<vec3> edges;
		for (size_t k = 1; k < N; ++k){
			size_t const parent = parent_index[k];
			assert_cgp_no_msg(parent>=0 && parent<N);
			edges.push_back(skeleton[k].translation);
			edges.push_back(skeleton[parent].translation);
		}

		segments.vbo_position.update(edges);
	}
} 