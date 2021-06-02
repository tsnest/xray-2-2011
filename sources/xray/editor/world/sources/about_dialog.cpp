#include "pch.h"
#include "about_dialog.h"

#pragma managed( push, off )
namespace xray {
namespace build {
	XRAY_CORE_API	u32		calculate_build_id			( pcstr current_date );
	XRAY_CORE_API	pcstr	build_date					( );
	XRAY_CORE_API	u32		build_station_build_id		( );
} // namespace build
} // namespace xray
#pragma managed( pop )

using namespace System;

namespace xray {
namespace editor {

void about_dialog::button_ok_Click		(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS			( sender, e );
	this->Close();
}

void about_dialog::about_dialog_Load	(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS		( sender, e );

	u32 const build_station_build_id	= xray::build::build_station_build_id();
	label1->Text	= 
		"              X-Ray Editor v2.0a\r\n"
		"\r\n"
		"         build #" +
		build_station_build_id + ", " + gcnew System::String( xray::build::build_date() ) +
		"\r\n"
		"\r\n"
		"Copyright(C) GSC Game World - 2011";
}

} // namespace editor 
} // namespace xray