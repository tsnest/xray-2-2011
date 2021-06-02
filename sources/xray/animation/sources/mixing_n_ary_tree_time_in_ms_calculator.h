////////////////////////////////////////////////////////////////////////////
//	Created		: 23.09.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_TIME_IN_MS_CALCULATOR_H_INCLUDED
#define MIXING_N_ARY_TREE_TIME_IN_MS_CALCULATOR_H_INCLUDED

#include "mixing_n_ary_tree_visitor.h"

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_time_in_ms_calculator :
	public n_ary_tree_visitor,
	private boost::noncopyable
{
public:
					n_ary_tree_time_in_ms_calculator(
						n_ary_tree_animation_node& animation,
						u32 start_time_in_ms,
						float start_animation_time,
						float target_animation_time,
						u32 event_type
					);
	inline	u32		time_in_ms						( ) const { return m_time_in_ms; }
	inline	u32		event_type						( ) const { return m_event_type; }
	inline	float	event_time						( ) const { return m_event_time; }

public:
	virtual	void	visit							( n_ary_tree_animation_node& node );
	virtual	void	visit							( n_ary_tree_weight_transition_node& node );
	virtual	void	visit							( n_ary_tree_time_scale_transition_node& node );
	virtual	void	visit							( n_ary_tree_weight_node& node );
	virtual	void	visit							( n_ary_tree_time_scale_node& node );
	virtual	void	visit							( n_ary_tree_addition_node& node );
	virtual	void	visit							( n_ary_tree_subtraction_node& node );
	virtual	void	visit							( n_ary_tree_multiplication_node& node );

private:
	float		m_target_animation_time;
	float		m_start_animation_time;
	float		m_event_time;
	u32			m_time_in_ms;
	u32			m_event_type;
	u32	const	m_start_time_in_ms;
}; // class n_ary_tree_time_in_ms_calculator

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_TIME_IN_MS_CALCULATOR_H_INCLUDED