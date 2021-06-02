////////////////////////////////////////////////////////////////////////////
//	Created		: 24.09.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_TIME_SCALE_NODE_H_INCLUDED
#define MIXING_N_ARY_TREE_TIME_SCALE_NODE_H_INCLUDED

#include "mixing_n_ary_tree_base_node.h"
#include <xray/animation/base_interpolator.h>

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_time_scale_node : public n_ary_tree_base_node {
public:
	inline			n_ary_tree_time_scale_node			( base_interpolator const& interpolator, float const time_scale, float animation_time_before_scale_starts, u32 time_scale_start_time_in_ms );
	inline			n_ary_tree_time_scale_node			( n_ary_tree_time_scale_node const& other );
	inline	base_interpolator const& interpolator		( ) const;
	inline	float	time_scale							( ) const;
	inline	u32		time_scale_start_time_in_ms				( ) const;
	inline	float	animation_time_before_scale_starts	( ) const;
	inline	void	set_time_scale_start_time				( u32 time_scale_start_time_in_ms, float animation_time_before_scale_starts );
	inline	bool	operator ==							( n_ary_tree_time_scale_node const& other ) const;
	inline	bool	operator !=							( n_ary_tree_time_scale_node const& other ) const;

private:
	n_ary_tree_time_scale_node& operator=				( n_ary_tree_time_scale_node const& other);
	virtual	void	accept								( n_ary_tree_visitor& visitor );
	virtual void	accept								( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_base_node& node );
	virtual void	visit								( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_animation_node& node );
	virtual void	visit								( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_weight_node& node );
	virtual void	visit								( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_time_scale_node& node );
	virtual void	visit								( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_addition_node& node );
	virtual void	visit								( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_subtraction_node& node );
	virtual void	visit								( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_multiplication_node& node );
	virtual void	visit								( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_weight_transition_node& node );
	virtual void	visit								( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_time_scale_transition_node& node );
	virtual	bool	is_time_scale						( ) { return true; }

private:
	base_interpolator const&	m_interpolator;
	float const					m_time_scale;
	float						m_animation_time_before_scale_starts;
	u32							m_time_scale_start_time_in_ms;

}; // class n_ary_tree_time_scale_node

	inline	bool	operator <						(
						n_ary_tree_time_scale_node const& left,
						n_ary_tree_time_scale_node const& right
					);

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_n_ary_tree_time_scale_node_inline.h"

#endif // #ifndef MIXING_N_ARY_TREE_TIME_SCALE_NODE_H_INCLUDED