////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_VISITOR_H_INCLUDED
#define N_ARY_TREE_VISITOR_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_animation_node;
class n_ary_tree_weight_node;
class n_ary_tree_time_scale_node;
class n_ary_tree_addition_node;
class n_ary_tree_subtraction_node;
class n_ary_tree_multiplication_node;
class n_ary_tree_weight_transition_node;
class n_ary_tree_time_scale_transition_node;

struct XRAY_NOVTABLE n_ary_tree_visitor {
	virtual	void	visit		( n_ary_tree_animation_node& node ) = 0;
	virtual	void	visit		( n_ary_tree_weight_transition_node& node ) = 0;
	virtual	void	visit		( n_ary_tree_time_scale_transition_node& node ) = 0;
	virtual	void	visit		( n_ary_tree_weight_node& node ) = 0;
	virtual	void	visit		( n_ary_tree_time_scale_node& node ) = 0;
	virtual	void	visit		( n_ary_tree_addition_node& node ) = 0;
	virtual	void	visit		( n_ary_tree_subtraction_node& node ) = 0;
	virtual	void	visit		( n_ary_tree_multiplication_node& node ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( n_ary_tree_visitor )
}; // class n_ary_tree_visitor

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef N_ARY_TREE_VISITOR_H_INCLUDED