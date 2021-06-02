////////////////////////////////////////////////////////////////////////////
//	Created		: 26.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BINARY_TREE_BINARY_OPERATION_NODE_H_INCLUDED
#define BINARY_TREE_BINARY_OPERATION_NODE_H_INCLUDED

#include "mixing_binary_tree_base_node.h"

namespace xray {
namespace animation {
namespace mixing {

class binary_tree_binary_operation_node : public binary_tree_base_node {
public:
	inline	binary_tree_binary_operation_node	(
									binary_tree_base_node* const left,
									binary_tree_base_node* const right
								);
	inline	binary_tree_base_node& left			( ) const;
	inline	binary_tree_base_node& right		( ) const;

private:
	binary_tree_base_node_ptr	m_left;
	binary_tree_base_node_ptr	m_right;
}; // class binary_tree_binary_operation_node

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_binary_tree_binary_operation_node_inline.h"

#endif // #ifndef BINARY_TREE_BINARY_OPERATION_NODE_H_INCLUDED