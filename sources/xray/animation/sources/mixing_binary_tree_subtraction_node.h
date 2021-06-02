////////////////////////////////////////////////////////////////////////////
//	Created		: 25.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BINARY_TREE_SUBTRACTION_NODE_H_INCLUDED
#define BINARY_TREE_SUBTRACTION_NODE_H_INCLUDED

#include "mixing_binary_tree_binary_operation_node.h"

namespace xray {
namespace animation {
namespace mixing {

class binary_tree_subtraction_node : public binary_tree_binary_operation_node {
public:
	inline			binary_tree_subtraction_node(
						binary_tree_base_node* const left,
						binary_tree_base_node* const right
					);

private:
	typedef binary_tree_binary_operation_node	super;

private:
	virtual	void	accept						( binary_tree_visitor& visitor );
}; // class binary_tree_subtraction_node

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_binary_tree_subtraction_node_inline.h"

#endif // #ifndef BINARY_TREE_SUBTRACTION_NODE_H_INCLUDED