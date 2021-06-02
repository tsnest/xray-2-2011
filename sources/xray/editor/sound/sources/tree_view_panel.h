////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef TREE_VIEW_PANEL_H_INCLUDED
#define TREE_VIEW_PANEL_H_INCLUDED

#include "tree_view_files_source.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace System::Windows::Forms;
using xray::editor::tree_view_event_args;
using xray::editor::controls::tree_node;

namespace xray {
namespace sound_editor {

	ref class sound_editor;

	public ref class tree_view_panel: public xray::editor::controls::file_view_panel_base
	{
		typedef xray::editor::controls::file_view_panel_base super;
		enum class fs_operation_type
		{
			file_create		= 1,
			file_rename		= 2,
			file_remove		= 3,
			file_copy		= 4,
			folder_create	= 5,
			folder_rename	= 6,
			folder_remove	= 7,
			folder_copy		= 8
		};
	public:
						tree_view_panel			(sound_editor^ parent);
						~tree_view_panel		() {};
		virtual	void	track_active_item		(String^ path) override;
				void	track_active_items		(List<String^>^ paths);
				void	switch_mode				(u32 index);
				bool	is_selected_node		(String^ path);
			tree_node^	get_node				(String^ path) {return m_raw_files_tree_view->get_node(path);};
		ReadOnlyCollection<tree_node^>^	get_selected_nodes();

	protected:
		virtual	void	on_node_double_click	(Object^, TreeNodeMouseClickEventArgs^ e) override;
		virtual	void	on_node_name_edit		(Object^, NodeLabelEditEventArgs^ e) override;
		virtual	void	on_node_drag_over		(Object^, DragEventArgs^ e) override;
		virtual	void	on_node_drag_drop		(Object^, DragEventArgs^ e) override;
		virtual	void	on_tree_mouse_down		(Object^, MouseEventArgs^ e) override;
		virtual	void	on_tree_mouse_move		(Object^, MouseEventArgs^ e) override;

	private:
				void	selection_changed		(Object^, editor::controls::tree_view_selection_event_args^);
				void	on_tab_changed			(Object^, TabControlEventArgs^ e);
				void	menu_opened				(Object^, EventArgs^);
				void	new_file_click			(Object^, EventArgs^);
				void	new_folder_click		(Object^, EventArgs^);
				void	apply_changes_click		(Object^, EventArgs^);
				void	revert_changes_click	(Object^, EventArgs^);
				void	add_to_collection_click	(Object^, EventArgs^);
				void	cut_click				(Object^, EventArgs^);
				void	copy_click				(Object^, EventArgs^);
				void	paste_click				(Object^, EventArgs^);
				void	remove_click			(Object^, EventArgs^);
				void	rename_click			(Object^, EventArgs^);
				void	copy_nodes_impl			(tree_node^ s, tree_node^ d, bool del_old);
				String^	get_absolute_path		();
				void	remove_parent_sounds	(tree_node^ s);
				void	do_fs_operation			(fs_operation_type t, String^ new_node_path, String^ old_node_path);

		tree_view_files_source_mode	get_mode	();

	private:
		System::Windows::Forms::ImageList^	m_tree_view_image_list;
		sound_editor^						m_parent;
		tree_node^							m_copied_node;
		bool								m_is_cutting_node;
		TabControl^							tabControl1;
		TabPage^  							tabPage1;
		TabPage^  							tabPage2;
		TabPage^  							tabPage3;
		TabPage^  							tabPage4;
		System::Windows::Forms::ContextMenuStrip^ m_context_menu;
		ToolStripMenuItem^					addToolStripMenuItem;
		ToolStripMenuItem^					newFileToolStripMenuItem;
		ToolStripMenuItem^					newFolderToolStripMenuItem;
		ToolStripMenuItem^					applyChangesToolStripMenuItem;
		ToolStripMenuItem^					revertChangesToolStripMenuItem;
		ToolStripMenuItem^					addToCollectionToolStripMenuItem;
		ToolStripSeparator^					toolStripSeparator1;
		ToolStripMenuItem^					cutToolStripMenuItem;
		ToolStripMenuItem^					copyToolStripMenuItem;
		ToolStripMenuItem^					pasteToolStripMenuItem;
		ToolStripMenuItem^					removeItemToolStripMenuItem;
		ToolStripMenuItem^					renameItemToolStripMenuItem;
	}; // class tree_view_panel
} // namespace sound_editor
} // namespace xray
#endif // #ifndef TREE_VIEW_PANEL_H_INCLUDED