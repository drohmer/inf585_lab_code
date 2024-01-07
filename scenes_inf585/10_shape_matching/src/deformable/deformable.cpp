#include "deformable.hpp"



void shape_deformable_structure::initialize(cgp::mesh const& shape)
{
    drawable.initialize_data_on_gpu(shape);

	position = shape.position;
	position_reference = position;
	position_predict = position;
	normal = shape.normal;
	connectivity = shape.connectivity;


	velocity.resize(position.size());
	com = average(position);
	com_reference = average(position_reference);
}

void shape_deformable_structure::set_position_and_velocity(cgp::vec3 translation, cgp::vec3 linear_velocity, cgp::vec3 angular_velocity)
{
    // Apply the translation
	position = add(position, translation); 
    // Update the center of mass
	com = average(position);

	// Add linear velocity to the velocity structure
	velocity = add(velocity,linear_velocity);

	// Add angular velocity to the velocity structure
	for(int k=0; k<size(); ++k) {
		velocity[k] += cross(angular_velocity, position[k]-com);
	}
}

int shape_deformable_structure::size() const {
    return position.size();
}


void shape_deformable_structure::update_drawable() {
    drawable.vbo_position.update(position);
    normal_per_vertex(position, connectivity, normal);
    drawable.vbo_normal.update(normal);
}