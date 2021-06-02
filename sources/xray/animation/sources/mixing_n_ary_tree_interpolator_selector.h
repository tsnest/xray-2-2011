////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_INTERPOLATOR_SELECTOR_H_INCLUDED
#define N_ARY_TREE_INTERPOLATOR_SELECTOR_H_INCLUDED

#include "mixing_n_ary_tree_visitor.h"

namespace xray {
namespace animation {

struct base_interpolator;

namespace mixing {

class n_ary_tree_interpolator_selector : public n_ary_tree_visitor {
public:
	inline	n_ary_tree_interpolator_selector	( base_interpolator const* const result = 0 ) : m_result(result) { }
	inline	base_interpolator const* result		( ) const { return m_result; }

public:
	virtual	void	visit						( n_ary_tree_animation_node& node );
	virtual	void	visit						( n_ary_tree_weight_transition_node& node );
	virtual	void	visit						( n_ary_tree_time_scale_transition_node& node );
	virtual	void	visit						( n_ary_tree_weight_node& node );
	virtual	void	visit						( n_ary_tree_time_scale_node& node );
	virtual	void	visit						( n_ary_tree_addition_node& node );
	virtual	void	visit						( n_ary_tree_subtraction_node& node );
	virtual	void	visit						( n_ary_tree_multiplication_node& node );

private:
	base_interpolator const*	m_result;
}; // class n_ary_tree_interpolator_selector

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef N_ARY_TREE_INTERPOLATOR_SELECTOR_H_INCLUDED