////////////////////////////////////////////////////////////////////////////
//	Created		: 05.10.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_TARGET_TIME_SCALE_CALCULATOR_H_INCLUDED
#define MIXING_N_ARY_TREE_TARGET_TIME_SCALE_CALCULATOR_H_INCLUDED

#include "mixing_n_ary_tree_visitor.h"

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_target_time_scale_calculator :
	public n_ary_tree_visitor,
	private boost::noncopyable
{
public:
	n_ary_tree_target_time_scale_calculator	( n_ary_tree_animation_node& node );
	inline	float	result					( ) const { return m_result; }

private:
	virtual	void	visit					( n_ary_tree_animation_node& node );
	virtual	void	visit					( n_ary_tree_weight_transition_node& node );
	virtual	void	visit					( n_ary_tree_time_scale_transition_node& node );
	virtual	void	visit					( n_ary_tree_weight_node& node );
	virtual	void	visit					( n_ary_tree_time_scale_node& node );
	virtual	void	visit					( n_ary_tree_addition_node& node );
	virtual	void	visit					( n_ary_tree_subtraction_node& node );
	virtual	void	visit					( n_ary_tree_multiplication_node& node );

private:
	float			m_result;
}; // struct n_ary_tree_target_time_scale_calculator

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_TARGET_TIME_SCALE_CALCULATOR_H_INCLUDED