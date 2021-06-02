////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_COLLECTIONS_VIEW_PANEL_H_INCLUDED
#define ANIMATION_COLLECTIONS_VIEW_PANEL_H_INCLUDED


using namespace System;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using System::Collections::ObjectModel::ReadOnlyCollection;
using System::ComponentModel::CancelEventArgs;

using xray::editor::controls::tree_node;

namespace xray
{
	namespace animation_editor
	{
		ref class animation_collections_editor;

		public ref class animation_collections_view_panel : public xray::editor::controls::file_view_panel_base
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
			animation_collections_view_panel( animation_collections_editor^ parent );
			virtual	void	track_active_item				( String^ path ) override;
					void	track_active_items				( List<String^>^ paths );

		protected:
			~animation_collections_view_panel( ){}
					void	in_constructor					( );
			virtual	void	on_node_double_click			( Object^ sender, TreeNodeMouseClickEventArgs^ e) override;
			virtual	void	on_node_key_down				( Object^ sender, KeyEventArgs^ e) override;
			virtual	void	on_node_name_edit				( Object^, NodeLabelEditEventArgs^ e) override;
			virtual	void	on_node_drag_over				( Object^, DragEventArgs^ e) override;
			virtual	void	on_node_drag_drop				( Object^, DragEventArgs^ e) override;
			virtual	void	on_tree_mouse_down				( Object^, MouseEventArgs^ e) override;
			virtual	void	on_tree_mouse_move				( Object^, MouseEventArgs^ e) override;

		private:
					ReadOnlyCollection<tree_node^>^ get_selected_nodes( );
					bool	is_selected_node				( String^ path );
					void	selection_changed				( Object^, editor::controls::tree_view_selection_event_args^ );
					void	menu_opening					( Object^, CancelEventArgs^ );
					void	new_file_click					( Object^, EventArgs^ );
					void	new_folder_click				( Object^, EventArgs^ );
					void	apply_changes_click				( Object^, EventArgs^ );
					void	revert_changes_click			( Object^, EventArgs^ );
					void	add_to_collection_click			( Object^, EventArgs^ );
					void	cut_click						( Object^, EventArgs^ );
					void	copy_click						( Object^, EventArgs^ );
					void	paste_click						( Object^, EventArgs^ );
					void	remove_click					( Object^, EventArgs^ );
					void	rename_click					( Object^, EventArgs^ );
					void	copy_nodes_impl					( tree_node^ source_node, tree_node^ destination_node, bool delete_old );
					void	remove_collections_tree			( tree_node^ root_node );
					void	do_fs_operation					( fs_operation_type t, String^ new_node_path, String^ old_node_path );

		private:
			ImageList^									m_tree_view_image_list;
			tree_node^									m_copied_node;
			animation_collections_editor^				m_parent;
			System::Windows::Forms::ContextMenuStrip^	m_context_menu;
			Boolean										m_is_cutting_node;
			ToolStripMenuItem^							addToolStripMenuItem;
			ToolStripMenuItem^							newFileToolStripMenuItem;
			ToolStripMenuItem^							newFolderToolStripMenuItem;
			ToolStripMenuItem^							applyChangesToolStripMenuItem;
			ToolStripMenuItem^							revertChangesToolStripMenuItem;
			ToolStripMenuItem^							addToCollectionToolStripMenuItem;
			ToolStripSeparator^							toolStripSeparator1;
			ToolStripMenuItem^							cutToolStripMenuItem;
			ToolStripMenuItem^							copyToolStripMenuItem;
			ToolStripMenuItem^							pasteToolStripMenuItem;
			ToolStripMenuItem^							removeItemToolStripMenuItem;
			ToolStripMenuItem^							renameItemToolStripMenuItem;

		};// class animation_collections_view_panel
	} //namespace animation_editor
} //namespace xray

#endif // #ifndef ANIMATION_COLLECTIONS_VIEW_PANEL_H_INCLUDED#pragma once