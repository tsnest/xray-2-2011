////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_SIZE_CALCULATOR_H_INCLUDED
#define N_ARY_TREE_SIZE_CALCULATOR_H_INCLUDED

#include <xray/animation/mixing_binary_tree_visitor.h>
#include "mixing_n_ary_tree_visitor.h"

namespace xray {
namespace animation {
namespace mixing {

class animation_interval;
class binary_tree_binary_operation_node;

class n_ary_tree_size_calculator :
	public binary_tree_visitor,
	public n_ary_tree_visitor
{
public:
		n_ary_tree_size_calculator	( );
	inline	u32		calculated_size	( ) const { return m_size; }

private:
	virtual	void	visit			( binary_tree_animation_node& node );
	virtual	void	visit			( binary_tree_weight_node& node );
	virtual	void	visit			( binary_tree_addition_node& node );
	virtual	void	visit			( binary_tree_subtraction_node& node );
	virtual	void	visit			( binary_tree_multiplication_node& node );

public:
	virtual	void	visit			( n_ary_tree_animation_node& node );
	virtual	void	visit			( n_ary_tree_weight_transition_node& node );
	virtual	void	visit			( n_ary_tree_time_scale_transition_node& node );
	virtual	void	visit			( n_ary_tree_weight_node& node );
	virtual	void	visit			( n_ary_tree_time_scale_node& node );
	virtual	void	visit			( n_ary_tree_addition_node& node );
	virtual	void	visit			( n_ary_tree_subtraction_node& node );
	virtual	void	visit			( n_ary_tree_multiplication_node& node );

private:
			void	propagate		( binary_tree_binary_operation_node& node );
	template < typename T >
	inline	void	propagate		( T& node );

private:
	u32				m_size;
}; // class n_ary_tree_size_calculator

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef N_ARY_TREE_SIZE_CALCULATOR_H_INCLUDED