////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_projection_matrix.h"
#include <xray/render/base/world.h>
#include <xray/render/base/platform.h>

using xray::render::command_projection_matrix;

command_projection_matrix::command_projection_matrix( xray::render::platform& platform, xray::float4x4 const& projection ) :
m_projection						( projection ),
m_platform					( platform )
{
}

void command_projection_matrix::execute		( )
{
	m_platform.set_projection_matrix	( m_projection );
}