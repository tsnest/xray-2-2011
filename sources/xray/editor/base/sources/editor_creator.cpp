////////////////////////////////////////////////////////////////////////////
//	Created		: 05.08.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "editor_creator.h"

using xray::editor_base::editor_creator;

void editor_creator::set_memory_allocator(xray::editor_base::allocator_type* allocator)
{
	ASSERT(!g_allocator);
	g_allocator = allocator;
}
