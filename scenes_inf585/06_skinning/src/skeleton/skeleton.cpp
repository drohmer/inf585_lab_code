#include "skeleton.hpp"

namespace cgp
{
	static bool find_interval(int& index_0, float& alpha, numarray<float> times, float t)
	{
		assert_cgp(times.size()>=2, "time intervals should have more than 2 values");

		size_t const N = times.size();
		if (t < times[0] || t >= times[N - 1])
		{
			index_0 = -1;
			alpha = 0.0f;
			return false;
		}

		int current_index = 0;
		while (t > times[current_index + 1]) {
			assert_cgp_no_msg(N>current_index);
			++current_index;
		}
		index_0 = current_index;

		float const t0 = times[current_index];
		float const t1 = times[current_index+1];
		float const dt = t1-t0;
		assert_cgp(std::abs(dt)>0, "Time interval should be > 0");

		alpha = (t-t0)/dt;
		return true;
	}

	numarray<affine_rt> skeleton_animation_structure::evaluate_local(float t) const
	{
		int kt=0;
		float alpha;
		bool const find_time = find_interval(kt, alpha, animation_time, t);
		assert_cgp(find_time, "Could not find correct time interval for time t="+str(t)+", while allowed time interval is ["+str(animation_time[0])+","+str(animation_time[animation_time.size()-1])+"]");

		size_t const N_joint = animation_geometry_local[0].size();
		numarray<affine_rt> skeleton_current;
		skeleton_current.resize(N_joint);

		for(size_t kj=0; kj<N_joint; ++kj)
		{
			
			affine_rt const T0 = animation_geometry_local[kt][kj];
			affine_rt const T1 = animation_geometry_local[kt+1][kj];

			// To do:
			//   Set a correct interpolated rigid transform T for the skeleton
			//   T = interpolation(T0, T1, alpha)
			//   Note: T0, T1, and alpha are provided
			//    T0 and T1 are affine_rt type that contains a translation (vec3) and a rotation element
			//    alpha is the interpolation value \in [0,1]
			//    You have to find which values to set on T.translation and T.rotation
			
			skeleton_current[kj] = T0; // Change this line
		}

		return skeleton_current;
	}
	size_t skeleton_animation_structure::number_joint() const
	{
		return parent_index.size();
	}
	size_t skeleton_animation_structure::number_animation_frame() const
	{
		return animation_time.size();
	}

	void skeleton_animation_structure::scale(float s)
	{
		size_t const N_joint = number_joint();
		size_t const N_time = number_animation_frame();
		for(size_t k=0; k<N_joint; ++k)
			rest_pose_local[k].translation *= s;

		for(size_t kt=0; kt<N_time; ++kt)
			for(size_t k=0; k<N_joint; ++k)
				animation_geometry_local[kt][k].translation *= s;
	}
	

	numarray<affine_rt> skeleton_animation_structure::evaluate_global(float t) const
	{
		return skeleton_local_to_global(evaluate_local(t), parent_index);
	}

	numarray<affine_rt> skeleton_animation_structure::rest_pose_global() const
	{
		return skeleton_local_to_global(rest_pose_local, parent_index);
	}

	numarray<affine_rt> skeleton_local_to_global(numarray<affine_rt> const& local, numarray<int> const& parent_index)
	{
		assert_cgp(parent_index.size()==local.size(), "Incoherent size of skeleton data");
		size_t const N = parent_index.size();
		numarray<affine_rt> global;
		global.resize(N);
		global[0] = local[0];

		for (size_t k = 1; k < N; ++k)
			global[k] = global[parent_index[k]] * local[k];
	
		return global;
	}

}