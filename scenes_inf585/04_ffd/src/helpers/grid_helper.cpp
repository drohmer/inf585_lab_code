#include "grid_helper.hpp"

using namespace cgp;

grid_3D<vec3> initialize_grid(int Nx, int Ny, int Nz)
{
    grid_3D<vec3> grid;
    grid.resize(Nx,Ny,Nz);
	for (int kx = 0; kx < Nx; ++kx) {
		for (int ky = 0; ky < Ny; ++ky) {
			for (int kz = 0; kz < Nz; ++kz) {

				float const x = kx/(Nx-1.0f);
				float const y = ky/(Ny-1.0f);
				float const z = kz/(Nz-1.0f);

				grid(kx,ky,kz) = {x,y,z};

			}
		}
	}
    return grid;
}


void update_grid_segments(numarray<vec3>& segments_grid, grid_3D<vec3> const& grid)
{
	int const Nx = int(grid.dimension.x);
	int const Ny = int(grid.dimension.y);
	int const Nz = int(grid.dimension.z);

	int N_edge = 2*(Nx-1)*Ny*Nz + 2*Nx*(Ny-1)*Nz + 2*Nx*Ny*(Nz-1);
	if(segments_grid.size()<N_edge)
		segments_grid.resize(N_edge);

	int offset = 0;
	for (int kx = 0; kx < Nx-1; ++kx) {
		for (int ky = 0; ky < Ny; ++ky) {
			for (int kz = 0; kz < Nz; ++kz) {
				vec3 const p0 = grid(kx,ky,kz);
				vec3 const p1 = grid(kx+1,ky,kz);
				segments_grid[offset] = p0; offset++;
				segments_grid[offset] = p1; offset++;
			}
		}
	}

	for (int kx = 0; kx < Nx; ++kx) {
		for (int ky = 0; ky < Ny-1; ++ky) {
			for (int kz = 0; kz < Nz; ++kz) {
				vec3 const p0 = grid(kx,ky,kz);
				vec3 const p1 = grid(kx,ky+1,kz);
				segments_grid[offset] = p0; offset++;
				segments_grid[offset] = p1; offset++;
			}
		}
	}

	for (int kx = 0; kx < Nx; ++kx) {
		for (int ky = 0; ky < Ny; ++ky) {
			for (int kz = 0; kz < Nz-1; ++kz) {
				vec3 const p0 = grid(kx,ky,kz);
				vec3 const p1 = grid(kx,ky,kz+1);
				segments_grid[offset] = p0; offset++;
				segments_grid[offset] = p1; offset++;
			}
		}
	}
	
}