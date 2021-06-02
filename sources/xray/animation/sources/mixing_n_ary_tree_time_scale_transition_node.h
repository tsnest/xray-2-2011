////////////////////////////////////////////////////////////////////////////
//	Created		: 28.09.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_TIME_SCALE_TRANSITION_NODE_H_INCLUDED
#define MIXING_N_ARY_TREE_TIME_SCALE_TRANSITION_NODE_H_INCLUDED

#include "mixing_n_ary_tree_weight_transition_node.h"

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_time_scale_transition_node : public n_ary_tree_weight_transition_node {
public:
	inline	n_ary_tree_time_scale_transition_node				(
						n_ary_tree_base_node& from,
						n_ary_tree_base_node& to,
						base_interpolator const& interpolator,
						u32 const current_time_in_ms,
						u32 const last_integration_interval_id,
						float const animation_time_on_transition_start
					);

	inline	float	last_integration_interval_value				( ) const;
	inline	void	set_last_integration_interval_value			( float const last_integration_interval_value );

	inline	u32		last_integration_interval_id				( ) const;
	inline	void	set_last_integration_interval_id			( u32 const last_integration_interval_id );

	inline	void	set_last_integration_interval_cycles_count	( int const last_integration_interval_cycles_count );
	inline	int		last_integration_interval_cycles_count		( ) const;

	virtual	void	accept										( n_ary_tree_visitor& visitor );

private:
	n_ary_tree_time_scale_transition_node						( n_ary_tree_weight_transition_node const& other);
	n_ary_tree_time_scale_transition_node& operator=			( n_ary_tree_weight_transition_node const& other);

private:
	virtual	bool	is_time_scale								( ) { return true; }
	virtual	bool	is_weight									( ) { return false; }

	virtual void	accept										( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_base_node& node );

	virtual void	visit										( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_animation_node& node );
	virtual void	visit										( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_weight_node& node );
	virtual void	visit										( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_time_scale_node& node );
	virtual void	visit										( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_addition_node& node );
	virtual void	visit										( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_subtraction_node& node );
	virtual void	visit										( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_multiplication_node& node );
	virtual void	visit										( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_weight_transition_node& node );
	virtual void	visit										( n_ary_tree_double_dispatcher& dispatcher, n_ary_tree_time_scale_transition_node& node );

private:
	typedef n_ary_tree_weight_transition_node	super;

private:
	float			m_last_integration_interval_value;
	u32				m_last_integration_interval_id;
	int				m_last_integration_interval_cycles_count;
}; // class n_ary_tree_time_scale_transition_node

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_n_ary_tree_time_scale_transition_node_inline.h"

#endif // #ifndef MIXING_N_ARY_TREE_TIME_SCALE_TRANSITION_NODE_H_INCLUDED