////////////////////////////////////////////////////////////////////////////
//	Created		: 11.01.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATIONS_LIST_PANEL_H_INCLUDED
#define ANIMATIONS_LIST_PANEL_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace xray::editor::controls;

namespace xray {
namespace animation_editor {

	ref class animation_setup_manager;

	public ref class animations_list_panel : public xray::editor::controls::file_view_panel_base
	{
		typedef xray::editor::controls::file_view_panel_base super;
	public:
						animations_list_panel			(animation_setup_manager^ m);
						~animations_list_panel			()	{};
		virtual	void	track_active_item				(System::String^ path) override;

	protected:
		virtual	void	on_node_double_click			(System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e) override;
		virtual	void	on_node_key_down				(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e) override;

	private:
		System::Windows::Forms::ImageList^			tree_view_image_list;
		System::Windows::Forms::ContextMenuStrip^	m_context_menu;
		animation_setup_manager^					m_manager;
	}; // ref class channels_files_view_panel
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATIONS_LIST_PANEL_H_INCLUDED