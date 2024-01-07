#include "picking_visual.hpp"

using namespace cgp;

void picking_visual_parameters::initialize()
{
	sphere.initialize_data_on_gpu(mesh_primitive_sphere(0.02f));
	circle.initialize_data_on_gpu(curve_primitive_circle(1.0f, { 0,0,0 }, { 0,0,1 }, 40));
}

void picking_visual_parameters::draw(environment_structure const& environment, vec3 const& position, vec3 const& normal, float radius)
{
	// Display a sphere representing the picked vertex
	sphere.model.translation = position;
	cgp::draw(sphere, environment);

	// Display a circle representing the radius of influence and the orientation of the surface and 
	circle.model.scaling = radius;
	circle.model.translation = position;
	circle.model.rotation = rotation_transform::from_vector_transform({ 0,0,1 }, normal);
	cgp::draw(circle, environment);
}