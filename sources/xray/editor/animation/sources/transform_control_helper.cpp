////////////////////////////////////////////////////////////////////////////
//	Created		: 09.03.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "transform_control_helper.h"
#include "animation_editor.h"

using xray::animation_editor::ae_transform_control_helper;
using xray::animation_editor::animation_editor;

ae_transform_control_helper::ae_transform_control_helper( animation_editor^ ae ) :
	super				( ae->scene().c_ptr(), ae->scene_view().c_ptr(), ae->render_output_window().c_ptr() ),
	m_animation_editor	( ae )
{
}

float4x4 ae_transform_control_helper::get_inverted_view_matrix()
{
	return m_animation_editor->viewport->get_inverted_view_matrix();
}

float4x4 ae_transform_control_helper::get_projection_matrix( )
{
	return m_animation_editor->viewport->get_projection_matrix();
}

void ae_transform_control_helper::get_mouse_ray(float3& origin, float3& direction)
{
	m_animation_editor->viewport->get_mouse_ray(origin, direction);
}