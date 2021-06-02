////////////////////////////////////////////////////////////////////////////
//	Created		: 09.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tool_window_storage.h"

namespace xray{
namespace editor{

tool_window_storage::tool_window_storage( )
{
}

void tool_window_storage::register_tool_window	( editor_base::tool_window^ w )
{
	R_ASSERT(!m_active_tool_windows.Contains(w));
	m_active_tool_windows.Add( w );
}

void tool_window_storage::unregister_tool_window	( editor_base::tool_window^ w )
{
	R_ASSERT(m_active_tool_windows.Contains(w));
	m_active_tool_windows.Remove( w );
}

bool tool_window_storage::close_query( )
{
	for each ( editor_base::tool_window^ w in m_active_tool_windows )
	{
		if( !w->close_query() )
			return false;
	}

	return true;
}

void tool_window_storage::destroy( )
{
	windows_list					copy_tool_windows;
	copy_tool_windows.AddRange		(%m_active_tool_windows);

	for each ( editor_base::tool_window^ w in copy_tool_windows )
		delete w;

	m_active_tool_windows.Clear();
}

void tool_window_storage::tick( )
{
	for each ( editor_base::tool_window^ w in m_active_tool_windows )
	{
		if( xray::editor_base::editors_helper::is_active_form_belong_to( w->main_form() ) )
		{
			w->tick( );
			break;
		}
	}
}

void tool_window_storage::clear_resources( )
{
	for each ( editor_base::tool_window^ w in m_active_tool_windows )
		w->clear_resources( );
}

xray::editor_base::tool_window^ tool_window_storage::get( System::String^ name )
{
	for each ( editor_base::tool_window^ w in m_active_tool_windows )
		if(w->name() == name)
			return w;

	return nullptr;
}

} //namespace editor
} //namespace xray
