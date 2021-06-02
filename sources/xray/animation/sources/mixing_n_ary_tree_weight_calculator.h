////////////////////////////////////////////////////////////////////////////
//	Unit		: n_ary_tree_weight_calculator.h
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_N_ARY_TREE_WEIGHT_CALCULATOR_H_INCLUDED
#define XRAY_ANIMATION_N_ARY_TREE_WEIGHT_CALCULATOR_H_INCLUDED

#include "mixing_n_ary_tree_visitor.h"
#include "mixing_n_ary_tree_animation_node.h"

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_weight_calculator :
	public n_ary_tree_visitor,
	private boost::noncopyable
{
public:
	inline	n_ary_tree_weight_calculator( u32 const current_time_in_ms );
	inline	float	weight				( ) const;
	inline	bool	null_weight_found	( ) const;
	inline	u32		weight_transition_ended_time_in_ms	( ) const;

	virtual	void	visit				( n_ary_tree_animation_node& node );
	virtual	void	visit				( n_ary_tree_weight_transition_node& node );
	virtual	void	visit				( n_ary_tree_time_scale_transition_node& node );
	virtual	void	visit				( n_ary_tree_weight_node& node );
	virtual	void	visit				( n_ary_tree_time_scale_node& node );
	virtual	void	visit				( n_ary_tree_addition_node& node );
	virtual	void	visit				( n_ary_tree_subtraction_node& node );
	virtual	void	visit				( n_ary_tree_multiplication_node& node );

private:
	n_ary_tree_base_node*	m_result;
	u32						m_current_time_in_ms;
	float					m_weight;
	bool					m_null_weight_found;
	u32						m_weight_transition_ended_time_in_ms;
}; // class n_ary_tree_weight_calculator

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_n_ary_tree_weight_calculator_inline.h"

#endif // #ifndef XRAY_ANIMATION_N_ARY_TREE_WEIGHT_CALCULATOR_H_INCLUDED