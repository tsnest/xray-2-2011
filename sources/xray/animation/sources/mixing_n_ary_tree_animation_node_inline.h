////////////////////////////////////////////////////////////////////////////
//	Created		: 26.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_ANIMATION_NODE_INLINE_H_INCLUDED
#define N_ARY_TREE_ANIMATION_NODE_INLINE_H_INCLUDED

#include <xray/animation/mixing_animation_interval.h>
#include <xray/animation/i_editor_mixer.h>

namespace xray {
namespace animation {
namespace mixing {

inline n_ary_tree_animation_node::n_ary_tree_animation_node						(
#ifndef MASTER_GOLD
		pcstr const identifier,
#endif // #ifndef MASTER_GOLD
		animation_interval const* const animation_intervals_begin,
		animation_interval const* const animation_intervals_end,
		u32 const start_cycle_animation_interval_id,
		base_interpolator const& weight_interpolator,
		playing_type_enum const playing_type,
		u32 const synchronization_group_id,
		bool const override_existing_animation,
		u32 const additivity_priority,
		u32 const operands_count,
		bool const is_transitting_to_zero
	) :
	n_ary_tree_n_ary_operation_node	( operands_count ),
#ifndef MASTER_GOLD
	m_identifier					( identifier ),
#endif // #ifndef MASTER_GOLD
	m_animation_state				( 0 ),
	m_next_animation				( 0 ),
	m_driving_animation 			( 0 ),
	m_animation_intervals			( animation_intervals_begin ),
	m_animation_intervals_count		( u32(animation_intervals_end - animation_intervals_begin) ),
	m_start_cycle_interval_id		( start_cycle_animation_interval_id ),
	m_interpolator					( weight_interpolator ),
	m_playing_type					( playing_type ),
	m_synchronization_group_id		( synchronization_group_id ),
	m_is_transitting_to_zero		( is_transitting_to_zero ),
	m_override_existing_animation	( override_existing_animation ),
	m_additivity_priority			( additivity_priority )
{
	assign_uninitialized_user_data	( );
}

inline n_ary_tree_animation_node::n_ary_tree_animation_node						(
#ifndef MASTER_GOLD
		pcstr const identifier,
#endif // #ifndef MASTER_GOLD
		n_ary_tree_animation_node& driving_animation,
		animation_interval const* const animation_intervals_begin,
		animation_interval const* const animation_intervals_end,
		base_interpolator const& weight_interpolator,
		playing_type_enum const playing_type,
		bool const override_existing_animation,
		u32 const additivity_priority,
		u32 const operands_count,
		bool const is_transitting_to_zero
	) :
	n_ary_tree_n_ary_operation_node	( operands_count ),
#ifndef MASTER_GOLD
	m_identifier					( identifier ),
#endif // #ifndef MASTER_GOLD
	m_driving_animation				( &driving_animation ),
	m_animation_intervals			( animation_intervals_begin ),
	m_animation_intervals_count		( u32(animation_intervals_end - animation_intervals_begin) ),
	m_interpolator					( weight_interpolator ),
	m_playing_type					( playing_type ),
	m_override_existing_animation	( override_existing_animation ),
	m_additivity_priority			( additivity_priority ),
	m_is_transitting_to_zero		( is_transitting_to_zero ),
	m_animation_state				( 0 ),
	m_next_animation				( 0 ),
	m_synchronization_group_id		( driving_animation.synchronization_group_id() ),
	m_start_cycle_interval_id		( driving_animation.start_cycle_animation_interval_id() )
{
	assign_uninitialized_user_data	( );
}

inline n_ary_tree_animation_node::n_ary_tree_animation_node						( n_ary_tree_animation_node const& other ) :
	n_ary_tree_n_ary_operation_node	( other ),
#ifndef MASTER_GOLD
	m_identifier					( other.m_identifier ),
#endif // #ifndef MASTER_GOLD
	m_driving_animation				( other.m_driving_animation ),
	m_animation_intervals			( other.m_animation_intervals ),
	m_animation_intervals_count		( other.m_animation_intervals_count ),
	m_start_cycle_interval_id		( other.m_start_cycle_interval_id ),
	m_interpolator					( other.m_interpolator ),
	m_playing_type					( other.m_playing_type ),
	m_override_existing_animation	( other.m_override_existing_animation ),
	m_additivity_priority			( other.m_additivity_priority ),
	m_is_transitting_to_zero		( other.m_is_transitting_to_zero ),
	m_animation_state				( 0 ),
	m_next_animation				( 0 ),
	m_synchronization_group_id		( other.m_synchronization_group_id )
{
#ifndef MASTER_GOLD
	user_data						= other.user_data;
#endif // #ifndef MASTER_GOLD
}

inline animation_state&	n_ary_tree_animation_node::animation_state				( ) const
{
	R_ASSERT						( m_animation_state );
	return							*m_animation_state;
}

#ifndef MASTER_GOLD
inline pcstr n_ary_tree_animation_node::identifier								( ) const
{
	R_ASSERT						( m_identifier );
	return							m_identifier;
}
#endif // #ifndef MASTER_GOLD

inline void n_ary_tree_animation_node::set_animation_state						( mixing::animation_state& animation_state )
{
	m_animation_state				= &animation_state;
}

inline void n_ary_tree_animation_node::assign_uninitialized_user_data			( )
{
#ifndef MASTER_GOLD
	user_data						= memory::uninitialized_value<u32>( );
#endif // #ifndef MASTER_GOLD
}

inline base_interpolator const& n_ary_tree_animation_node::interpolator			( ) const
{
	return							m_interpolator;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef N_ARY_TREE_ANIMATION_NODE_INLINE_H_INCLUDED