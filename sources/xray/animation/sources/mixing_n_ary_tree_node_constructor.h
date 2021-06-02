////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_NODE_CONSTRUCTOR_H_INCLUDED
#define N_ARY_TREE_NODE_CONSTRUCTOR_H_INCLUDED

#include <xray/animation/mixing_binary_tree_visitor.h>

namespace xray {
namespace animation {

struct base_interpolator;

namespace mixing {

struct n_ary_tree_base_node;
class binary_tree_binary_operation_node;

class n_ary_tree_node_constructor : public binary_tree_visitor {
public:
				n_ary_tree_node_constructor	(
						mutable_buffer&	buffer,
						base_interpolator const* const* const interpolators_begin,
						base_interpolator const* const*	const interpolators_end
					);
	inline	n_ary_tree_base_node* result	( ) const;

private:
	n_ary_tree_node_constructor				( n_ary_tree_node_constructor const& other );
	n_ary_tree_node_constructor& operator =	( n_ary_tree_node_constructor const& other );

private:
	virtual	void	visit					( binary_tree_animation_node& node );
	virtual	void	visit					( binary_tree_weight_node& node );
	virtual	void	visit					( binary_tree_addition_node& node );
	virtual	void	visit					( binary_tree_subtraction_node& node );
	virtual	void	visit					( binary_tree_multiplication_node& node );
			void	propagate				( binary_tree_binary_operation_node& node );

private:
	mutable_buffer&							m_buffer;
	n_ary_tree_base_node*					m_result;
	base_interpolator const* const* const	m_interpolators_begin;
	base_interpolator const* const*	const	m_interpolators_end;

}; // class n_ary_tree_node_constructor

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_n_ary_tree_node_constructor_inline.h"

#endif // #ifndef N_ARY_TREE_NODE_CONSTRUCTOR_H_INCLUDED