////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_N_ARY_OPERATION_NODE_H_INCLUDED
#define N_ARY_TREE_N_ARY_OPERATION_NODE_H_INCLUDED

#include "mixing_n_ary_tree_base_node.h"

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_n_ary_operation_node : public n_ary_tree_base_node {
public:
	inline	n_ary_tree_n_ary_operation_node		( u32 const operands_count );
	inline	n_ary_tree_n_ary_operation_node		( n_ary_tree_n_ary_operation_node const& other );
	inline	u32				operands_count		( ) const;
	inline	void		decrement_operands_count( );
	inline	n_ary_tree_base_node** operands		( u32 const size );

private:
	n_ary_tree_n_ary_operation_node& operator=	( n_ary_tree_n_ary_operation_node const& other);

private:
	u32		m_operands_count;
}; // class n_ary_tree_n_ary_operation_node

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_n_ary_tree_n_ary_operation_node_inline.h"

#endif // #ifndef N_ARY_TREE_N_ARY_OPERATION_NODE_H_INCLUDED