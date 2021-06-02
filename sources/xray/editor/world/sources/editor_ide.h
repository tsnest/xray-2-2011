////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_IDE_H_INCLUDED
#define EDITOR_IDE_H_INCLUDED

namespace xray {
namespace editor_base { enum images16x16; }
namespace editor {

ref class gui_binder;
interface class options_manager;

public interface class editor_ide
{
	typedef System::Windows::Forms::IWin32Window		IWin32Window;
	typedef WeifenLuo::WinFormsUI::Docking::DockContent	DockContent;
	typedef WeifenLuo::WinFormsUI::Docking::DockState	DockState;

public:
	void							show_tab			(DockContent^) = 0;
	void							show_tab			(DockContent^ tab, DockState state) = 0;
	
	void							add_menu_item		(System::String^ key, System::String^ text, int prio) = 0; // add sub-folder
	void							add_menu_item		(gui_binder^ binder, System::String^ action_name, System::String^ key, int prio) = 0;
	void							add_button_item		(System::String^ key, int prio) = 0; // add empty strip
	void							add_button_item		(gui_binder^ binder, System::String^ action_name, System::String^ key, int prio) = 0;
	void							set_status_label	(int idx, System::String^ text) = 0;
	
	void							register_image		(System::String^ action_name, xray::editor_base::images16x16 image_id ) = 0;
	System::Drawing::Image^			get_image			(System::String^ key) = 0;
	RegistryKey^					base_registry_key	() = 0;
	IWin32Window^					wnd					() = 0;
	options_manager^				get_options_manager	() = 0;

	System::Collections::ArrayList^			get_held_keys			() = 0;
	int										get_current_keys_string	( System::String^& combination ) = 0;
	void									get_mouse_position	(xray::math::int2& ) = 0;
	void									get_mouse_position	(System::Drawing::Point%) = 0;
	void									set_mouse_position	(System::Drawing::Point) = 0;
	void									get_view_size		(xray::math::int2&) = 0;
	void									get_view_size		(System::Drawing::Size%) = 0;
	void									set_mouse_sensivity	(float sensivity) = 0;
}; // interface class editor_ide

}// namespace editor
}// namespace xray

#endif // #ifndef EDITOR_IDE_H_INCLUDED