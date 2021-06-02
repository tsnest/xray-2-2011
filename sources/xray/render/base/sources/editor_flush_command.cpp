////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_flush_command.h"
#include "editor_renderer.h"

namespace xray {
namespace render {
namespace editor {

flush_command::~flush_command	( )
{
}

void flush_command::execute		( )
{
	m_renderer.flush_impl();
}

} // namespace editor
} // namespace render
} // namespace xray