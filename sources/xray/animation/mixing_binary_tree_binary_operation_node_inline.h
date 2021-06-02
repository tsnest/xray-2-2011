////////////////////////////////////////////////////////////////////////////
//	Created		: 26.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BINARY_TREE_BINARY_OPERATION_NODE_INLINE_H_INCLUDED
#define BINARY_TREE_BINARY_OPERATION_NODE_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline binary_tree_binary_operation_node::binary_tree_binary_operation_node	(
		binary_tree_base_node* const left,
		binary_tree_base_node* const right
	) :
	m_left	( left ),
	m_right	( right )
{
}

inline binary_tree_base_node& binary_tree_binary_operation_node::left		( ) const
{
	ASSERT	( m_left );
	return	*m_left;
}

inline binary_tree_base_node& binary_tree_binary_operation_node::right		( ) const
{
	ASSERT	( m_right );
	return	*m_right;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef BINARY_TREE_BINARY_OPERATION_NODE_INLINE_H_INCLUDED