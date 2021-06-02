////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_FILES_VIEW_PANEL_H_INCLUDED
#define DIALOG_FILES_VIEW_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace xray::editor::controls;

namespace xray {
namespace dialog_editor {
	ref class dialog_editor_impl;

	public ref class dialog_files_view_panel :  public xray::editor::controls::file_view_panel_base
	{
		typedef xray::editor::controls::file_view_panel_base super;
		System::Windows::Forms::ImageList^ tree_view_image_list;
		xray::editor::controls::tree_node^ m_copied_node;
		dialog_editor_impl^ m_dialog_editor;
		bool m_is_creating_node;
		System::Windows::Forms::ContextMenuStrip^	m_context_menu;
		System::Windows::Forms::ToolStripMenuItem^	m_remove_item;

	public:
						dialog_files_view_panel			(dialog_editor_impl^ de);
		virtual	void	track_active_item				(System::String^ path) override;
		virtual	void	remove_node						(System::Object^ sender, xray::editor::tree_view_event_args^ e) override;
		virtual	void	copy							(bool del) override;
		virtual	void	paste							() override;

		property xray::editor::controls::tree_node^ copied_node
		{
				void	set								(xray::editor::controls::tree_node^ n)	{m_copied_node=n;};
				xray::editor::controls::tree_node^ 	get	()										{return m_copied_node;};
		};

	protected:
						~dialog_files_view_panel		()	{};
		virtual	void	on_node_double_click			(System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e) override;
		virtual	void	on_node_key_down				(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e) override;
		virtual	void	add_new_folder					(System::Object^ sender, xray::editor::tree_view_event_args^ e) override;
		virtual	void	add_new_file					(System::Object^ sender, xray::editor::tree_view_event_args^ e) override;
		virtual	void	on_node_name_edit				(System::Object^ sender, System::Windows::Forms::NodeLabelEditEventArgs^ e) override;
		virtual	void	on_node_drag_over				(System::Object^ sender, System::Windows::Forms::DragEventArgs^ e) override;
		virtual	void	on_node_drag_drop				(System::Object^ sender, System::Windows::Forms::DragEventArgs^ e) override;
		virtual	void	on_tree_mouse_down				(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) override;
		virtual	void	on_tree_mouse_move				(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) override;

	private:
				bool	copy_node_impl					(xray::editor::controls::tree_node^ s, xray::editor::controls::tree_node^ d, bool del_old);
				void	remove_item_click				(System::Object^, System::EventArgs^);
	}; // ref class dialog_files_view_panel
} // namespace dialog_editor
} // namespace xray
#endif // #ifndef DIALOG_FILES_VIEW_PANEL_H_INCLUDED