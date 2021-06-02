////////////////////////////////////////////////////////////////////////////
//	Created		: 09.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TOOL_WINDOW_STORAGE_H_INCLUDED
#define TOOL_WINDOW_STORAGE_H_INCLUDED

namespace xray {
namespace editor {

ref class tool_window_storage : public xray::editor_base::tool_window_holder
{
	typedef System::Collections::Generic::List<editor_base::tool_window^>	windows_list;
public:
								tool_window_storage		( );

	virtual void				register_tool_window	( xray::editor_base::tool_window^ w );
	virtual void				unregister_tool_window	( xray::editor_base::tool_window^ w );

	void						tick					( );
	void						clear_resources			( );
	void						destroy					( );
	bool						close_query				( );
	xray::editor_base::tool_window^ get					( System::String^ name );

protected:
	windows_list					m_active_tool_windows;
}; // class tool_window_storage

} // namespace xray
} // namespace editor

#endif // #ifndef TOOL_WINDOW_STORAGE_H_INCLUDED