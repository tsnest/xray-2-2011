////////////////////////////////////////////////////////////////////////////
//	Created		: 03.10.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_TIME_SCALE_START_TIME_MODIFIER_H_INCLUDED
#define MIXING_N_ARY_TREE_TIME_SCALE_START_TIME_MODIFIER_H_INCLUDED

#include "mixing_n_ary_tree_visitor.h"

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_animation_node;

class n_ary_tree_time_scale_start_time_modifier :
	public n_ary_tree_visitor,
	private boost::noncopyable
{
public:
	n_ary_tree_time_scale_start_time_modifier	(
						n_ary_tree_animation_node& animation,
						u32 const new_start_time_in_ms,
						float animation_interval_time
					);

private:
	virtual	void	visit						( n_ary_tree_animation_node& node );
	virtual	void	visit						( n_ary_tree_weight_transition_node& node );
	virtual	void	visit						( n_ary_tree_time_scale_transition_node& node );
	virtual	void	visit						( n_ary_tree_weight_node& node );
	virtual	void	visit						( n_ary_tree_time_scale_node& node );
	virtual	void	visit						( n_ary_tree_addition_node& node );
	virtual	void	visit						( n_ary_tree_subtraction_node& node );
	virtual	void	visit						( n_ary_tree_multiplication_node& node );

private:
	u32	const		m_new_start_time_in_ms;
	float const		m_animation_interval_time;
}; // class n_ary_tree_time_scale_start_time_modifier

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_TIME_SCALE_START_TIME_MODIFIER_H_INCLUDED