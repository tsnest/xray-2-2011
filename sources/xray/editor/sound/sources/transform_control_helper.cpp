//-------------------------------------------------------------------------------------------
//	Created		: 29.06.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
//-------------------------------------------------------------------------------------------
#include "pch.h"
#include "transform_control_helper.h"
#include "sound_scene_document.h"

using xray::sound_editor::se_transform_control_helper;
using xray::sound_editor::se_transform_value_object;
using xray::sound_editor::sound_scene_document;
//-------------------------------------------------------------------------------------------
//-- ref class se_transform_control_helper --------------------------------------------------
//-------------------------------------------------------------------------------------------
se_transform_control_helper::se_transform_control_helper(sound_scene_document^ doc)
:super(doc->scene().c_ptr(), doc->scene_view().c_ptr(), doc->render_output_window().c_ptr()),
m_document(doc)
{
}

float4x4 se_transform_control_helper::get_inverted_view_matrix()
{
	return m_document->viewport->get_inverted_view_matrix();
}

float4x4 se_transform_control_helper::get_projection_matrix( )
{
	return m_document->viewport->get_projection_matrix();
}

void se_transform_control_helper::get_mouse_ray(float3& origin, float3& direction)
{
	m_document->viewport->get_mouse_ray(origin, direction);
}
//-------------------------------------------------------------------------------------------
//-- ref class se_transform_value_object ----------------------------------------------------
//-------------------------------------------------------------------------------------------
se_transform_value_object::se_transform_value_object(xray::math::float4x4* m, Action<bool>^ d)
:m_delegate(d)
{
	m_value_transform = m;
	m_start_transform = NEW(math::float4x4)(*m);
}

se_transform_value_object::~se_transform_value_object()
{
	DELETE(m_start_transform);
}

void se_transform_value_object::start_modify(transform_control_base^ control)
{
	XRAY_UNREFERENCED_PARAMETER(control);
	(*m_start_transform) = *m_value_transform;
}

void se_transform_value_object::execute_preview(transform_control_base^ control)
{
	math::float4x4 m = control->calculate(*m_start_transform);

	bool bneed_update = (
		!math::is_similar(m.i.xyz(), (*m_value_transform).i.xyz()) ||
		!math::is_similar(m.j.xyz(), (*m_value_transform).j.xyz()) ||
		!math::is_similar(m.k.xyz(), (*m_value_transform).k.xyz()) ||
		!math::is_similar(m.c.xyz(), (*m_value_transform).c.xyz())
	);

	if(bneed_update)
	{
		if(m_delegate)
			m_delegate(false);

		*m_value_transform = m;
	}
}

void se_transform_value_object::end_modify(transform_control_base^)
{
	if(m_delegate)
		m_delegate(true);
}

float4x4 se_transform_value_object::get_ancor_transform()
{
	return *m_value_transform;
}

float4x4 se_transform_value_object::get_start_transform()
{
	return *m_start_transform;
}
