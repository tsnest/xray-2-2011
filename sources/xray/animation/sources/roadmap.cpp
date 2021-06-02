//////////////////////////////////////////////////////////////////////////
//	Created		: 19.04.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
//////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if 0

struct cubic_spline {};

struct channel {
	cubic_spline;
};

struct bone {
	channel<float3>	translation;
	channel<float3>	rotation;
	channel<float3>	scale;
	channel<bool>	visibility;
};

struct animation_data {
	bones[];
};

struct event {};

struct event_channel {
	events[];
};

struct animation {
//	animation_data;
	cubic_spline_joint_animation joints[];
	event_channels[];
	type; // full/additive
};

struct animation_interval {
	animation;
	start_offset;
	length;
};

struct animation_clip {
	animation_intervals[];
	cycle_start_interval_id; // if more than intervals count than no cycle
	// animation clip type (full/additive) should be the same for all the animations in animation intervals
};

struct event_callback {
	functor;
	event_channel_id;
};

struct binary_animation_lexeme {
	animation_clip;
	event_callbacks[];
}; // struct binary_animation_lexeme

struct binary_expression {};

struct n_ary_tree {
	void				tick							( time );
};

struct animation_mixer {
	void				set_target						( binary_expression, time );

	// tick should move through channel events, ticking on every event in the order they appear
	// and recalculating the next event time for the active event only (optimization)
	// events may change n_ary_tree, so in the case of change, we should recompute other animation events times as well
	void				tick							( time );
	
	weights_type const& get_weights						( ) const;
	
	void				add_object_transform_modifier	( transform_modifier* modifier );
	void				remove_object_transform_modifier( transform_modifier* modifier );
};

struct linear_channel_mixer {};
struct rotation_channel_mixer {};
struct object_transform_mixer {};

struct bone_mixer {
	linear_channel_mixer<float3>	m_translation_channel_mixer;
	linear_channel_mixer<float3>	m_scale_channel_mixer;
	linear_channel_mixer<bool>		m_visibility_channel_mixer;
	rotation_channel_mixer			m_rotation_channel_mixer;
	object_transform_mixer			m_object_transform_mixer;

	void				compute_bones_matrices			(
							skeleton const& skeleton,
							animation_states_type const& animation_states,
							matrices_type& matrices,
							float4x4& object_transform
						)
	{
		
	}
};

struct animation_player {
	void				set_target_and_tick				( binary_expression, float time );
	void				tick							( float time );

	animation_mixer;
	bone_mixer;
	footsteps_position_modifier;
};

void foo	( )
{
	animation_mixer animation_mixer;
	animation_mixer.set_target_and_tick	( animation_expression, current_time );
	
	bone_mixer bone_mixer;
	bone_mixer.compute_bones_matrices	(
		skeleton,
		animation_mixer.get_animations_states( ),
		matrices,
		object_transform
	);
}

struct animation_event {
	animation_interval_id;
	animation_interval_time;
	event_time_in_ms;
	event_type;
};

struct time_calculator_data {
	last_integration_interval_id;
	last_integration_interval_value;
};

struct animation_state {
	struct bone_matrices_computer_data {
		pinned_animation;
		animation_time;									// redundant
		animation_weight;
		struct object_movement_data {
			object_transform_at_time_animation_started;	// redundant
			inverted_animated_object_transform_at_time_animation_started;
			struct accumulated_object_movement {
				translation;
				rotation;
				scale;
				time_in_ms;								// redundant
			};
		};
	};
	struct event_iterator {
		struct animation_event_iterator {
			animation_node;								// redundant
			animation_event;
			time_calculator_data;						// redundant, should speedup computations
		};
		struct time_scale_event_iterator {
			animation_node;								// redundant
			time_in_ms;
		};
		struct weight_event_iterator {
			animation_node;								// redundant
			time_in_ms;
		};
		event_iterator_state;
		animation_event;
	};
	time_calculator_data;

	animation_interval_id;
	animation_interval_time;

	animation_interval_start_time_in_ms;				// ?
}; // struct animation_state

#endif // #if 0