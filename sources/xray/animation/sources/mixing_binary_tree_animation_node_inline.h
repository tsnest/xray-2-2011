////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BINARY_TREE_ANIMATION_NODE_INLINE_H_INCLUDED
#define BINARY_TREE_ANIMATION_NODE_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline binary_tree_animation_node::binary_tree_animation_node		( animation_clip* const animation, float const time_scale ) :
	m_animation				( animation ),
	m_next_animation		( 0 ),
	m_unique_weights_count	( 0 ),
	m_time_scale			( time_scale )
{
	ASSERT_CMP				( time_scale, >, 0.f );
}

inline binary_tree_animation_node::binary_tree_animation_node		( binary_tree_animation_node const& other ) :
	m_animation				( other.m_animation ),
	m_next_animation		( other.m_next_animation ),
	m_unique_weights_count	( other.m_unique_weights_count ),
	m_time_scale			( other.m_time_scale )
{
}

inline animation_clip const* binary_tree_animation_node::animation	( ) const
{
	ASSERT					( m_animation );
	return					m_animation;
}

inline float binary_tree_animation_node::time_scale					( ) const
{
	return					m_time_scale;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef BINARY_TREE_ANIMATION_NODE_INLINE_H_INCLUDED