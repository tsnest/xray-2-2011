#include "pch.h"
#include "triangulation_tester.h"
#include <xray/configs_lua_config.h>

using xray::editor::triangulation_tester;
using namespace xray::configs;

void triangulation_tester::open_button_onClick(System::Object^, System::EventArgs^ ) {
	OpenFileDialog dialog;
	dialog.Filter = "(*.lua)|";
	dialog.InitialDirectory = "E:/stalker2/resources/";
	
	if ( dialog.ShowDialog() != System::Windows::Forms::DialogResult::OK )
		return;

	canvas->load_from_file ( dialog.FileName );
	triangulation_type->SelectedIndex = canvas->get_triangulation_type();
}

void triangulation_tester::save_button_onClick(System::Object^, System::EventArgs^ ) {
	SaveFileDialog dialog;
	dialog.Filter = "(*.lua)|";
	dialog.InitialDirectory = "E:/stalker2/resources/";
	
	if ( dialog.ShowDialog() != System::Windows::Forms::DialogResult::OK )
		return;

	lua_config_ptr config = create_lua_config( unmanaged_string( dialog.FileName ).c_str( ) );
	canvas->build_lua_config( config );
	config->save( xray::configs::target_sources );
}

void triangulation_tester::show_bounding_triangle_onCheckedChanged (System::Object^, System::EventArgs^ ) 
{
	canvas->show_bounding_triangle( show_bounding_triangle->Checked );
}

void triangulation_tester::clear_vertices_onClick (System::Object^, System::EventArgs^ ) 
{
	canvas->clear_vertices( );
}

void triangulation_tester::triangulation_type_onChange(System::Object^, System::EventArgs^ )
{
	canvas->set_triangulation_type ( triangulation_type->SelectedIndex );
}

void triangulation_tester::show_errors_onCheckedChanged (System::Object^, System::EventArgs^ )
{
	canvas->set_show_errors ( show_errors->Checked );
}
