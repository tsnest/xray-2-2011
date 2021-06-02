////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BINARY_TREE_BASE_NODE_INLINE_H_INCLUDED
#define BINARY_TREE_BASE_NODE_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline binary_tree_base_node::binary_tree_base_node	( )	:
	m_next_weight				( 0 ),
	m_same_weight				( 0 ),
	m_next_unique_interpolator	( 0 ),
	m_reference_count			( 0 )
{
#ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
	threading::interlocked_increment( g_binary_tree_stats.constructors_count );
	threading::interlocked_increment( g_binary_tree_stats.difference );
#endif // #ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
}

#ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
inline binary_tree_base_node::binary_tree_base_node	( binary_tree_base_node const& other ) :
	m_next_weight				( other.m_next_weight ),
	m_same_weight				( other.m_same_weight ),
	m_next_unique_interpolator	( other.m_next_unique_interpolator ),
	m_reference_count			( other.m_reference_count )
{
	threading::interlocked_increment( g_binary_tree_stats.constructors_count );
	threading::interlocked_increment( g_binary_tree_stats.difference );
}
#endif // #ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS

inline binary_tree_base_node::~binary_tree_base_node( )
{
#ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
	threading::interlocked_increment( g_binary_tree_stats.destructors_count );
	R_ASSERT_CMP					( g_binary_tree_stats.difference, >, 0 );
	threading::interlocked_decrement( g_binary_tree_stats.difference );
#endif // #ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
}

template < typename T >
inline void binary_tree_base_node::destroy	( T* object )
{
	object->~T					( );
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef BINARY_TREE_BASE_NODE_INLINE_H_INCLUDED