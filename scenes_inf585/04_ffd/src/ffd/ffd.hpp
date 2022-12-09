#pragma once


#include "cgp/cgp.hpp"



// Precompute the weights used in the FFD
cgp::numarray<cgp::grid_3D<float>> precompute_weights(cgp::numarray<cgp::vec3>& position, int Nx, int Ny, int Nz);

// Compute the FFD deformation
void ffd_deform(cgp::numarray<cgp::vec3>& position, cgp::grid_3D<cgp::vec3> const& grid, cgp::numarray<cgp::grid_3D<float> > const& weights);




