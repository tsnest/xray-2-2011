////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "debug_draw_lines_command.h"
#include "debug_renderer.h"

using xray::render::debug::draw_lines_command;

draw_lines_command::~draw_lines_command	( )
{
}

void draw_lines_command::execute		( )
{
	m_renderer.draw_lines_impl	( m_vertices, m_indices );
}