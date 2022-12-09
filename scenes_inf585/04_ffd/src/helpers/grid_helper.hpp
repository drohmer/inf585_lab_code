#pragma once

#include "cgp/cgp.hpp"

cgp::grid_3D<cgp::vec3> initialize_grid(int Nx, int Ny, int Nz);
void update_grid_segments(cgp::numarray<cgp::vec3>& segments_grid, cgp::grid_3D<cgp::vec3> const& grid);

