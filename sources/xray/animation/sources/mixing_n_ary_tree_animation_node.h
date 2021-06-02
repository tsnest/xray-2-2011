////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_ANIMATION_NODE_H_INCLUDED
#define N_ARY_TREE_ANIMATION_NODE_H_INCLUDED

#include "mixing_n_ary_tree_n_ary_operation_node.h"
#include <xray/animation/mixing.h>

namespace xray {
namespace animation {

struct base_interpolator;

namespace mixing {

class animation_interval_base;
struct animation_state;
class animation_interval;

class n_ary_tree_animation_node : public n_ary_tree_n_ary_operation_node {
public:
	inline							n_ary_tree_animation_node	(
#ifndef MASTER_GOLD
										pcstr const identifier,
#endif // #ifndef MASTER_GOLD
										animation_interval const* animation_intervals_begin,
										animation_interval const* animation_intervals_end,
										u32 start_cycle_animation_interval_id,
										base_interpolator const& weight_interpolator,
										playing_type_enum playing_type,
										u32 synchronization_group_id,
										bool override_existing_animation,
										u32 additivity_priority,
										u32 operands_count,
										bool is_transitting_to_zero
									);
	inline							n_ary_tree_animation_node	(
#ifndef MASTER_GOLD
										pcstr const identifier,
#endif // #ifndef MASTER_GOLD
										n_ary_tree_animation_node& driving_animation,
										animation_interval const* animation_intervals_begin,
										animation_interval const* animation_intervals_end,
										base_interpolator const& weight_interpolator,
										playing_type_enum playing_type,
										bool override_existing_animation,
										u32 additivity_priority,
										u32 operands_count,
										bool is_transitting_to_zero
									);
	inline							n_ary_tree_animation_node	( n_ary_tree_animation_node const& other  );

	virtual	void					accept						( n_ary_tree_visitor& visitor );
	virtual void					accept						( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_base_node& node );

	inline	void					set_animation_state			( mixing::animation_state& animation_state );

	// accessors
	inline	animation_state&		animation_state				( ) const;
#ifndef MASTER_GOLD
	inline	pcstr					identifier					( ) const;
#endif // #ifndef MASTER_GOLD
	inline	base_interpolator const& interpolator				( ) const;

	// one-line accessors
	inline	animation_interval const* animation_intervals		( ) const { return m_animation_intervals		; }
	inline	n_ary_tree_animation_node* driving_animation		( ) const { return m_driving_animation			; }
	inline	u32						synchronization_group_id	( ) const { return m_synchronization_group_id	; }
	inline	u32						animation_intervals_count	( ) const { return m_animation_intervals_count	; }
	inline	u32						start_cycle_animation_interval_id		( ) const { return m_start_cycle_interval_id	; }
	inline	playing_type_enum		playing_type				( ) const { return m_playing_type				; }
	inline	u32						additivity_priority			( ) const { return m_additivity_priority		; }
	inline	bool					is_transitting_to_zero		( ) const { return m_is_transitting_to_zero		; }
	inline	bool					override_existing_animation	( ) const { return m_override_existing_animation; }


private:
	virtual void					visit						( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_animation_node& node );
	virtual void					visit						( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_weight_node& node );
	virtual void					visit						( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_time_scale_node& node );
	virtual void					visit						( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_addition_node& node );
	virtual void					visit						( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_subtraction_node& node );
	virtual void					visit						( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_multiplication_node& node );
	virtual void					visit						( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_weight_transition_node& node );
	virtual void					visit						( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_time_scale_transition_node& node );

private:
	n_ary_tree_animation_node&		operator =					( n_ary_tree_animation_node const& other );

private:
	inline	void				assign_uninitialized_user_data	( );

private:
	animation_interval const* const	m_animation_intervals;
	n_ary_tree_animation_node* const m_driving_animation;
	mixing::animation_state*		m_animation_state;
	base_interpolator const&		m_interpolator;
#ifndef MASTER_GOLD
	pcstr const						m_identifier;
#endif // #ifndef MASTER_GOLD

public:
	n_ary_tree_animation_node*		m_next_animation;
#ifndef MASTER_GOLD
	u32								user_data;
#endif // #ifndef MASTER_GOLD

private:
	u32 const						m_synchronization_group_id;
	u32	const						m_animation_intervals_count;
	u32	const						m_start_cycle_interval_id;
	playing_type_enum const			m_playing_type;
	u32 const						m_additivity_priority;
	bool const						m_is_transitting_to_zero;
	bool const						m_override_existing_animation;
}; // class n_ary_tree_animation_node

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_n_ary_tree_animation_node_inline.h"

#endif // #ifndef N_ARY_TREE_ANIMATION_NODE_H_INCLUDED