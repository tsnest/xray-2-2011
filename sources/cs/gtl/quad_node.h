////////////////////////////////////////////////////////////////////////////
//	Module 		: quad_node.h
//	Created 	: 13.05.2005
//  Modified 	: 13.05.2005
//	Author		: Dmitriy Iassenev
//	Description : quad node class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_QUAD_NODE_H_INCLUDED
#define CS_GTL_QUAD_NODE_H_INCLUDED

namespace gtl { namespace detail {

class quad_node {
public:
	quad_node*			m_neighbours[4];

public:
	inline	quad_node*	&next		();
};

}};

#include <cs/gtl/quad_node_inline.h>

#endif // #ifndef CS_GTL_QUAD_NODE_H_INCLUDED