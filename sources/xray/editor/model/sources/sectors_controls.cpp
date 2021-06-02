#include "pch.h"
#include "sectors_controls.h"
#include "sectors_generator.h"
#include "spatial_sector.h"
namespace xray{
namespace model_editor{

	System::Void sectors_controls::chb_delimited_edges_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
	{
		XRAY_UNREFERENCED_PARAMETERS(sender, e);
		sectors_generator::ms_draw_delimited_edges = chb_delimited_edges->Checked;
	}

	System::Void sectors_controls::chb_cuboids_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
	{
		XRAY_UNREFERENCED_PARAMETERS(sender, e);
		sectors_generator::ms_draw_cuboids = chb_cuboids->Checked;
	}

	System::Void sectors_controls::chb_sectors_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
	{
		XRAY_UNREFERENCED_PARAMETERS(sender, e);
		sectors_generator::ms_draw_sectors = chb_sectors->Checked;
	}

	System::Void sectors_controls::sectors_controls_Load(System::Object^  sender, System::EventArgs^  e)
	{
		XRAY_UNREFERENCED_PARAMETERS(sender, e);
		chb_delimited_edges->Checked		= sectors_generator::ms_draw_delimited_edges;
		chb_cuboids->Checked				= sectors_generator::ms_draw_cuboids;
		chb_sectors->Checked				= sectors_generator::ms_draw_sectors;
		chb_draw_all_portals->Checked		= sectors_generator::ms_draw_all_portals;
		chb_draw_hanged->Checked			= sectors_generator::ms_draw_hanged_portals;
		chb_solid_sectors->Checked			= spatial_sector::ms_solid_sectors;

	}

	System::Void sectors_controls::chb_draw_all_portals_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		XRAY_UNREFERENCED_PARAMETERS(sender, e);
		sectors_generator::ms_draw_all_portals	= chb_draw_all_portals->Checked;
		chb_draw_hanged->Enabled				= chb_draw_all_portals->Checked;
	}

	System::Void sectors_controls::chb_draw_hanged_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		XRAY_UNREFERENCED_PARAMETERS(sender, e);
		sectors_generator::ms_draw_hanged_portals = chb_draw_hanged->Checked;
	}

	System::Void sectors_controls::chb_solid_sectors_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		XRAY_UNREFERENCED_PARAMETERS(sender, e);
		spatial_sector::ms_solid_sectors = chb_solid_sectors->Checked;
	}

}
}