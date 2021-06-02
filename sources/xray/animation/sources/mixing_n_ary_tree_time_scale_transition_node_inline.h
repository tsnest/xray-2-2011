////////////////////////////////////////////////////////////////////////////
//	Created		: 28.09.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_TIME_SCALE_TRANSITION_NODE_INLINE_H_INCLUDED
#define MIXING_N_ARY_TREE_TIME_SCALE_TRANSITION_NODE_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline n_ary_tree_time_scale_transition_node::n_ary_tree_time_scale_transition_node	(
		n_ary_tree_base_node& from,
		n_ary_tree_base_node& to,
		base_interpolator const& interpolator,
		u32 const current_time_in_ms,
		u32 const last_integration_interval_id,
		float const animation_time_on_transition_start
	) :
	super										( from, to, interpolator, current_time_in_ms ),
	m_last_integration_interval_value			( animation_time_on_transition_start ),
	m_last_integration_interval_id				( last_integration_interval_id ),
	m_last_integration_interval_cycles_count	( 0 )
{
	R_ASSERT_CMP								( m_last_integration_interval_value, >=, 0.f );
}

inline float n_ary_tree_time_scale_transition_node::last_integration_interval_value				( ) const
{
	return										m_last_integration_interval_value;
}

inline void	n_ary_tree_time_scale_transition_node::set_last_integration_interval_value			( float const last_integration_interval_value )
{
	R_ASSERT_CMP								( last_integration_interval_value, >=, 0.f );
	m_last_integration_interval_value			= last_integration_interval_value;
}

inline u32 n_ary_tree_time_scale_transition_node::last_integration_interval_id					( ) const
{
	return										m_last_integration_interval_id;
}

inline void	n_ary_tree_time_scale_transition_node::set_last_integration_interval_id				( u32 const last_integration_interval_id )
{
	m_last_integration_interval_id				= last_integration_interval_id;
}

inline void	n_ary_tree_time_scale_transition_node::set_last_integration_interval_cycles_count	( int const last_integration_interval_cycles_count )
{
	m_last_integration_interval_cycles_count	= last_integration_interval_cycles_count;
}

inline int	n_ary_tree_time_scale_transition_node::last_integration_interval_cycles_count		( ) const
{
	return										m_last_integration_interval_cycles_count;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_TIME_SCALE_TRANSITION_NODE_INLINE_H_INCLUDED