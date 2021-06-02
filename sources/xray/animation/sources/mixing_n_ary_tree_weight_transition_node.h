////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_WEIGHT_TRANSITION_NODE_H_INCLUDED
#define N_ARY_TREE_WEIGHT_TRANSITION_NODE_H_INCLUDED

#include "mixing_n_ary_tree_base_node.h"
#include <xray/animation/base_interpolator.h>

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_weight_transition_node : public n_ary_tree_base_node {
public:
	inline			n_ary_tree_weight_transition_node		(
						n_ary_tree_base_node& from,
						n_ary_tree_base_node& to,
						base_interpolator const& interpolator,
						u32 const current_time_in_ms
					);

	inline	n_ary_tree_base_node& from				( ) const;
	inline	n_ary_tree_base_node& to				( ) const;
	inline	base_interpolator const& interpolator	( ) const;
	inline	u32		start_time_in_ms				( ) const;

	virtual	void	accept							( n_ary_tree_visitor& visitor );
	inline	void	on_from_changed					( n_ary_tree_base_node& new_from ) { m_from	= &new_from; }

private:
	n_ary_tree_weight_transition_node				( n_ary_tree_weight_transition_node const& other);
	n_ary_tree_weight_transition_node& operator=	( n_ary_tree_weight_transition_node const& other);

private:
	virtual	bool	is_weight						( ) { return true; }

	virtual void	accept							( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_base_node& node );

	virtual void	visit							( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_animation_node& node );
	virtual void	visit							( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_weight_node& node );
	virtual void	visit							( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_time_scale_node& node );
	virtual void	visit							( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_addition_node& node );
	virtual void	visit							( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_subtraction_node& node );
	virtual void	visit							( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_multiplication_node& node );
	virtual void	visit							( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_weight_transition_node& node );
	virtual void	visit							( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_time_scale_transition_node& node );

private:
	friend class n_ary_tree_time_scale_calculator;

private:
	n_ary_tree_base_node*		m_from;
	n_ary_tree_base_node*		m_to;
	base_interpolator const&	m_interpolator;
	u32 const					m_start_time_in_ms;
}; // class n_ary_tree_weight_transition_node

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_n_ary_tree_weight_transition_node_inline.h"

#endif // #ifndef N_ARY_TREE_WEIGHT_TRANSITION_NODE_H_INCLUDED