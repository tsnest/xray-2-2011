////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/mixing_binary_tree_addition_node.h>
#include <xray/animation/mixing_binary_tree_visitor.h>

using xray::animation::mixing::binary_tree_addition_node;
using xray::animation::mixing::binary_tree_visitor;
using xray::animation::mixing::binary_tree_base_node;

void binary_tree_addition_node::accept	( binary_tree_visitor& visitor )
{
	visitor.visit	( *this );
}