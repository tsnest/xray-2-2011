////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_BINARY_TREE_ANIMATION_NODE_INLINE_H_INCLUDED
#define MIXING_BINARY_TREE_ANIMATION_NODE_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline binary_tree_animation_node::binary_tree_animation_node			( binary_tree_animation_node const& other ) :
	m_buffer							( other.m_buffer ),
#ifndef MASTER_GOLD
	m_identifier						( other.m_identifier ),
#endif // #ifndef MASTER_GOLD
	m_animation_intervals				( other.m_animation_intervals ),
	m_animation_intervals_count			( other.m_animation_intervals_count ),
	m_start_animation_interval_id		( other.m_start_animation_interval_id ),
	m_start_animation_interval_time		( other.m_start_animation_interval_time ),
	m_start_cycle_animation_interval_id	( other.m_start_cycle_animation_interval_id ),
	m_next_animation					( other.m_next_animation ),
	m_driving_animation					( other.m_driving_animation ),
	m_n_ary_driving_animation			( other.m_n_ary_driving_animation ),
	m_unique_weights_count				( other.m_unique_weights_count ),
	m_weight_interpolator				( other.m_weight_interpolator ),
	m_time_scale						( other.m_time_scale ),
	m_time_scale_interpolator			( other.m_time_scale_interpolator ),
	m_playing_type						( other.m_playing_type ),
	m_null_weight_found					( other.m_null_weight_found ),
	m_synchronization_group_id			( other.synchronization_group_id() ),
	m_override_existing_animation		( other.m_override_existing_animation ),
	m_additivity_priority				( other.m_additivity_priority )
{
#ifndef MASTER_GOLD
	user_data							= other.user_data;
#endif // #ifndef MASTER_GOLD
}

inline float binary_tree_animation_node::time_scale						( ) const
{
	return								m_time_scale;
}

inline playing_type_enum binary_tree_animation_node::playing_type		( ) const
{
	return								m_playing_type;
}

inline u32 binary_tree_animation_node::synchronization_group_id			( ) const
{
	return								m_synchronization_group_id;
}

inline u32 binary_tree_animation_node::additivity_priority				( ) const
{
	return								m_additivity_priority;
}

inline binary_tree_animation_node_ptr binary_tree_animation_node::driving_animation		( ) const
{
	return								m_driving_animation;
}

inline n_ary_tree_animation_node* binary_tree_animation_node::n_ary_driving_animation	( ) const
{
	return								m_n_ary_driving_animation;
}

inline void binary_tree_animation_node::set_n_ary_driving_animation		( n_ary_tree_animation_node* animation )
{
	// this is possible when single lexeme is used in several set_target function calls 
//	R_ASSERT							( !m_n_ary_driving_animation );
	R_ASSERT							( animation );
	m_n_ary_driving_animation			= animation;
}

inline void binary_tree_animation_node::set_driving_animation			( binary_tree_animation_node* driving_animation )
{
	// this is possible when single lexeme is used in several set_target function calls 
//	R_ASSERT							( !m_driving_animation );
	R_ASSERT							( driving_animation );
	m_driving_animation					= driving_animation;
}

inline void binary_tree_animation_node::assign_uninitialized_user_data	( )
{
#ifndef MASTER_GOLD
	user_data							= xray::memory::uninitialized_value<u32>( );
#endif // #ifndef MASTER_GOLD
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_BINARY_TREE_ANIMATION_NODE_INLINE_H_INCLUDED