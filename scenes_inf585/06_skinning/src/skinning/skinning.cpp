#include "skinning.hpp"

namespace cgp
{
	void normalize_weights(numarray<numarray<float>>& weights)
	{
		size_t const N = weights.size();
		for (size_t k = 0; k < N; ++k) {
			float s = 0.0f;
			for(float w : weights[k]) s += w;
			assert_cgp_no_msg(s>1e-5f);
			for(float& w : weights[k]) w /= s;
		}
	}


	// Linear Blend Skinning
	void skinning_LBS_compute(
		numarray<vec3>& position_skinned,  // position to deform
		numarray<vec3>& normal_skinned,    // normal to deform
		numarray<affine_rt> const& skeleton_current,    // rigid transforms for skeleton joints in current pose
		numarray<affine_rt> const& skeleton_rest_pose,  // rigid transforms of skeleton joints in rest pose
		numarray<vec3> const& position_rest_pose,       // vertex positions of the mesh in rest pose
		numarray<vec3> const& normal_rest_pose,         // normal coordinates of the mesh in rest pose
		rig_structure const& rig)                     // information of the skinning weights (joints and weights associated to a vertex)
	{
		size_t const N_vertex = position_rest_pose.size();
		size_t const N_joint = skeleton_current.size();

		// Sanity check on sizes of buffers
		assert_cgp_no_msg(position_skinned.size()==N_vertex);
		assert_cgp_no_msg(normal_skinned.size()==N_vertex);
		assert_cgp_no_msg(normal_rest_pose.size()==N_vertex);
		assert_cgp_no_msg(skeleton_rest_pose.size()==N_joint);
		assert_cgp_no_msg(rig.joint.size()==N_vertex);
		assert_cgp_no_msg(rig.weight.size()==N_vertex);

		// To do
		//   Compute the Linear Blend Skinning ...



	}

}