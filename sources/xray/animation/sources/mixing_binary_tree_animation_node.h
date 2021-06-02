////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BINARY_TREE_ANIMATION_NODE_H_INCLUDED
#define BINARY_TREE_ANIMATION_NODE_H_INCLUDED

#include "mixing_binary_tree_base_node.h"

namespace xray {
namespace animation {
namespace mixing {

class animation_clip;
class binary_tree_weight_node;

class binary_tree_animation_node;
typedef intrusive_ptr<
			binary_tree_animation_node,
			binary_tree_base_node,
			threading::single_threading_policy
		> binary_tree_animation_node_ptr;

class binary_tree_animation_node : public binary_tree_base_node {
public:
	inline					binary_tree_animation_node	( animation_clip* const animation, float const time_scale );
	inline					binary_tree_animation_node	( binary_tree_animation_node const& other );
	inline	animation_clip const*	animation			( ) const;
	inline	float	time_scale							( ) const;
	virtual	void	accept								( binary_tree_visitor& visitor );

private:
	virtual	base_interpolator const*	interpolator	( ) const;
	inline	binary_tree_animation_node& operator=		( binary_tree_animation_node const& other );

private:
	animation_clip const*	m_animation;

public:
	binary_tree_animation_node_ptr	m_next_animation;
	u32						m_unique_weights_count;

private:
	float const				m_time_scale;
}; // class binary_tree_animation_node

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_binary_tree_animation_node_inline.h"

#endif // #ifndef BINARY_TREE_ANIMATION_NODE_H_INCLUDED