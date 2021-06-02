////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_view_matrix.h"
#include <xray/render/base/world.h>
#include <xray/render/base/platform.h>

using xray::render::command_view_matrix;

command_view_matrix::command_view_matrix( xray::render::platform& platform, xray::float4x4 const& view ) :
	m_view						( view ),
	m_platform					( platform )
{
}

void command_view_matrix::execute		( )
{
	m_platform.set_view_matrix	( m_view );
}