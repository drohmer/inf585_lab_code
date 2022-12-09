#include "simulation.hpp"

using namespace cgp;



void divergence_free(grid_2D<vec2>& new_velocity, grid_2D<vec2> const& velocity, grid_2D<float>& divergence, grid_2D<float>& gradient_field)
{
    // v = projection of v0 on divergence free vector field
    //
    // v : Final vector field to be filled
    // v0: Initial vector field (non divergence free)
    // divergence: temporary buffer used to compute the divergence of v0
    // gradient_field: temporary buffer used to compute v = v0 - nabla(gradient_field)


    // TO do:
    // 1. Compute divergence of v0
    // 2. Compute gradient_field such that nabla(gradient_field)^2 = div(v0)
    // 3. Compute v = v0 - nabla(gradient_field)

}