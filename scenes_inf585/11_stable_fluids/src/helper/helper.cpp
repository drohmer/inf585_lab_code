#include "helper.hpp"

using namespace cgp;

void initialize_density_color(grid_2D<vec3>& density, size_t N)
{
    density.resize(N,N);
    density.fill({1,1,1});
    for(size_t kx=N/6; kx<N/2; ++kx)
        for(size_t ky=N/6; ky<5*N/6; ++ky)
            density(kx,ky) = {1,0,0};
    for(size_t kx=N/2; kx<5*N/6; ++kx)
        for(size_t ky=N/6; ky<5*N/6; ++ky)
            density(kx,ky) = {0,1,0};

}

void initialize_density_visual(mesh_drawable& density_visual, size_t N)
{
	float const dx = 2.0f/(N-1.0f);
	density_visual.initialize_data_on_gpu(mesh_primitive_quadrangle({-1-dx/2,-1-dx/2,0},{1+dx/2,-1-dx/2,0},{1+dx/2,1+dx/2,0}, {-1-dx/2, 1+dx/2, 0}) );
	
	density_visual.material.phong = {1,0,0,1};
	density_visual.material.color = {1,1,1};
	density_visual.material.texture_settings.inverse_v = false;
}

void initialize_grid(curve_drawable& grid_visual, size_t N)
{

    numarray<vec3> edges;
	float const e = 1e-4f;
    float const dx = 2.0f/(N-1.0f);
    for(int kx=0; kx<=N; ++kx) {
        float const x  = kx*dx-1;
        edges.push_back( vec3(x-dx/2, -dx/2-1, e) );
        edges.push_back( vec3(x-dx/2, 1+dx/2, e) );
    }
    for(int ky=0; ky<=N; ++ky) {
        float const y  = ky*dx-1;
        edges.push_back( vec3(-dx/2-1, y-dx/2, e) );
        edges.push_back( vec3(1+dx/2, y-dx/2, e) );
    }

    grid_visual.initialize_data_on_gpu(edges);
}

void update_velocity_visual(curve_drawable& velocity_visual, numarray<vec3>& velocity_grid_data, grid_2D<vec2> const& velocity, float scale)
{
	int const N = int(velocity.dimension.x);
	float const dL = 2.0f/(N-1.0f);
	float const lambda = 0.01f * scale;

	for(int kx=0; kx<N; ++kx){
		for(int ky=0; ky<N; ++ky){
			vec3 const p0 = {-1+kx*dL, -1+ky*dL, 1e-4f};
			size_t const offset = velocity.index_to_offset(kx,ky);
			velocity_grid_data[2*offset+0] = p0;
			velocity_grid_data[2*offset+1] = p0 + lambda*vec3(velocity(kx,ky),0.0f);
		}
	}

	velocity_visual.vbo_position.update(velocity_grid_data);
}

void density_to_velocity_curl(grid_2D<vec3>& density, grid_2D<vec2> const& velocity)
{
	size_t const N = velocity.dimension.x;
	float const dL = 2.0f/(N-1);
	for (size_t kx = 0; kx < N-1; ++kx){
		for (size_t ky = 0; ky < N-1; ++ky){

			float const w = 0.01f*((velocity(kx+1,ky).y-velocity(kx,ky).y)/dL - (velocity(kx,ky+1).x-velocity(kx,ky).x)/dL);
			float const wp = clamp(w,0,1);
			float const wn = clamp(-w,0,1);

			float const r = 1-wn;
			float const g = 1-wp-wn;
			float const b = 1-wp;

			density(kx,ky) = vec3{r,g,b};
		}
	}
}

void mouse_velocity_to_grid(grid_2D<vec2>& velocity, vec2 const& mouse_velocity, mat4 const& P_inv, vec2 const& p_mouse)
{
    size_t const N = velocity.dimension.x;
	float const L = 2.0f/(N-1.0f);

	vec2 const picked = (P_inv*vec4(p_mouse,0,1)).xy();
	vec2 const p0 = vec2(-1-L/2,-1-L/2); // grid extremity
	vec2 const p1 = vec2( 1+L/2, 1+L/2);
	int const x = std::floor( N*(picked.x-p0.x)/(p1.x-p0.x) );
	int const y = std::floor( N*(picked.y-p0.y)/(p1.y-p0.y) );
	for (int dx = -5; dx < 5; ++dx)
	{
		for (int dy = -5; dy < 5; ++dy)
		{
			int const xc = x+dx;
			int const yc = y+dy;

			if(xc>1 && yc>1 && xc<int(N-2) && yc<int(N-2)) {
				// Set mouse speed to the corresponding entry of the velocity
				float const dist = norm(picked-vec2{-1-L/2+xc*L, -1-L/2+yc*L});
				float const weight = exp(-(dist*dist)/(0.05f*0.05f));

				velocity(xc,yc) += 5.0f * weight * mouse_velocity;
			}
		}
	}
}