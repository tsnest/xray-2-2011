////////////////////////////////////////////////////////////////////////////
//	Module 		: quad_node_inline.h
//	Created 	: 13.05.2005
//  Modified 	: 13.05.2005
//	Author		: Dmitriy Iassenev
//	Description : quad node class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_QUAD_NODE_INLINE_H_INCLUDED
#define CS_GTL_QUAD_NODE_INLINE_H_INCLUDED

inline	gtl::detail::quad_node* &gtl::detail::quad_node::next	()
{
	return	m_neighbours[0];
}

#endif // #ifndef CS_GTL_QUAD_NODE_INLINE_H_INCLUDED