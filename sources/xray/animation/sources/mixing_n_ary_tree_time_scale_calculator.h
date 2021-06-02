////////////////////////////////////////////////////////////////////////////
//	Created		: 27.09.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_TIME_SCALE_CALCULATOR_H_INCLUDED
#define MIXING_N_ARY_TREE_TIME_SCALE_CALCULATOR_H_INCLUDED

#include "mixing_n_ary_tree_visitor.h"
#include "mixing_n_ary_tree_animation_node.h"

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_time_scale_calculator :
	private boost::noncopyable,
	public n_ary_tree_visitor
{
public:
	enum transitions_destroying_enum {
		forbid_transitions_destroying,
		permit_transitions_destroying,
	}; // enum transitions_destroying_enum

public:
					n_ary_tree_time_scale_calculator(
						u32 current_time_in_ms,
						float previous_animation_time,
						u32 previous_time_in_ms,
						transitions_destroying_enum transitions_destroying,
						n_ary_tree_animation_node const* animation = 0
					);
	inline	float	time_scale						( ) const;
	inline	bool	mixed_time_direction_nodes_found( ) const { return m_forward_time_scale_nodes_count && m_backward_time_scale_nodes_count; }
	inline	n_ary_tree_base_node*	result			( ) const;
	inline	base_interpolator const* interpolator	( ) const;

	virtual	void	visit							( n_ary_tree_animation_node& node );
	virtual	void	visit							( n_ary_tree_weight_transition_node& node );
	virtual	void	visit							( n_ary_tree_time_scale_transition_node& node );
	virtual	void	visit							( n_ary_tree_weight_node& node );
	virtual	void	visit							( n_ary_tree_time_scale_node& node );
	virtual	void	visit							( n_ary_tree_addition_node& node );
	virtual	void	visit							( n_ary_tree_subtraction_node& node );
	virtual	void	visit							( n_ary_tree_multiplication_node& node );

private:
	n_ary_tree_animation_node const* const	m_animation;
	n_ary_tree_base_node*				m_result;
	base_interpolator const*			m_interpolator;
	u32 const							m_current_time_in_ms;
	float const							m_previous_animation_time;
	u32 const							m_previous_time_in_ms;
	float								m_time_scale;
	transitions_destroying_enum const	m_transitions_destroying;
	u32									m_forward_time_scale_nodes_count;
	u32									m_backward_time_scale_nodes_count;
}; // class n_ary_tree_time_scale_calculator

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_n_ary_tree_time_scale_calculator_inline.h"

#endif // #ifndef MIXING_N_ARY_TREE_TIME_SCALE_CALCULATOR_H_INCLUDED