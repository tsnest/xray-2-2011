////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_WEIGHT_NODE_H_INCLUDED
#define N_ARY_TREE_WEIGHT_NODE_H_INCLUDED

#include "mixing_n_ary_tree_base_node.h"
#include <xray/animation/base_interpolator.h>

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_weight_node : public n_ary_tree_base_node {
public:
	inline			n_ary_tree_weight_node			( base_interpolator const& interpolator, float const weight );
	inline			n_ary_tree_weight_node			( n_ary_tree_weight_node const& other );
	inline	base_interpolator const& interpolator	( ) const;
	inline	float	weight							( ) const;
	inline	bool	operator ==						( n_ary_tree_weight_node const& other ) const;
	inline	bool	operator !=						( n_ary_tree_weight_node const& other ) const;

private:
			n_ary_tree_weight_node& operator=		( n_ary_tree_weight_node const& other);

private:
	virtual	bool	is_weight						( ) { return true; }

private:
	virtual	void	accept							( n_ary_tree_visitor& visitor );
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
	base_interpolator const&	m_interpolator;
	float const					m_weight;
}; // class n_ary_tree_weight_node

			bool	operator <						(
						n_ary_tree_weight_node const& left,
						n_ary_tree_weight_node const& right
					);

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_n_ary_tree_weight_node_inline.h"

#endif // #ifndef N_ARY_TREE_WEIGHT_NODE_H_INCLUDED