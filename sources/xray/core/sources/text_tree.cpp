////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.08.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/text_tree.h>
#include <xray/linkage_helper.h>

XRAY_DECLARE_LINKAGE_ID(core_text_tree)

using xray::strings::text_tree;

text_tree::text_tree(pvoid buffer, u32 const buffer_size, pcstr root_name)
:m_root(m_allocator, NULL)
{	
	m_allocator.initialize(buffer, buffer_size, "text_tree");
	m_root.set_name(root_name);
}

text_tree::~text_tree()
{	
	m_allocator.finalize();
}

void text_tree::clear()
{
	m_root.clear();
	m_allocator.reset();
}

void text_tree::break_page()
{
	m_root.new_child(NULL, true);
}