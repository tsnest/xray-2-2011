////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_BASE_NODE_INLINE_H_INCLUDED
#define MIXING_N_ARY_TREE_BASE_NODE_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline n_ary_tree_base_node::n_ary_tree_base_node	( )
{
#ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
	threading::interlocked_increment( g_n_ary_tree_stats.constructors_count );
	threading::interlocked_increment( g_n_ary_tree_stats.difference );
#endif // #ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
}

#ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
inline n_ary_tree_base_node::n_ary_tree_base_node	( n_ary_tree_base_node const& )
{
	threading::interlocked_increment( g_n_ary_tree_stats.constructors_count );
	threading::interlocked_increment( g_n_ary_tree_stats.difference );
}
#endif // #ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS

inline n_ary_tree_base_node::~n_ary_tree_base_node	( )
{
#ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
	threading::interlocked_increment( g_n_ary_tree_stats.destructors_count );
	R_ASSERT_CMP					( g_n_ary_tree_stats.difference, >, 0 );
	threading::interlocked_decrement( g_n_ary_tree_stats.difference );
#endif // #ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_BASE_NODE_INLINE_H_INCLUDED