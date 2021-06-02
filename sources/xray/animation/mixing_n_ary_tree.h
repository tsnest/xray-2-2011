////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_MIXING_N_ARY_TREE_H_INCLUDED
#define XRAY_ANIMATION_MIXING_N_ARY_TREE_H_INCLUDED

#include <xray/animation/mixing_n_ary_tree_intrusive_base.h>

#if 1//ndef DEBUG
#	define XRAY_NORMALIZE_ANIMATIONS_WEIGHTS
#endif // #ifndef DEBUG

namespace xray {
namespace animation {

struct base_interpolator;
class bone_mixer;
class skeleton_bone;
class skeleton;
struct editor_animation_state;
struct callback_generator_info;
struct subscribed_channel;

namespace mixing {

class n_ary_tree_animation_node;
struct animation_state;

template class XRAY_ANIMATION_API intrusive_ptr <
	n_ary_tree_intrusive_base,
	n_ary_tree_intrusive_base,
	threading::single_threading_policy
>;

class XRAY_ANIMATION_API n_ary_tree {
public:
	typedef	intrusive_ptr <
				n_ary_tree_intrusive_base,
				n_ary_tree_intrusive_base,
				threading::single_threading_policy
			>						n_ary_tree_ptr;

public:
	inline	explicit	n_ary_tree					( float4x4 const& object_transform );
						n_ary_tree					(
							n_ary_tree_animation_node* const root,
							base_interpolator const** const interpolators,
							animation_state* const animation_states,
							animation_state** const animation_events,
							n_ary_tree_intrusive_base* const reference_counter,
							u32 const animations_count,
							u32 const interpolators_count,
							u32 const target_time_in_ms,
							n_ary_tree const& previous_tree
						);
						n_ary_tree					(
							n_ary_tree_animation_node* const root,
							base_interpolator const** const interpolators,
							animation_state* const animation_states,
							animation_state** const animation_events,
							n_ary_tree_intrusive_base* const reference_counter,
							u32 const animations_count,
							u32 const interpolators_count,
							u32 const target_time_in_ms,
							float4x4 const& object_transform
						);
						~n_ary_tree					( );
			void		tick						(
							u32 target_time_in_ms,
							subscribed_channel*& channels_head,
							bool& callbacks_are_actual
						);
			void		set_object_transform		( float4x4 const& transform );
			float4x4	get_object_transform		( ) const;
			void		compute_bones_matrices		( skeleton const& skeleton, float4x4* const begin, float4x4* const end ) const;
			float4x4	computed_bone_matrix		( skeleton_bone const& bone ) const;
#ifndef MASTER_GOLD
			void		dump_animation_states		( u32 const target_time_in_ms ) const;
			void		dump_tree					( u32 const target_time_in_ms ) const;
#endif // #ifndef MASTER_GOLD

	inline	bool		are_there_any_animations	( ) const { return m_animations_count > 0; }
	inline	u32			interpolators_count			( ) const { return m_interpolators_count; }
	inline	base_interpolator const** interpolators	( ) const { return m_interpolators; }
	inline	n_ary_tree_animation_node* root			( ) const { return m_root; }

#ifndef MASTER_GOLD
			void		fill_animation_states		( vectora< animation::editor_animation_state >& result );
#endif // #ifndef MASTER_GOLD

private:
	enum process_event_result_enum {
		process_event_result_animation_removed,
		process_event_result_nothing_to_do,
	}; // enum process_event_result_enum

private:
			void		initialize					( );
			void		destroy						( );

			float4x4	computed_local_bone_matrix	( skeleton_bone const& bone ) const;
			void		compute_skeleton_branch		( skeleton_bone const& bone, float4x4* const result, float4x4 const& parent ) const;

			void	update_animation_interval_time	(
							n_ary_tree_animation_node& animation,
							u32 const start_time_in_ms,
							u32 const target_time_in_ms
						);

			void		accumulate_object_movement	(
							n_ary_tree_animation_node& animation_node,
							float const animation_time,
							u32 const time_in_ms
						);
			void		accumulate_object_movement	(
							n_ary_tree_animation_node& animation,
							u32 start_time_in_ms,
							float start_animation_time,
							u32 target_time_in_ms
						);

			void		update_animation_state		(
							n_ary_tree_animation_node& animation_node,
							u32 const start_time_in_ms,
							u32 target_time_in_ms
						);

			void	update_synchronization_group_using_integration	(
							n_ary_tree_animation_node& animation_node,
							u32 const start_time_in_ms,
							u32 const target_time_in_ms
						);
			void		update_synchronization_group(
							n_ary_tree_animation_node& animation_node,
							u32 const start_time_in_ms,
							u32 const target_time_in_ms
						);
			void		update_animation_states		( u32 start_time_in_ms, u32 target_time_in_ms );
			bool		need_new_transform			( u32 const target_time_in_ms ) const;

			void		dispatch_callbacks			( u32 const target_time_in_ms ) const;

			void		remove_animation			(
							n_ary_tree_animation_node*& i,
							n_ary_tree_animation_node* j
						);
			void		set_object_transform		( n_ary_tree_animation_node& animation_node );
			process_event_result_enum process_event	(
							n_ary_tree_animation_node*& current_animation_node,
							n_ary_tree_animation_node* previous_animation_node,
							u32 event_types
						);
			void		process_events				( u32 target_time_in_ms, u32 event_types );
			void		update_event_iterators		( u32 target_time_in_ms );
			void		dispatch_callbacks			(
							callback_generator_info const* const callback_generators_head,
							subscribed_channel*& channels_head,
							u32 const current_time_in_ms,
							bool& callbacks_are_actual
						);
			void update_event_iterators_and_dispatch_callbacks	(
							u32 const target_time_in_ms,
							subscribed_channel*& channels_head,
							bool& callbacks_are_actual
						);
	static	void		update_animation_time		( animation_state& animation_state );

#ifdef XRAY_NORMALIZE_ANIMATIONS_WEIGHTS
			void		normalize_weights			( );
#endif // #ifdef XRAY_NORMALIZE_ANIMATIONS_WEIGHTS

private:
	float4x4					m_object_transform;
	n_ary_tree_ptr				m_reference_counter;
	n_ary_tree_animation_node*	m_root;
	base_interpolator const**	m_interpolators;
	animation_state*			m_animation_states;
	animation_state**			m_animation_events;
	u32							m_animations_count;
	u32							m_interpolators_count;
	u32							m_tree_actual_time_in_ms;
}; // class n_ary_tree

} // namespace mixing
} // namespace animation
} // namespace xray

#include <xray/animation/mixing_n_ary_tree_inline.h>

#endif // #ifndef XRAY_ANIMATION_MIXING_N_ARY_TREE_H_INCLUDED