
#include "initialization.hpp"
#include "environment.hpp"

using namespace cgp;


mesh initialize_cylinder()
{
    float const h = 0.8f;
    float const radius = 0.3f;
    return mesh_primitive_cylinder(radius, { 0.5f,(1 - h) / 2,0.5f }, { 0.5f,1 - (1 - h) / 2,0.5f }, 15, 30);
}
mesh initialize_sphere()
{
    int const N = 20;
    float const radius = 0.4f;
    return mesh_primitive_sphere(radius, { 0.5f,0.5f,0.5f }, N, 2 * N);
}
mesh initialize_cube()
{
    int const N = 10;
    mesh cubic = mesh_primitive_cubic_grid({ 0,0,0 }, { 1,0,0 }, { 1,1,0 }, { 0,1,0 }, { 0,0,1 }, { 1,0,1 }, { 1,1,1 }, { 0,1,1 }, N, N, N);
    for (auto& p : cubic.position)
        p = 0.8f * p + vec3(0.1f, 0.1f, 0.1f);
    return cubic;
}
mesh initialize_mesh()
{
    mesh shape = mesh_load_file_obj(project::path+"assets/bunny.obj");
    shape.position *= 0.6f;
    for(auto& p : shape.position)
        p += vec3(0.7f, 0.1f, 0.5f);
    return shape;
}

