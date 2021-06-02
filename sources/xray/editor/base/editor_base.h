//-------------------------------------------------------------------------------------------
//	Created		: 03.06.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
//-------------------------------------------------------------------------------------------

#ifndef EDITOR_BASE_H_INCLUDED
#define EDITOR_BASE_H_INCLUDED

namespace xray {
namespace editor_base {
	ref class command_engine;

	public interface class editor_base
	{
	public:
		virtual xray::editor_base::command_engine^	get_command_engine	() = 0;
		virtual void							on_render				() = 0;
		virtual void							initialize				() = 0;
		virtual void							destroy					() = 0;
		virtual void							clear_resources			() = 0;
		virtual void							tick					() = 0;
		virtual bool							close_query				() = 0;
		virtual bool							load					(System::String^ name) = 0;
		virtual void							show					() = 0;
		virtual void							load_settings			() = 0;
		virtual void							save_settings			() = 0;
		virtual	void							set_default_layout		() = 0;
		virtual WeifenLuo::WinFormsUI::Docking::IDockContent^	find_dock_content(System::String^ ) = 0;
		virtual	System::String^					name					() = 0;
	}; // class editor_base
} // namespace editor_base
} // namespace xray
#endif // #ifndef EDITOR_BASE_H_INCLUDED