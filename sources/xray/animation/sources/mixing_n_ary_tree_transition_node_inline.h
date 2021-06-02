////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_TRANSITION_NODE_INLINE_H_INCLUDED
#define MIXING_N_ARY_TREE_TRANSITION_NODE_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline n_ary_tree_transition_node::n_ary_tree_transition_node			(
		n_ary_tree_base_node& from,
		n_ary_tree_base_node& to,
		base_interpolator const& interpolator,
		u32 const current_time_in_ms
	) :
	m_from			( from ),
	m_to			( to ),
	m_interpolator	( interpolator ),
	m_start_time	( current_time_in_ms )
{
}

inline n_ary_tree_base_node& n_ary_tree_transition_node::from			( ) const
{
	return			m_from;
}

inline n_ary_tree_base_node& n_ary_tree_transition_node::to				( ) const
{
	return			m_to;
}

inline base_interpolator const& n_ary_tree_transition_node::interpolator( ) const
{
	return			m_interpolator;
}

inline u32 n_ary_tree_transition_node::start_time						( ) const
{
	return			m_start_time;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_TRANSITION_NODE_INLINE_H_INCLUDED