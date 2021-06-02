////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_setup_rotation_control_modes_command.h"
#include "editor_renderer.h"

namespace xray {
namespace render {
namespace editor {

setup_rotation_control_modes_command::~setup_rotation_control_modes_command	( )
{
}

void setup_rotation_control_modes_command::execute		( )
{
	m_renderer.setup_rotation_control_modes_impl( m_color_write);
}

} // namespace editor
} // namespace render
} // namespace xray