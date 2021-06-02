////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_NODE_CONSTRUCTOR_INLINE_H_INCLUDED
#define N_ARY_TREE_NODE_CONSTRUCTOR_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline n_ary_tree_base_node* n_ary_tree_node_constructor::result	( ) const
{
	ASSERT	( m_result );
	return	m_result;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef N_ARY_TREE_NODE_CONSTRUCTOR_INLINE_H_INCLUDED