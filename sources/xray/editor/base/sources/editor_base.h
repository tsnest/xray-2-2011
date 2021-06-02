////////////////////////////////////////////////////////////////////////////
//	Created		: 03.06.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_BASE_H_INCLUDED
#define EDITOR_BASE_H_INCLUDED

using Microsoft::Win32::RegistryKey;

namespace xray {
namespace editor_base {
	interface class tool_window_holder;

	public interface class editor_base
	{
	public:
		virtual void				initialize				( ) = 0;
		virtual void				clear_resources			( ) = 0;
		virtual void				tick					( ) = 0;
		virtual bool				close_query				( ) = 0;
		virtual void				show					( ) = 0;
		virtual	System::String^		name					( ) = 0;
	}; // class editor_base

	public interface class tool_window
	{
	public:
		virtual void				Show					( System::String^ context1, System::String^ context2 ) = 0;
		virtual void				tick					( ) = 0;
		virtual void				clear_resources			( ) = 0;
		virtual bool				close_query				( ) = 0;
		virtual	System::String^		name					( ) = 0;
		virtual System::Windows::Forms::Form^ main_form		( ) = 0;
		virtual void				load_settings			( RegistryKey^ product_key ) =0;
		virtual void				save_settings			( RegistryKey^ product_key ) =0;
	};//

	public interface class tool_window_holder
	{
	public:
		virtual void				register_tool_window	( tool_window^ w ) = 0;
		virtual void				unregister_tool_window	( tool_window^ w ) = 0;
	};

} // namespace editor_base
} // namespace xray

#endif // #ifndef EDITOR_BASE_H_INCLUDED