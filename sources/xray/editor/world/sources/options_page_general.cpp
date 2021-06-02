#include "pch.h"
#include "options_page_general.h"
#include "editor_world.h"

namespace xray {
namespace editor {


void options_page_general::activate_page()
{
	bool current_value					= m_editor_world.m_b_draw_collision;
	m_backup_values["draw_collision"]	= current_value;
	draw_collision_check_box->Checked	= current_value;

	float current_focus_dist			= m_editor_world.focus_distance();
	m_backup_values["focus_dist"]		= current_focus_dist;
	m_focus_distance_box->Value			= System::Convert::ToDecimal(current_focus_dist);
	
	math::color& selection_color  = *m_editor_world.view_window()->m_selection_color;
	
	m_backup_values["selection_color_r"]= selection_color.get_R();
	m_backup_values["selection_color_g"]= selection_color.get_G();
	m_backup_values["selection_color_b"]= selection_color.get_B();
	m_backup_values["selection_color_a"]= selection_color.get_A();
	
	m_backup_values["cells_depth"]		= m_editor_world.get_cells_flood_depth();
	cells_depth_numericUpDown->Value	= m_editor_world.get_cells_flood_depth();

	Color color;
	
	color = color.FromArgb(	selection_color.get_A(),
							selection_color.get_R(),
							selection_color.get_G(),
							selection_color.get_B()
							);
	
	colorDialog1->Color = color;
	panel1->BackColor   = color;
	
	m_backup_values["selection_rate"]	= m_editor_world.view_window()->m_selection_rate;
	
	numericUpDown1->Value				= System::Convert::ToDecimal(m_editor_world.view_window()->m_selection_rate);
	Visible								= true;
}

bool options_page_general::changed( )
{
	bool current_value		= m_editor_world.m_b_draw_collision;
	bool backup_value		= System::Convert::ToBoolean(m_backup_values["draw_collision"]);
	bool changed			= (current_value!=backup_value);

	math::float2 near_far_backup;
	near_far_backup.x		= System::Convert::ToSingle(m_backup_values["near_plane"]);
	near_far_backup.y		= System::Convert::ToSingle(m_backup_values["far_plane"]);

	float focus_dist_backup	= System::Convert::ToSingle(m_backup_values["focus_dist"]);
	changed					|= (m_focus_distance_box->Value != System::Convert::ToDecimal(focus_dist_backup) );
	
	xray::math::float4 selection_color(
		System::Convert::ToSingle(m_backup_values["selection_color_x"]),
		System::Convert::ToSingle(m_backup_values["selection_color_y"]),
		System::Convert::ToSingle(m_backup_values["selection_color_z"]),
		System::Convert::ToSingle(m_backup_values["selection_color_w"])
	);
	
	float selection_rate	= System::Convert::ToSingle(m_backup_values["selection_rate"]);
	changed					|= (numericUpDown1->Value != System::Convert::ToDecimal(selection_rate) );
	
	changed					|= (
		panel1->BackColor.R != System::Convert::ToInt32(selection_color.x*255.0f) ||
		panel1->BackColor.G != System::Convert::ToInt32(selection_color.y*255.0f) ||
		panel1->BackColor.G != System::Convert::ToInt32(selection_color.z*255.0f) ||
		panel1->BackColor.A != System::Convert::ToInt32(selection_color.w*255.0f)
	);
	
	changed			|= (System::Convert::ToUInt32(cells_depth_numericUpDown->Value)	!= m_editor_world.get_cells_flood_depth());

	return			changed;
}

void options_page_general::deactivate_page()
{
}

bool options_page_general::accept_changes()
{
	return								true;
}

void options_page_general::cancel_changes()
{
	m_editor_world.m_b_draw_collision	= System::Convert::ToBoolean(m_backup_values["draw_collision"]);

	m_editor_world.view_window()->m_near_plane = System::Convert::ToSingle(m_backup_values["near_plane"]);
	m_editor_world.view_window()->m_far_plane = System::Convert::ToSingle(m_backup_values["far_plane"]);

	float focus_dist_backup	= System::Convert::ToSingle(m_backup_values["focus_dist"]);
	m_editor_world.focus_distance() = focus_dist_backup;

	float sens_val = System::Convert::ToSingle(m_backup_values["mouse_sens"]);
	m_editor_world.view_window()->set_mouse_sensitivity( sens_val );
	
	float gs = System::Convert::ToSingle(m_backup_values["gizmo_size"]);
	xray::editor_base::transform_control_base::gizmo_size = gs;

	u32 depth = System::Convert::ToUInt32(m_backup_values["cells_depth"]);
	m_editor_world.set_cells_flood_depth( depth );
}

void options_page_general::draw_collision_check_box_CheckedChanged(System::Object^, System::EventArgs^)
{
	m_editor_world.m_b_draw_collision = draw_collision_check_box->Checked;
}

void options_page_general::m_focus_distance_box_ValueChanged		( System::Object^, System::EventArgs^ )
{
	float val		= System::Convert::ToSingle(m_focus_distance_box->Value);
	m_editor_world.focus_distance() = val;	
}

void options_page_general::numericUpDown1_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS	( sender, e );
	m_editor_world.view_window()->m_selection_rate = System::Convert::ToSingle(numericUpDown1->Value);
}

void  options_page_general::cells_depth_numericUpDown_ValueChanged(System::Object^ , System::EventArgs^ )
{
	m_editor_world.set_cells_flood_depth ( System::Convert::ToUInt32(cells_depth_numericUpDown->Value) );
}

void options_page_general::button1_Click( System::Object^ , System::EventArgs^ )
{
	if( colorDialog1->ShowDialog() == DialogResult::OK )
	{
		Color color		  = colorDialog1->Color;
		panel1->BackColor = color;
		
		m_editor_world.view_window()->m_selection_color->set( color.R, color.G, color.B, color.A );
	}
}

} //	namespace editor
} //	namespace xray
