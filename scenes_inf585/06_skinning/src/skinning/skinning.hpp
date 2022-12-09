#pragma once

#include "cgp/cgp.hpp"


namespace cgp
{
	struct rig_structure
	{
		numarray<numarray<int>> joint;
		numarray<numarray<float>> weight;
	};

	void normalize_weights(numarray<numarray<float>>& weights);

	void skinning_LBS_compute(
		numarray<vec3>& position_skinned,
		numarray<vec3>& normal_skinned,
		numarray<affine_rt> const& skeleton_current,
		numarray<affine_rt> const& skeleton_rest_pose,
		numarray<vec3> const& position_rest_pose,
		numarray<vec3> const& normal_rest_pose,
		rig_structure const& rig);


}