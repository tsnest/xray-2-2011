////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tool_base.h"
#include "object_base.h"
#include "level_editor.h"
#include "editor_world.h"
#include "object_inspector_tab.h"
#include "lua_config_value_editor.h"

namespace xray {
namespace editor {

tool_base::tool_base( level_editor^ le, System::String^ name )
:m_level_editor		( le ),
m_tool_name			( name ),
m_b_ready			( false ),
m_selected_library	( "" )
{
	m_library			= gcnew objects_library;
}

tool_base::~tool_base( )
{
	delete m_library;
}

void tool_base::select_library_name( System::String^ name )
{
	R_ASSERT(name!=nullptr);
	m_selected_library = name;
}

void tool_base::on_library_files_loaded( resources::queries_result& data )
{
	R_ASSERT					( data.is_successful() );
	configs::lua_config* cfg	= static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
	m_library->load				( cfg )	;
	
	on_library_loaded			( );
	m_b_ready					= true;
	m_level_editor->tool_loaded	( this );
}

void tool_base::on_commit_library_item( lib_item^ lib_item )
{
	m_library->on_commit			( lib_item );

	on_library_loaded		( );
}


window_ide^ tool_base::ide( )
{
	return get_level_editor()->ide();
}

void tool_base::load_settings( RegistryKey^ level_editor_key )
{
	System::String^ key_name = "selected_"+ name();
	System::String^ sel			= level_editor_key->GetValue(key_name, "")->ToString();
	select_library_name			( sel );
}

void tool_base::save_settings( RegistryKey^ level_editor_key )
{
	System::String^ key_name	= "selected_"+ name();
	level_editor_key->SetValue	( key_name, get_selected_library_name() );
}

} // namespace editor
} // namespace xray
