////////////////////////////////////////////////////////////////////////////
//	Created		: 29.09.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_ANIMATION_TIME_CALCULATOR_H_INCLUDED
#define MIXING_N_ARY_TREE_ANIMATION_TIME_CALCULATOR_H_INCLUDED

#include "mixing_n_ary_tree_visitor.h"

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_animation_node;
struct n_ary_tree_base_node;

class n_ary_tree_animation_time_calculator :
	public n_ary_tree_visitor,
	private boost::noncopyable
{
public:
					n_ary_tree_animation_time_calculator	(
						n_ary_tree_animation_node& animation,
						u32 const start_time_in_ms,
						float const start_animation_time,
						u32 const target_time_in_ms,
						bool const is_read_only
					);
	inline	float	animation_time					( ) const { return m_animation_time; }

private:
	virtual	void	visit							( n_ary_tree_animation_node& node );
	virtual	void	visit							( n_ary_tree_weight_transition_node& node );
	virtual	void	visit							( n_ary_tree_time_scale_transition_node& node );
	virtual	void	visit							( n_ary_tree_weight_node& node );
	virtual	void	visit							( n_ary_tree_time_scale_node& node );
	virtual	void	visit							( n_ary_tree_addition_node& node );
	virtual	void	visit							( n_ary_tree_subtraction_node& node );
	virtual	void	visit							( n_ary_tree_multiplication_node& node );

private:
	n_ary_tree_animation_node& m_animation;
	n_ary_tree_base_node* m_new_time_scale_node;
	u32	const		m_start_time_in_ms;
	float const		m_start_animation_time;
	u32 const		m_target_time_in_ms;
	float			m_animation_time;
	float const		m_animation_interval_length;
	bool const		m_is_read_only;
}; // class n_ary_tree_animation_time_calculator

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_ANIMATION_TIME_CALCULATOR_H_INCLUDED