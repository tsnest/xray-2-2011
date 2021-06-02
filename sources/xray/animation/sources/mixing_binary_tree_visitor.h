////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BINARY_TREE_VISITOR_H_INCLUDED
#define BINARY_TREE_VISITOR_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

class binary_tree_animation_node;
class binary_tree_weight_node;
class binary_tree_addition_node;
class binary_tree_subtraction_node;
class binary_tree_multiplication_node;

struct XRAY_NOVTABLE binary_tree_visitor {
	virtual	void	visit			( binary_tree_animation_node& node ) = 0;
	virtual	void	visit			( binary_tree_weight_node& node ) = 0;
	virtual	void	visit			( binary_tree_addition_node& node ) = 0;
	virtual	void	visit			( binary_tree_subtraction_node& node ) = 0;
	virtual	void	visit			( binary_tree_multiplication_node& node ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( binary_tree_visitor )
}; // struct binary_tree_visitor

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef BINARY_TREE_VISITOR_H_INCLUDED