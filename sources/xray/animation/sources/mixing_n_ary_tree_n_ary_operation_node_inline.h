////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_N_ARY_OPERATION_NODE_INLINE_H_INCLUDED
#define MIXING_N_ARY_TREE_N_ARY_OPERATION_NODE_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline n_ary_tree_n_ary_operation_node::n_ary_tree_n_ary_operation_node	( u32 const operands_count ) :
	m_operands_count	( operands_count )
{
}

inline n_ary_tree_n_ary_operation_node::n_ary_tree_n_ary_operation_node	( n_ary_tree_n_ary_operation_node const& other ) :
	n_ary_tree_base_node( other ),
	m_operands_count	( other.m_operands_count )
{
}

inline u32 n_ary_tree_n_ary_operation_node::operands_count				( ) const
{
	return				m_operands_count;
}

inline n_ary_tree_base_node** n_ary_tree_n_ary_operation_node::operands	( u32 const size )

{
	return				pointer_cast< n_ary_tree_base_node** >( pointer_cast<pbyte>(this) + size );
}

inline void n_ary_tree_n_ary_operation_node::decrement_operands_count	( )
{
	R_ASSERT			( m_operands_count );
	--m_operands_count;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_N_ARY_OPERATION_NODE_INLINE_H_INCLUDED