#pragma once

#include "cgp/cgp.hpp"


namespace cgp
{
	// Store the rig/skinning weights.
	//   This structure stores only the non-zeros entry of the skinning weights.
	struct rig_structure
	{
		numarray<numarray<int>> joint; // Index of join associated to a given vertex
		numarray<numarray<float>> weight; // Skinning weight corresponding to the indexed joint

		//  For each vertex k:
		//    joint[k] is an array storing all join indices with non-zero skinning weights
		//    weight[k] is an array storing all non-zero weights between vertex k and the joints indiced in joint[k]
		// Note: the size of joint[k] and weight[k] depends on the number of non-zero skinning weights for the current vertex k.
	};

	// Renormalize skinning weights to sum-up to 1 for all vertices.
	void normalize_weights(numarray<numarray<float>>& weights);

	// Compute the Linear Blend Skinning deformation on the mesh (and its normals) given the skeleton data and rig.
	void skinning_LBS_compute(
		numarray<vec3>& position_skinned,
		numarray<vec3>& normal_skinned,
		numarray<affine_rt> const& skeleton_current,
		numarray<affine_rt> const& skeleton_rest_pose,
		numarray<vec3> const& position_rest_pose,
		numarray<vec3> const& normal_rest_pose,
		rig_structure const& rig);


}