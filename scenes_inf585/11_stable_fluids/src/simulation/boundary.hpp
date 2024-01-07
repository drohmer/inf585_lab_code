#pragma once

#include "cgp/cgp.hpp"

enum boundary_condition { copy, reflective };


template <typename T> void set_boundary_corners(cgp::grid_2D<T>& grid);
template <typename T> void set_boundary(cgp::grid_2D<T>& grid);
template <typename T> void set_boundary_reflective(cgp::grid_2D<T>& grid);




template <typename T>
void set_boundary_corners(cgp::grid_2D<T>& grid)
{
    int const Nx = int(grid.dimension.x);
    int const Ny = int(grid.dimension.y);

    grid(0,0)       = (grid(1,0) + grid(0,1))/2.0f;
    grid(Nx-1,0)    = (grid(Nx-2,0) + grid(Nx-1,1))/2.0f;
    grid(0,Ny-1)    = (grid(1,Ny-1) + grid(0,Ny-2))/2.0f;
    grid(Nx-1,Ny-1) = (grid(Nx-2,Ny-1) + grid(Nx-1,Ny-2))/2.0f;
}


template <typename T>
void set_boundary(cgp::grid_2D<T>& grid)
{

    int const Nx = int(grid.dimension[0]);
    int const Ny = int(grid.dimension[1]);

    for(int x=1; x<Nx-1; ++x) {
        grid(x,0)    = grid(x,1);
        grid(x,Ny-1) = grid(x,Ny-2);
    }

    for(int  y=1; y<Ny-1; ++y) {
        grid(0,y)    = grid(1,y);
        grid(Nx-1,y) = grid(Nx-2,y);
    }

    set_boundary_corners(grid);
}


template <typename T>
void set_boundary_reflective(cgp::grid_2D<T>& grid)
{
	set_boundary(grid);
    int const Nx = int(grid.dimension[0]);
    int const Ny = int(grid.dimension[1]);

    for(int  x=1; x<Nx-1; ++x) {
        grid(x,0).y *= -1.0f;
        grid(x,Ny-1).y *= -1.0f;
    }

    for(int y=1; y<Ny-1; ++y) {
        grid(0,y).x *= -1.0f;
        grid(Nx-1,y).x *= -1.0f;
    }

}

