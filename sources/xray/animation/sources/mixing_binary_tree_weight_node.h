////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BINARY_TREE_WEIGHT_NODE_H_INCLUDED
#define BINARY_TREE_WEIGHT_NODE_H_INCLUDED

#include "mixing_binary_tree_base_node.h"

namespace xray {
namespace animation {

struct base_interpolator;

namespace mixing {

class binary_tree_weight_node : public binary_tree_base_node {
public:
	inline						binary_tree_weight_node	( float const weight, base_interpolator const* const interpolator );
	inline	base_interpolator const& interpolator_impl	( ) const;
	inline	float	weight								( ) const;
	virtual	binary_tree_weight_node* cast_weight		( ) { return this; }

protected:
	inline	void				set_weight				( float const new_weight );

private:
	virtual	void				accept					( binary_tree_visitor& visitor );
	virtual	base_interpolator const* interpolator		( ) const { return &interpolator_impl(); }

private:
	base_interpolator const*	m_interpolator;
	float						m_weight;

public:
	float						m_simplified_weight;
}; // class binary_tree_weight_node

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_binary_tree_weight_node_inline.h"

#endif // #ifndef BINARY_TREE_WEIGHT_NODE_H_INCLUDED