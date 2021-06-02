////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_TRANSITION_TREE_CONSTRUCTOR_H_INCLUDED
#define N_ARY_TREE_TRANSITION_TREE_CONSTRUCTOR_H_INCLUDED

#include "mixing_n_ary_tree_cloner.h"

namespace xray {
namespace animation {

struct base_interpolator;
struct subscribed_channel;

namespace mixing {

class n_ary_tree;
class n_ary_tree_intrusive_base;
struct animation_state;
class animation_clip;

enum interpolation_direction;

class n_ary_tree_transition_tree_constructor : private boost::noncopyable {
public:
				n_ary_tree_transition_tree_constructor					(
						mutable_buffer& buffer,
						n_ary_tree const& from,
						n_ary_tree const& to,
						u32 const animations_count,
						u32 const current_time_in_ms,
						subscribed_channel*& channels_head
					);
	n_ary_tree	computed_tree											( n_ary_tree const& previous_tree );

private:
	n_ary_tree_animation_node* add_animation_node						(
						n_ary_tree_animation_node& new_animation,
						animation_state const* previous_animation_state,
						u32 animation_interval_id,
						float animation_interval_time,
						bool is_new_animation
					);
	n_ary_tree_animation_node* add_animation_node						(
						n_ary_tree_animation_node& new_animation,
						animation_state const* previous_animation_state,
						bool is_new_animation
					);
	n_ary_tree_animation_node* new_animation							(
					n_ary_tree_animation_node& animation,
					n_ary_tree_animation_node* driving_animation,
					u32 operands_count,
					bool is_transitting_to_zero
				);
	n_ary_tree_base_node* new_time_scale_transition	(
					n_ary_tree_animation_node& from_animation,
					n_ary_tree_base_node& from,
					n_ary_tree_base_node& to,
					base_interpolator const& to_interpolator
				);
	n_ary_tree_base_node* new_time_scale_transition	(
					n_ary_tree_animation_node& from_animation,
					n_ary_tree_base_node& from,
					float to
				);
	n_ary_tree_base_node* new_time_scale_transition	(
					float const animation_time,
					float from,
					n_ary_tree_base_node& to,
					base_interpolator const& to_interpolator
				);

	n_ary_tree_base_node* new_weight_transition			(
					base_interpolator const& interpolator,
					float from,
					float to
				);
	n_ary_tree_base_node* new_weight_transition			(
					n_ary_tree_base_node& from,
					n_ary_tree_base_node& to
				);
	n_ary_tree_base_node* new_weight_transition			(
					base_interpolator const& from_animation_interpolator,
					n_ary_tree_base_node& from,
					float to
				);
	n_ary_tree_base_node* new_weight_transition			(
					base_interpolator const& to_animation_interpolator,
					float from,
					n_ary_tree_base_node& to
				);

	void		add_operands											(
					n_ary_tree_animation_node& from,
					n_ary_tree_animation_node& to,
					n_ary_tree_base_node** operands_begin,
					n_ary_tree_base_node** operands_end,
					bool const skip_time_scale_node
				);

	n_ary_tree_animation_node* add_animation							(
					n_ary_tree_animation_node& animation,
					n_ary_tree_animation_node* const driving_animation,
					base_interpolator const& interpolator
				);
	n_ary_tree_animation_node* remove_animation							(
					n_ary_tree_animation_node& animation,
					n_ary_tree_animation_node* const driving_animation,
					base_interpolator const& interpolator
				);
	void		change_animation										(
					n_ary_tree_animation_node& from,
					n_ary_tree_animation_node& to,
					n_ary_tree_animation_node* const driving_animation
				);

	void		merge_asynchronous_groups								(
					n_ary_tree_animation_node* const from_begin,
					n_ary_tree_animation_node* const from_end,
					n_ary_tree_animation_node* const to_begin,
					n_ary_tree_animation_node* const to_end
				);
	void		merge_synchronization_groups							(
					n_ary_tree_animation_node* from_begin,
					n_ary_tree_animation_node* from_end,
					n_ary_tree_animation_node* to_begin,
					n_ary_tree_animation_node* to_end,
					n_ary_tree_animation_node& new_driving_animation,
					base_interpolator const& interpolator
				);

	n_ary_tree_animation_node* new_driving_animation					(
					n_ary_tree_animation_node& animation,
					u32 const animation_interval_id,
					float const animation_interval_time,
					n_ary_tree_base_node* const time_scale_node,
					base_interpolator const& time_scale_interpolator
				);
	n_ary_tree_animation_node* new_driving_animation					(
					n_ary_tree_animation_node& new_driving_animation,
					n_ary_tree_animation_node& new_driving_animation_in_previous_target,
					u32 const animation_interval_id,
					float const animation_interval_time,
					n_ary_tree_base_node* const time_scale_node
				);

	void		add_synchronization_group								(
					n_ary_tree_animation_node* begin,
					n_ary_tree_animation_node* end
				);
	void		remove_synchronization_group							(
					n_ary_tree_animation_node* begin,
					n_ary_tree_animation_node* end
				);
	void		change_synchronization_group							(
					n_ary_tree_animation_node* from_begin,
					n_ary_tree_animation_node* from_end,
					n_ary_tree_animation_node* to_begin,
					n_ary_tree_animation_node* to_end
				);
	void		merge_trees												(
					n_ary_tree const& from,
					n_ary_tree const& to
				);

private:
	n_ary_tree_cloner					m_cloner;
	mutable_buffer&						m_buffer;
	n_ary_tree_animation_node*			m_result;
	animation_state*					m_animation_states;
	animation_state*					m_new_animation_state;
	animation_state**					m_animation_events;
	animation_state**					m_new_animation_event;
	n_ary_tree_intrusive_base*			m_reference_counter;
	n_ary_tree_animation_node*			m_previous_animation;
	subscribed_channel*&	m_channels_head;
	u32									m_current_time_in_ms;
	u32									m_animations_count;
}; // class n_ary_tree_transition_tree_constructor

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef N_ARY_TREE_TRANSITION_TREE_CONSTRUCTOR_H_INCLUDED