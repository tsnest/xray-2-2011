////////////////////////////////////////////////////////////////////////////
//	Created		: 04.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_helper.h"
#include "particle_editor.h"

namespace xray{
namespace particle_editor{

pe_transform_control_helper::pe_transform_control_helper( particle_editor^ pe ) :
	super				( pe->scene().c_ptr(), pe->scene_view().c_ptr(), pe->render_output_window().c_ptr() ),
	m_particle_editor	( pe )
{
}

float4x4 pe_transform_control_helper::get_inverted_view_matrix( )
{
	return m_particle_editor->view_window->get_inverted_view_matrix();
}

float4x4 pe_transform_control_helper::get_projection_matrix( )
{
	return m_particle_editor->view_window->get_projection_matrix();
}

void pe_transform_control_helper::get_mouse_ray( float3& origin, float3& direction )
{
	m_particle_editor->view_window->get_mouse_ray( origin, direction );
}


} //namespace particle_editor
} //namespace xray
