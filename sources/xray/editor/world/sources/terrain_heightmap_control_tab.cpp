////////////////////////////////////////////////////////////////////////////
//	Created		: 14.01.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_heightmap_control_tab.h"
#include "tool_terrain.h"

using xray::editor::terrain_heightmap_control_tab;

void terrain_heightmap_control_tab::in_constructor()
{
}

void terrain_heightmap_control_tab::on_control_activated(xray::editor::editor_control_base^ c)
{
	XRAY_UNREFERENCED_PARAMETER(c);
	Show						();
	sync						(true);
}

void terrain_heightmap_control_tab::on_control_deactivated(xray::editor::editor_control_base^ c)
{
	Hide						();
	XRAY_UNREFERENCED_PARAMETER	(c);
}

void terrain_heightmap_control_tab::sync(bool b_sync_ui)
{
	XRAY_UNREFERENCED_PARAMETER	( b_sync_ui );

	//if(m_in_sync)
	//	return;

	//m_in_sync = true;
	//terrain_modifier_bump^		m_control = m_tool->m_terrain_modifier_bump;

	//if(b_sync_ui)
	//{
	//	radius_track_bar->fltValue		= m_control->radius;
	//	hardness_track_bar->fltValue	= m_control->hardness;
	//	distortion_track_bar->fltValue	= m_control->distortion;
	//	strength_track_bar->fltValue	= m_control->strength;
	//	grab_mode_check->Checked		= m_control->grab_mode;
	//}else
	//{
	//	m_control->radius				= radius_track_bar->fltValue;
	//	m_control->hardness				= hardness_track_bar->fltValue;
	//	m_control->distortion			= distortion_track_bar->fltValue;
	//	m_control->strength				= strength_track_bar->fltValue;
	//	m_control->grab_mode			= grab_mode_check->Checked;
	//}

	//m_in_sync = false;
}

//void terrain_flatten_control_tab::sync(System::Object^, System::EventArgs^)
//{
//	sync					(false);
//}
