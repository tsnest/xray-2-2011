////////////////////////////////////////////////////////////////////////////
//	Created		: 19.08.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef CHANNELS_FILES_VIEW_PANEL_H_INCLUDED
#define CHANNELS_FILES_VIEW_PANEL_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace xray::editor::controls;

namespace xray {
namespace animation_editor {
	public ref class channels_files_view_panel : public xray::editor::controls::file_view_panel_base
	{
		typedef xray::editor::controls::file_view_panel_base super;
	public:
		channels_files_view_panel(xray::editor::controls::document_editor_base^ de)
			:xray::editor::controls::file_view_panel_base(de)
		{
			in_constructor();
			HideOnClose = true;
		}
		virtual	void	track_active_item				(System::String^ path) override;

	protected:
						~channels_files_view_panel		()	{};
				void	in_constructor					();
		virtual	void	on_node_double_click			(System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e) override;
		virtual	void	on_node_key_down				(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e) override;

	private:
		System::Windows::Forms::ImageList^ tree_view_image_list;
		System::Windows::Forms::ContextMenuStrip^	m_context_menu;
	}; // ref class channels_files_view_panel
} // namespace animation_editor
} // namespace xray
#endif // #ifndef CHANNELS_FILES_VIEW_PANEL_H_INCLUDED