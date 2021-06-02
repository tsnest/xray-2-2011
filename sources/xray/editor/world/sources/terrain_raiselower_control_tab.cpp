////////////////////////////////////////////////////////////////////////////
//	Created		: 14.01.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_raiselower_control_tab.h"
#include "terrain_modifier_raise_lower.h"
#include "tool_terrain.h"

using xray::editor::terrain_raiselower_control_tab;
using xray::editor::terrain_modifier_raise_lower;

void terrain_raiselower_control_tab::in_constructor()
{
	terrain_modifier_raise_lower^ control = m_tool->m_terrain_modifier_raise_lower;
	control->subscribe_on_activated		(gcnew activate_control_event(this, &terrain_raiselower_control_tab::on_control_activated));
	control->subscribe_on_deactivated	(gcnew deactivate_control_event(this, &terrain_raiselower_control_tab::on_control_deactivated));
}

void terrain_raiselower_control_tab::on_control_activated(xray::editor::editor_control_base^ c)
{
	XRAY_UNREFERENCED_PARAMETER(c);
	Show						();
	sync						(true);
}

void terrain_raiselower_control_tab::on_control_deactivated(xray::editor::editor_control_base^ c)
{
	Hide						();
	XRAY_UNREFERENCED_PARAMETER	(c);
}

void terrain_raiselower_control_tab::sync(bool b_sync_ui)
{
	if(m_in_sync)
		return;

	m_in_sync = true;
	terrain_modifier_raise_lower^ control = m_tool->m_terrain_modifier_raise_lower;

	if(b_sync_ui)
	{
		radius_track_bar->fltValue		= control->radius;
		hardness_track_bar->fltValue	= control->hardness;
		distortion_track_bar->fltValue	= control->distortion;
		height_track_bar->fltValue		= control->strength;
	}else
	{
		control->radius					= radius_track_bar->fltValue;
		control->hardness				= hardness_track_bar->fltValue;
		control->distortion				= distortion_track_bar->fltValue;
		control->strength				= height_track_bar->fltValue;
	}

	m_in_sync = false;
}

void terrain_raiselower_control_tab::sync(System::Object^, System::EventArgs^)
{
	sync					(false);
}
