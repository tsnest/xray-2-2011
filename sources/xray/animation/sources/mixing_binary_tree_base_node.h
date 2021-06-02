////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BINARY_TREE_BASE_NODE_H_INCLUDED
#define BINARY_TREE_BASE_NODE_H_INCLUDED

#include "mixing_debug.h"

namespace xray {
namespace animation {

struct base_interpolator;

namespace mixing {

struct binary_tree_visitor;
class binary_tree_weight_node;

extern u32 g_binary_tree_node_constructors;
extern u32 g_binary_tree_node_destructors;

struct XRAY_NOVTABLE binary_tree_base_node : public intrusive_base {
	inline				binary_tree_base_node		( );
	virtual				~binary_tree_base_node		( ) = 0;

#ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
	inline				binary_tree_base_node		( binary_tree_base_node const& other );
#endif // #ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS

	virtual	void		accept						( binary_tree_visitor& visitor ) = 0;
	virtual	binary_tree_weight_node* cast_weight	( ) { return 0; }
	virtual	base_interpolator const* interpolator	( ) const { return 0; }

	template < typename T >
	static inline void	destroy						( T* object );

	binary_tree_base_node*	m_next_weight;
	binary_tree_base_node*	m_same_weight;
	binary_tree_base_node*	m_next_unique_interpolator;

private:
	friend class threading::single_threading_policy;
	u32						m_reference_count;
}; // struct binary_tree_base_node

typedef intrusive_ptr<
			binary_tree_base_node,
			binary_tree_base_node,
			threading::single_threading_policy
		> binary_tree_base_node_ptr;

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_binary_tree_base_node_inline.h"

#endif // #ifndef BINARY_TREE_BASE_NODE_H_INCLUDED