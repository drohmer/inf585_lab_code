#pragma once

#include "cgp/cgp.hpp"


void initialize_density_color(cgp::grid_2D<cgp::vec3>& density, size_t N);
void initialize_density_visual(cgp::mesh_drawable& density_visual, size_t N);
void initialize_grid(cgp::curve_drawable& grid_visual, size_t N);

void update_velocity_visual(cgp::curve_drawable& velocity_visual, cgp::numarray<cgp::vec3>& velocity_grid_data, cgp::grid_2D<cgp::vec2> const& velocity, float scale);

void mouse_velocity_to_grid(cgp::grid_2D<cgp::vec2>& velocity, cgp::vec2 const& mouse_velocity, cgp::mat4 const& P_inv, cgp::vec2 const& p_mouse);
void density_to_velocity_curl(cgp::grid_2D<cgp::vec3>& density, cgp::grid_2D<cgp::vec2> const& velocity);