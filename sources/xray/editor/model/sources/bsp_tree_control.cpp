#include "pch.h"
#include "bsp_tree_control.h"
#include "bsp_tree_node.h"
#include "bsp_tree.h"

namespace xray {
namespace model_editor{

System::Void bsp_tree_control::min_square_trackbar_Scroll(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	float const value = min_square_trackbar->Value / 1000.0f;
	bsp_tree_node::ms_min_portal_square = value;
	lbl_min_square->Text				= System::String::Format( "Min square threshold: {0}", bsp_tree_node::ms_min_portal_square );
}

System::Void bsp_tree_control::slimness_threshold_trackbar_Scroll(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	float const value =  slimness_threshold_trackbar->Value / 1000.0f;
	bsp_tree_node::ms_slimness_threshold = value;
	lbl_slimness_threshold->Text		= System::String::Format( "Min slimness threshold: {0}", bsp_tree_node::ms_slimness_threshold );
}

System::Void bsp_tree_control::chb_aabb_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_aabb = chb_aabb->Checked;
}

System::Void bsp_tree_control::chb_solid_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_solid = chb_solid->Checked;

}

System::Void bsp_tree_control::chb_double_sided_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_double_sided = chb_double_sided->Checked;
}

System::Void bsp_tree_control::chb_draw_divider_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_divider = chb_draw_divider->Checked;
}

System::Void bsp_tree_control::chb_geometry_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_geometry = chb_geometry->Checked;
}

System::Void bsp_tree_control::chb_portal_blanks_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_portal_blanks = chb_portal_blanks->Checked;
}

System::Void bsp_tree_control::chb_portals_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_portal = chb_portals->Checked;
}

System::Void bsp_tree_control::chb_outer_edges_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_outer_edges = chb_outer_edges->Checked;
}

System::Void bsp_tree_control::chb_generated_portals_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree::ms_draw_extracted_portals = chb_generated_portals->Checked;
}

System::Void bsp_tree_control::bsp_tree_control_Load(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);

	chb_aabb->Checked					= bsp_tree_node::ms_draw_aabb;
	chb_solid->Checked					= bsp_tree_node::ms_draw_solid;
	chb_double_sided->Checked			= bsp_tree_node::ms_draw_double_sided;
	chb_draw_divider->Checked			= bsp_tree_node::ms_draw_divider;
	chb_geometry->Checked				= bsp_tree_node::ms_draw_geometry;
	chb_portal_blanks->Checked			= bsp_tree_node::ms_draw_portal_blanks;
	chb_portals->Checked				= bsp_tree_node::ms_draw_portal;
	chb_outer_edges->Checked			= bsp_tree_node::ms_draw_outer_edges;
	chb_generated_portals->Checked		= bsp_tree::ms_draw_extracted_portals;

	min_square_trackbar->Value			= static_cast<int>( bsp_tree_node::ms_min_portal_square * 1000 );
	lbl_min_square->Text				= System::String::Format( "Min square threshold: {0}", bsp_tree_node::ms_min_portal_square );

	slimness_threshold_trackbar->Value	= static_cast<int>( bsp_tree_node::ms_slimness_threshold * 1000 );
	lbl_slimness_threshold->Text		= System::String::Format( "Min slimness threshold: {0}", bsp_tree_node::ms_slimness_threshold );
}


}
}