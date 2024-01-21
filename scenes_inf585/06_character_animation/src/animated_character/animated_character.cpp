#include "animated_character.hpp"

using namespace cgp;

void character_structure::load_and_initialize(filename_loader_structure const& param_loader, affine_rts const& transform)
{
	animated_model = mesh_skinning_loader(param_loader, transform);
	for(auto const& entry: animated_model.rigged_mesh){
		std::string name = entry.first;
		drawable[name].initialize_data_on_gpu(entry.second.mesh_deformed);
		drawable[name].texture.load_and_initialize_texture_2d_on_gpu(param_loader.loader_rigged_mesh.at(name).texture);
	}
	
	sk_drawable = skeleton_drawable(animated_model.skeleton);
	set_current_animation(animated_model.animation.begin()->first);
}

void character_structure::set_current_animation(std::string const& animation_name) {
	current_animation_name = animation_name;
	timer.event_period = animated_model.animation[current_animation_name].time_max;
}

