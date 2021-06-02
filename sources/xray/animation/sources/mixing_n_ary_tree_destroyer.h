////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_DESTROYER_H_INCLUDED
#define N_ARY_TREE_DESTROYER_H_INCLUDED

#include "mixing_n_ary_tree_visitor.h"

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_n_ary_operation_node;

class n_ary_tree_destroyer : public n_ary_tree_visitor {
public:
		n_ary_tree_destroyer	( ) { }
	
private:
	virtual	void	visit		( n_ary_tree_animation_node& node );
	virtual	void	visit		( n_ary_tree_weight_node& node );
	virtual	void	visit		( n_ary_tree_time_scale_node& node );
	virtual	void	visit		( n_ary_tree_addition_node& node );
	virtual	void	visit		( n_ary_tree_subtraction_node& node );
	virtual	void	visit		( n_ary_tree_multiplication_node& node );
	virtual	void	visit		( n_ary_tree_weight_transition_node& node );
	virtual	void	visit		( n_ary_tree_time_scale_transition_node& node );
	template < typename T >
	inline	void	propagate	( T& node );
}; // class n_ary_tree_destroyer

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef N_ARY_TREE_DESTROYER_H_INCLUDED