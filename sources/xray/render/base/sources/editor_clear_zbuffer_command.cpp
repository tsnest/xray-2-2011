////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_clear_zbuffer_command.h"
#include "editor_renderer.h"

namespace xray {
namespace render {
namespace editor {

clear_zbuffer_command::~clear_zbuffer_command	( )
{
}

void clear_zbuffer_command::execute		( )
{
	m_renderer.clear_zbuffer_impl( m_zvalue );
}

} // namespace editor
} // namespace render
} // namespace xray
