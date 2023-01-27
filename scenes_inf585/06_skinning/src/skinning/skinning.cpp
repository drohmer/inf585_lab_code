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
		numarray<affine_rt> const& skeleton_current,    // rigid transforms for skeleton joints in current pose (expressed in the global frame)
		numarray<affine_rt> const& skeleton_rest_pose,  // rigid transforms of skeleton joints in rest pose (expressed in the global frame)
		numarray<vec3> const& position_rest_pose,       // vertex positions of the mesh in rest pose
		numarray<vec3> const& normal_rest_pose,         // normal coordinates of the mesh in rest pose
		rig_structure const& rig)                     // information of the skinning weights (joints and weights associated to a vertex): check the header file to understand the rig structure
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
		//   ...
		// 
		//
		// Additional notes on transformation and matrices:
		//
		//  1) An affine_rt structure ensure that its content correspond to a rigid transform ({affine_rt}.rotation, {affine_rt}.translation)
		//  - The inverse of an affine_rt structure can be efficiently computed using the inverse function
		//      {affine_rt_inverse} = inverse({affine_rt});
		// 
		//  2) Affine_rt structures can be multiplied together: {affine_rt} * {affine_rt}
		// 
		//  3) Affine_rt can be multiplied with a scalar thus creating a affine_rts (rotation, translation, scaling) structure: {affine_rts} = scalar * {affine_rt}
		//
		//  4) Affine_rt structures cannot be summed-up directly with another affine_rt as the result would generally not be a rigid transform.
		// 
		//  5) Affine_rt (and affine_rts) can be applied to 3D position p:
		//     vec3 q = {affine_rt} * p
		//
		//  6) Affine_rt structures can be converted to their corresponding matrix using
		//       {mat4} = {affine_rt}.matrix();
		//       mat4 matrices can be summed-up.
		//
		//  7) Applying a mat4 matrix to a 3/4D vector can be applied in the following ways:
		//       {vec4} = {mat4} * vec4{x,y,z,1} // applying the mat4 to a 3D point (result is a vec4)
		//       {vec4} = {mat4} * vec4{x,y,z,0} // applying the mat4 to a 3D vector (result is a vec4)
		//       A vec3 can be extracted from a vec4 using the following relation:
		//       {vec3} = {vec4}.xyz();
		//
		//   8) A direct operation between mat4 and 3D point is also available:
		//       {vec3} = {mat4} .apply_to_vec3_position( vec3{x,y,z} )
		//       => Assume the vector is a point vec4{x,y,z,1.0}, and perform automatically the renormalization.
		//       Similar to the following two lines:
		//         vec4 temp = {mat4} * vec4{x,y,z,1};
		//         {vec3} = temp.xyz()/ temp.w();
		//   - Rem. In the special case where the mat4 matrix represents an affine transform, the normalization is not necessary.
		//    Therefore the two following lines are similar (only when the mat4 is an affine transform)
		//         {vec3} = ({mat4} * vec4{x,y,z,1}).xyz();
		//         {vec3} = {mat4} * vec3{x,y,z};
		// 
		//    And applying the mat4 to a 3D vector (instead of point) can be written in one line
		//         {vec3} = ({mat4} * vec3{x,y,z,0}).xyz();
		//    Or in using the direct operation
		//         {vec3} = {mat4}.apply_to_vec3_normal({vec3});
		
		



	}

}