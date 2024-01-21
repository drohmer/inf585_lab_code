#pragma once

#include "cgp/cgp.hpp"

#include "../animated_character/animated_character.hpp"

// Structure storing state to compute a transition between two animations: source -> destination
struct effect_transition_structure {
	std::string destination_anim; // The name of the destination animation
	std::string source_anim;      // The name of the source animation (the one before the transition request)
	cgp::timer_event_periodic timer_source_anim;      // Local timer associated to the source animation
	cgp::timer_event_periodic timer_destination_anim; // Local timer associated to the destination animation
	cgp::timer_basic timer_completion; // A timer going from 0 to 1 indicating the ratio of completion of the transition. 
	float transition_time = 2.0f; // The total time took for a transition
	bool active = false; // Is a transition currently active
};

// Structure storing state to handle the walking effect
struct effect_walking_structure {
	std::string walk_anim_name = "Walk";
	std::string idle_anim_name = "Idle";
	cgp::vec3 root_position; // stores the current root position
	float root_angle;        // stores the orientation of the character
	bool active = false;    // Is the walk effect currently active
	cgp::timer_basic timer;
};

// Structure storing state to handle the Inverse Kinematics data
//  The joint at index joint_target, should target the position given as target_position+target_offset
//  The target_offset is controled via the GUI, as an offset relative to the initial position.
struct effect_ik_structure {
	int joint_target = 26;   // Index of the target joint IK
	int joint_root_ik = 7;  // Index of the start of the kinematics chain
	cgp::vec3 target_position; // Base target position
	cgp::vec3 target_offset;   // Offset of the target position (to be added to objective_position)
	bool active = false; // Is the IK effect currently active
};


// Update a transition_structure before starting a new transition
void effect_transition_start(effect_transition_structure& effect_transition, character_structure& character, std::string const& destination_anim);
// Check if a transition need to be de-activated if it is fully completed
void effect_transition_stop_if_completed(effect_transition_structure& transition, character_structure& character);
// Compute the skeleton (in the character structure) to correspond to a smooth transition between two animation indicated in the effect_transition structure
void effect_transition_compute(effect_transition_structure& effect_transition, character_structure& character);

// Rotate the joint of the head to look toward the given objective position (if possible)
void effect_rotate_head_toward_objective_position(skeleton_structure& skeleton, int joint_head_index, cgp::vec3 const& objective_position);

// Compute an Inverse Kinematic 
void effect_ik_compute(effect_ik_structure const& effect_ik, skeleton_structure& skeleton);
// Update the current position of the IK constraint to match at its start the current joint position
void effect_ik_start(effect_ik_structure& effect_ik, skeleton_structure& skeleton, int joint_end);

// Change of position of the character when a directional key is pressed
void effect_walking(effect_walking_structure& effect_walking, character_structure& character, cgp::input_devices const& inputs, effect_transition_structure const& effect_transition);
// Change of animation when we press or release the UP key during a walk effect
void effect_walking_keyboard_event(effect_transition_structure& effect_transition, character_structure& character, cgp::input_devices const& inputs, effect_walking_structure const& effect_walking);

