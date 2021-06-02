////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BINARY_TREE_WEIGHT_NODE_H_INCLUDED
#define BINARY_TREE_WEIGHT_NODE_H_INCLUDED

#include <xray/animation/mixing_binary_tree_base_node.h>

namespace xray {
namespace animation {

struct base_interpolator;

namespace mixing {

class XRAY_ANIMATION_API binary_tree_weight_node : public binary_tree_base_node {
public:
	inline						binary_tree_weight_node	( float const weight, base_interpolator const* const interpolator );
	inline	base_interpolator const& interpolator		( ) const;
	inline	float				weight					( ) const;
	virtual	binary_tree_weight_node* cast_weight		( ) { return this; }

protected:
	inline	void				set_weight				( float const new_weight );

private:
	virtual	void				accept					( binary_tree_visitor& visitor );

private:
	base_interpolator const*	m_interpolator;
	float						m_weight;

public:
	float						m_simplified_weight;
}; // class binary_tree_weight_node

typedef intrusive_ptr<
	binary_tree_weight_node,
	binary_tree_base_node,
	threading::single_threading_policy
> binary_tree_weight_node_ptr;

} // namespace mixing
} // namespace animation
} // namespace xray

#include <xray/animation/mixing_binary_tree_weight_node_inline.h>

#endif // #ifndef BINARY_TREE_WEIGHT_NODE_H_INCLUDED