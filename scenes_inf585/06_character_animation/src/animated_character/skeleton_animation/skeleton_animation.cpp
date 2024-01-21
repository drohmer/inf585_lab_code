#include "skeleton_animation.hpp"

using namespace cgp;

static bool find_relative_placement_in_array(numarray<float> const& times, float t, int& index_placement, float& ratio_placement);

mat4 skeleton_animation_structure::evaluate(int k_joint, float t) const {

    cgp::numarray<float> const& time_array = times[k_joint];
    int N_time = time_array.size();

    int idx0;
    float alpha;
    find_relative_placement_in_array(time_array, t, idx0, alpha);


    mat4 M;
    if(idx0<N_time-1){
        mat4 const& M0 = matrix[k_joint][idx0];
        mat4 const& M1 = matrix[k_joint][idx0+1];
        M = (1.0f-alpha)*M0 + alpha*M1;
    }
    if(idx0>=N_time-1) {
        M = matrix[k_joint][N_time-1];
    }

    return M;
}


void skeleton_animation_structure::update_time_max()
{
    time_max = times[0][0];
    int const N_joint = times.size();
    for(int k=1; k<N_joint; ++k) {
        time_max = std::max(time_max, times[k][times[k].size()-1]);
    }

}

// Helper function that finds the relative position of the time t in an array of discrete key time values [t_0,t_1,...t_N].
//  Return
//    index_placement: i such that t_i < t < t_{i+1}
//    ratio_placement: the relative position of t between its two discrete key time r = (t-t_i) / (t_{i+1}-t_i)
static bool find_relative_placement_in_array(numarray<float> const& times, float t, int& index_placement, float& ratio_placement)
{
    assert_cgp(times.size()>=2, "time intervals should have more than 2 values");

    size_t const N = times.size();
    if(t<times[0]) {
        index_placement = 0;
        ratio_placement = 0.0f;
        return false;
    }
    if(t>=times[N-1]) {
        index_placement = N-1;
        ratio_placement = 0.0f;
        return false;
    }


    int current_index = 0;
    while (t > times[current_index + 1]) {
        assert_cgp_no_msg(N>current_index);
        ++current_index;
    }
    index_placement = current_index;

    float const t0 = times[current_index];
    float const t1 = times[current_index+1];
    float const dt = t1-t0;
    assert_cgp(std::abs(dt)>0, "Time interval should be > 0");

    ratio_placement = (t-t0)/dt;
    return true;
}