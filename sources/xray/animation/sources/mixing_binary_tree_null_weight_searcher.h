////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_BINARY_TREE_NULL_WEIGHT_SEARCHER_H_INCLUDED
#define MIXING_BINARY_TREE_NULL_WEIGHT_SEARCHER_H_INCLUDED

#include <xray/animation/mixing_binary_tree_visitor.h>

namespace xray {
namespace animation {
namespace mixing {

class binary_tree_null_weight_searcher : public binary_tree_visitor {
public:
					binary_tree_null_weight_searcher( );
	inline	bool				result				( ) const { return m_result; }

private:
					binary_tree_null_weight_searcher( binary_tree_null_weight_searcher const& other );
		binary_tree_null_weight_searcher& operator =( binary_tree_null_weight_searcher const& other );

private:
	virtual	void				visit				( binary_tree_animation_node& node );
	virtual	void				visit				( binary_tree_weight_node& node );
	virtual	void				visit				( binary_tree_addition_node& node );
	virtual	void				visit				( binary_tree_subtraction_node& node );
	virtual	void				visit				( binary_tree_multiplication_node& node );

private:
	bool	m_result;
}; // class binary_tree_null_weight_searcher

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_BINARY_TREE_NULL_WEIGHT_SEARCHER_H_INCLUDED