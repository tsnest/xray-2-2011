#include "pch.h"
#include "options_page_view_panel.h"

namespace xray{
namespace editor{

void options_page_view_panel::activate_page()
{
	m_backup_values["near_plane"]		= m_view_window->m_near_plane;
	m_near_plane_box->Value				= System::Convert::ToDecimal(m_view_window->m_near_plane);
	
	m_backup_values["far_plane"]		= m_view_window->m_far_plane;
	m_far_plane_box->Value				= System::Convert::ToDecimal(m_view_window->m_far_plane);
	

	m_backup_values["mouse_sens"]		= m_view_window->mouse_sensitivity();
	float v = mouse_sens_track->Minimum + (mouse_sens_track->Maximum-mouse_sens_track->Minimum) * m_view_window->mouse_sensitivity();
	mouse_sens_track->Value				=  (int)v; 

	float gs							= xray::editor_base::transform_control_base::gizmo_size;
	m_backup_values["gizmo_size"]		= gs;
	gizmo_size_track->Value				=  math::floor(gizmo_size_track->Maximum * gs); 

	float grid_step						= editor_base::scene_view_panel::m_grid_step;
	m_backup_values["grid_step"]		= grid_step;
	grid_step_UpDown->Value				= System::Convert::ToDecimal(grid_step); 

	m_backup_values["grid_dim"]		= editor_base::scene_view_panel::m_grid_dim;
	grid_size_numericUpDown->Value		= editor_base::scene_view_panel::m_grid_dim;

	Visible								= true;
}

bool options_page_view_panel::changed( )
{
	math::float2 near_far_backup;
	near_far_backup.x		= System::Convert::ToSingle(m_backup_values["near_plane"]);
	near_far_backup.y		= System::Convert::ToSingle(m_backup_values["far_plane"]);

	bool changed			= (m_near_plane_box->Value != System::Convert::ToDecimal(near_far_backup.x) );
	changed					|= (m_far_plane_box->Value != System::Convert::ToDecimal(near_far_backup.y) );

	float v				= (mouse_sens_track->Value-mouse_sens_track->Minimum) / float( mouse_sens_track->Maximum-mouse_sens_track->Minimum );
	changed			|= (System::Convert::ToSingle(m_backup_values["mouse_sens"])!=v );




	float const f	= (float)gizmo_size_track->Value / gizmo_size_track->Maximum;
	float const gs	= xray::editor_base::transform_control_base::gizmo_size;
	changed			|= math::is_similar( f, gs );

	float step		= System::Convert::ToSingle(m_backup_values["grid_step"]);
	changed			|= (System::Convert::ToSingle(grid_step_UpDown->Value) != step); 

	u32 gcount		= System::Convert::ToUInt32(m_backup_values["grid_dim"]);
	changed			|= editor_base::scene_view_panel::m_grid_dim == gcount;

	return			changed;
}

void options_page_view_panel::deactivate_page()
{
}

bool options_page_view_panel::accept_changes()
{
	return								true;
}

void options_page_view_panel::cancel_changes()
{
	m_view_window->m_near_plane = System::Convert::ToSingle(m_backup_values["near_plane"]);
	m_view_window->m_far_plane = System::Convert::ToSingle(m_backup_values["far_plane"]);

	float sens_val = System::Convert::ToSingle(m_backup_values["mouse_sens"]);
	m_view_window->set_mouse_sensitivity( sens_val );
	
	float gs = System::Convert::ToSingle(m_backup_values["gizmo_size"]);
	xray::editor_base::transform_control_base::gizmo_size = gs;

	editor_base::scene_view_panel::m_grid_step = System::Convert::ToSingle(m_backup_values["grid_step"]);
	editor_base::scene_view_panel::m_grid_dim = System::Convert::ToUInt32(m_backup_values["grid_dim"]);
}

void options_page_view_panel::m_near_plane_box_ValueChanged(System::Object^, System::EventArgs^)
{
	m_view_window->m_near_plane = System::Convert::ToSingle(m_near_plane_box->Value);
	m_view_window->m_far_plane = System::Convert::ToSingle(m_far_plane_box->Value);
	m_view_window->update_projection_matrix();
}

void options_page_view_panel::mouse_sens_track_ValueChanged(System::Object^ , System::EventArgs^ )
{
	float v = (mouse_sens_track->Value-mouse_sens_track->Minimum) / float( mouse_sens_track->Maximum-mouse_sens_track->Minimum );
	m_view_window->set_mouse_sensitivity		( v );
}

void options_page_view_panel::gizmo_size_track_ValueChanged(System::Object^, System::EventArgs^ )
{
	float f = (float)gizmo_size_track->Value / gizmo_size_track->Maximum;
	xray::editor_base::transform_control_base::gizmo_size = f;
}

void options_page_view_panel::grid_step_UpDown_ValueChanged(System::Object^, System::EventArgs^ )
{
	editor_base::scene_view_panel::m_grid_step = System::Convert::ToSingle(grid_step_UpDown->Value);
}

void options_page_view_panel::grid_size_numericUpDown_ValueChanged(System::Object^, System::EventArgs^ )
{
	editor_base::scene_view_panel::m_grid_dim = System::Convert::ToUInt32(grid_size_numericUpDown->Value);
}


}
}