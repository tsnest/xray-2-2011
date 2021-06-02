////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_collections_view_panel.h"
#include "animation_collection_files_source.h"
#include "animation_collections_document_base.h"
#include "animation_collections_editor.h"
#include "animation_collection_item.h"
#include "animation_editor.h"
#include "animation_collection_document.h"

using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using System::Windows::Forms::TreeNode;
using xray::editor::controls::tree_node;
using xray::editor::controls::tree_node_type;
using System::Runtime::InteropServices::GCHandle;

namespace xray{
namespace animation_editor{

animation_collections_view_panel::animation_collections_view_panel( animation_collections_editor^ parent )
	:xray::editor::controls::file_view_panel_base( parent->multidocument_base, "Animation Collections" ),
	m_parent  ( parent )
{
	in_constructor	( );
	HideOnClose		= true;
}

void animation_collections_view_panel::in_constructor()
{
	m_is_cutting_node = false;
	m_context_menu = gcnew System::Windows::Forms::ContextMenuStrip();
	addToolStripMenuItem = gcnew ToolStripMenuItem();
	newFileToolStripMenuItem = gcnew ToolStripMenuItem();
	newFolderToolStripMenuItem = gcnew ToolStripMenuItem();
	applyChangesToolStripMenuItem = gcnew ToolStripMenuItem();
	revertChangesToolStripMenuItem = gcnew ToolStripMenuItem();
	addToCollectionToolStripMenuItem = gcnew ToolStripMenuItem();
	toolStripSeparator1 = gcnew ToolStripSeparator();
	cutToolStripMenuItem = gcnew ToolStripMenuItem();
	copyToolStripMenuItem = gcnew ToolStripMenuItem();
	pasteToolStripMenuItem = gcnew ToolStripMenuItem();
	removeItemToolStripMenuItem = gcnew ToolStripMenuItem();
	renameItemToolStripMenuItem = gcnew ToolStripMenuItem();

	SuspendLayout( );
	m_context_menu->SuspendLayout( );

	m_context_menu->Items->AddRange(gcnew cli::array<System::Windows::Forms::ToolStripItem^>(7) {
		addToolStripMenuItem, 
		//applyChangesToolStripMenuItem, 
		//revertChangesToolStripMenuItem,
		//addToCollectionToolStripMenuItem,
		toolStripSeparator1,
		cutToolStripMenuItem,
		copyToolStripMenuItem,
		pasteToolStripMenuItem,
		renameItemToolStripMenuItem,
		removeItemToolStripMenuItem,
	});
	m_context_menu->Name = L"m_context_menu";
	m_context_menu->Size = System::Drawing::Size( 153, 92 );
	m_context_menu->Opening += gcnew CancelEventHandler( this, &animation_collections_view_panel::menu_opening );

	// addToolStripMenuItem
	addToolStripMenuItem->Name = L"addToolStripMenuItem";
	addToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	addToolStripMenuItem->Text = L"Add";
	addToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array<System::Windows::Forms::ToolStripItem^>(2)
		{newFileToolStripMenuItem, newFolderToolStripMenuItem});

	// newFileToolStripMenuItem
	newFileToolStripMenuItem->Name = L"newFileToolStripMenuItem";
	newFileToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	newFileToolStripMenuItem->Text = L"New Collection";
	newFileToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_collections_view_panel::new_file_click);

	// newFolderToolStripMenuItem
	newFolderToolStripMenuItem->Name = L"newFolderToolStripMenuItem";
	newFolderToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	newFolderToolStripMenuItem->Text = L"New Directory";
	newFolderToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_collections_view_panel::new_folder_click);

	// applyChangesToolStripMenuItem
	//applyChangesToolStripMenuItem->Name = L"applyChangesToolStripMenuItem";
	//applyChangesToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	//applyChangesToolStripMenuItem->Text = L"Apply changes";
	//applyChangesToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_collections_view_panel::apply_changes_click);

	//// revertChangesToolStripMenuItem
	//revertChangesToolStripMenuItem->Name = L"revertChangesToolStripMenuItem";
	//revertChangesToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	//revertChangesToolStripMenuItem->Text = L"Revert changes";
	//revertChangesToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_collections_view_panel::revert_changes_click);

	// addToCollectionToolStripMenuItem
	//addToCollectionToolStripMenuItem->Name = L"addToCollectionToolStripMenuItem";
	//addToCollectionToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	//addToCollectionToolStripMenuItem->Text = L"Add to collection";
	//addToCollectionToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_collections_view_panel::add_to_collection_click);

	// toolStripSeparator1
	toolStripSeparator1->Name = L"toolStripSeparator1";
	toolStripSeparator1->Size = System::Drawing::Size(89, 6);

	// cutToolStripMenuItem
	cutToolStripMenuItem->Name = L"cutToolStripMenuItem";
	cutToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	cutToolStripMenuItem->Text = L"Cut";
	cutToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_collections_view_panel::cut_click);

	// copyToolStripMenuItem
	copyToolStripMenuItem->Name = L"copyToolStripMenuItem";
	copyToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	copyToolStripMenuItem->Text = L"Copy";
	copyToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_collections_view_panel::copy_click);

	// pasteToolStripMenuItem
	pasteToolStripMenuItem->Name = L"pasteToolStripMenuItem";
	pasteToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	pasteToolStripMenuItem->Text = L"Paste";
	pasteToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_collections_view_panel::paste_click);

	// removeItemToolStripMenuItem
	removeItemToolStripMenuItem->Name = L"removeItemToolStripMenuItem";
	removeItemToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	removeItemToolStripMenuItem->Text = L"Remove item";
	removeItemToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_collections_view_panel::remove_click);

	// renameItemToolStripMenuItem
	renameItemToolStripMenuItem->Name = L"renameItemToolStripMenuItem";
	renameItemToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	renameItemToolStripMenuItem->Text = L"Rename item";
	renameItemToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_collections_view_panel::rename_click);
	
	m_context_menu->ResumeLayout(false);

	xray::editor_base::image_loader	loader;
	m_tree_view_image_list = loader.get_shared_images16( );
	m_tree_view_image_list ->TransparentColor = System::Drawing::Color::Transparent;

	m_raw_files_tree_view->ContextMenuStrip = m_context_menu;
	m_raw_files_tree_view->ImageList = m_tree_view_image_list;
	m_raw_files_tree_view->HideSelection = false;
	m_raw_files_tree_view->is_multiselect = true;
	m_raw_files_tree_view->source = gcnew animation_collection_files_source( );
	m_raw_files_tree_view->refresh();
	m_raw_files_tree_view->Dock = DockStyle::Fill;
	m_raw_files_tree_view->selected_items_changed += gcnew EventHandler<xray::editor::controls::tree_view_selection_event_args^>(this, &animation_collections_view_panel::selection_changed);
	extension		= animation_collection_item::extension;

	AllowDrop		= true;
	HideOnClose		= true;
	ResumeLayout	( false );
}

void animation_collections_view_panel::on_node_double_click(System::Object^ , System::Windows::Forms::TreeNodeMouseClickEventArgs^ e)
{
	if( safe_cast<tree_node^>(e->Node)->m_node_type == tree_node_type::single_item )
	{
		String^ full_path = e->Node->FullPath;//->Remove( 0, ( (String^) "animations/" )->Length );
		m_editor->load_document( full_path );
	}
}

void animation_collections_view_panel::on_node_key_down(System::Object^, System::Windows::Forms::KeyEventArgs^ e)
{
	if(e->KeyCode==Keys::Return)
	{
		TreeNode^ n = m_raw_files_tree_view->SelectedNode;
		if(n->Nodes->Count==0)
		{
			System::String^ full_path = n->FullPath->Remove(0,11);
			m_editor->load_document(full_path);
		}
	}
}

void animation_collections_view_panel::track_active_item( String^ path )
{
	m_raw_files_tree_view->track_active_node( path );
}

void animation_collections_view_panel::track_active_items( List<String^>^ paths )
{
	m_raw_files_tree_view->track_active_nodes( paths );
	//safe_cast<animation_collection_files_source^>(m_raw_files_tree_view->source)->do_not_refresh = true;
}

void animation_collections_view_panel::on_tree_mouse_down(Object^, MouseEventArgs^ e)
{
	m_drag_node = safe_cast<tree_node^>( m_raw_files_tree_view->GetNodeAt( e->X, e->Y ) );
	if( !m_drag_node || m_drag_node->m_node_type != tree_node_type::single_item )
		m_parent->clear_properties( );
}

void animation_collections_view_panel::selection_changed(Object^, xray::editor::controls::tree_view_selection_event_args^)
{
	if( m_raw_files_tree_view->selected_nodes->Count == 0 )
		return m_parent->clear_properties( );

	List<animation_item^>^ list = gcnew List<animation_item^>( );
	for each( tree_node^ node in m_raw_files_tree_view->selected_nodes )
	{
		if( node->m_node_type == tree_node_type::group_item )
			continue;

		animation_collection_item^ item = m_parent->get_collection( node->FullPath );
		if( item->is_loaded )
			list->Add( item );
	}

	if( list->Count == 0 )
	{
		m_parent->clear_properties( );
		return;
	}

	array<animation_item^>^ arr		= gcnew array<animation_item^>( list->Count );
	list->CopyTo					( arr );
	m_parent->show_properties		( arr, true );
}

void animation_collections_view_panel::on_node_name_edit( Object^, NodeLabelEditEventArgs^ e )
{
	String^ new_name = e->Label;
	tree_node^ n = safe_cast<tree_node^>( e->Node );
	if( new_name == nullptr || new_name == n->Text )
	{
		e->CancelEdit = true;
		return;
	}

	TreeNodeCollection^ col;
	if( n->Parent )
		col = n->nodes;
	else
		col = m_raw_files_tree_view->nodes;

	if( col->ContainsKey( new_name ) )
	{
		MessageBox::Show( this, "Collection or folder named [" + new_name + "] already exists!", 
						"Sound editor", MessageBoxButtons::OK, MessageBoxIcon::Error );
		e->CancelEdit = true;
		return;
	}

	if( new_name->Contains( "." ) )
	{
		e->CancelEdit = true;
		new_name = new_name->Replace( ".", "_" );
		MessageBox::Show(this, "Name contains '.'!! Replacing with '_'", 
			"Animation editor", MessageBoxButtons::OK, MessageBoxIcon::Warning );
	}
	if(new_name->Contains("/") || new_name->Contains("\\"))
	{
		e->CancelEdit = true;
		new_name = new_name->Replace("\\", "_");
		new_name = new_name->Replace("/", "_");
		MessageBox::Show(this, "Name contains '/'!! Replacing with '_'", 
			"Animation editor", MessageBoxButtons::OK, MessageBoxIcon::Warning);
	}

	String^ old_node_path = n->FullPath;
	String^ new_node_path = "";
	int index = old_node_path->LastIndexOf("/");
	if(index>0)
		new_node_path += old_node_path->Remove(index + 1);

	new_node_path += new_name;
	//if(m_editor->is_opened(old_node_path))
	//{
	//	MessageBox::Show(this, "Collection ["+old_node_path+"] is opened. Please, close it before modifying name.", 
	//						"Animation editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
	//	e->CancelEdit = true;
	//	return;
	//}

	n->Text = new_name;
	n->Name = new_name;
	do_fs_operation((n->m_node_type==tree_node_type::single_item) ? 
			fs_operation_type::file_rename : 
			fs_operation_type::folder_rename, 
		new_node_path, 
		old_node_path
	);
}

void animation_collections_view_panel::on_node_drag_over(Object^, DragEventArgs^ e)
{
	if(!e->Data->GetDataPresent("tree_view_item"))
	{
		e->Effect = DragDropEffects::None;
		return;
	}
	//if dragging with control - copy
	if((ModifierKeys & Keys::Control) == Keys::Control)
		e->Effect = DragDropEffects::Copy;
	else
		e->Effect = DragDropEffects::Move;
}

void animation_collections_view_panel::on_node_drag_drop(Object^, DragEventArgs^ e)
{
	if(e->Data->GetDataPresent("tree_view_item"))
	{
		System::Drawing::Point node_position = m_raw_files_tree_view->PointToClient(System::Drawing::Point(e->X, e->Y));
		tree_node^ dest_node = safe_cast<tree_node^>(m_raw_files_tree_view->GetNodeAt(node_position));
		List<String^>^ lst = safe_cast<List<String^>^>(e->Data->GetData("tree_view_item"));
		List<tree_node^>^ drag_nodes = gcnew List<tree_node^>();
		for each(String^ drag_node_name in lst)
		{
			tree_node^ drag_node = m_raw_files_tree_view->get_node(drag_node_name);
			R_ASSERT(drag_node!=nullptr);

			//sound_object_type operation = (sound_object_type)safe_cast<u32>(e->Data->GetData("mode"));;
			//if(drag_node->m_node_type==tree_node_type::single_item && 
			//	!(m_parent->get_sound(drag_node_name, operation)->is_loaded))
			//{
			//	MessageBox::Show(this, "Sound ["+drag_node_name+"] is loading. Please, wait...", 
			//						"Sound editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
			//	e->Effect = DragDropEffects::None;
			//	return;
			//}

			TreeNodeCollection^ col;
			if(dest_node)
				col = dest_node->nodes;
			else
				col = m_raw_files_tree_view->nodes;

			if(dest_node==drag_node || col->Contains(drag_node))
			{
				e->Effect = DragDropEffects::None;
				return;
			}

			drag_nodes->Add(drag_node);
		}

		for each(tree_node^ drag_node in drag_nodes)
		{
			if(e->Effect==DragDropEffects::Move || e->Effect==DragDropEffects::Copy)
				copy_nodes_impl(drag_node, dest_node, e->Effect==DragDropEffects::Move);
		}
	}
	else
		e->Effect = DragDropEffects::None;
}

void animation_collections_view_panel::on_tree_mouse_move(Object^, MouseEventArgs^ e)
{
	u32 count = m_raw_files_tree_view->selected_nodes->Count;
	if( count > 0 && m_drag_node != nullptr )
	{
		if( ( e->Button & ::MouseButtons::Left) == ::MouseButtons::Left )
		{
			tree_node_type node_type = m_raw_files_tree_view->selected_nodes[0]->m_node_type;

			List<tree_node^>^ list = gcnew List<tree_node^>( );
			for( u32 i = 0; i < count; ++i )
			{
				tree_node^ n = m_raw_files_tree_view->selected_nodes[i];
				if( n->m_node_type == node_type )
					list->Add( n );
			}

			IntPtr handle		= GCHandle::ToIntPtr( GCHandle::Alloc( list ) );
			DataObject^ data	= gcnew DataObject( "nodes_list_handle", handle );

			animation_collections_editor_drag_drop_operation operation;
			if( node_type == tree_node_type::single_item )
				operation = animation_collections_editor_drag_drop_operation::tree_view_file;
			else
				operation = animation_collections_editor_drag_drop_operation::tree_view_folder;

			data->SetData( "item_type", node_type );
			data->SetData( "animation_item_type", animation_item_type::animation_collection_item );
			if( ( ModifierKeys & Keys::Control ) == Keys::Control )
				m_raw_files_tree_view->DoDragDrop( data, DragDropEffects::Copy );
			else
				m_raw_files_tree_view->DoDragDrop( data, DragDropEffects::Move );

			GCHandle::FromIntPtr( IntPtr( handle ) ).Free( );
		}
	}
}

void animation_collections_view_panel::menu_opening(Object^, CancelEventArgs^)
{
	addToolStripMenuItem->Visible					= true;
	newFileToolStripMenuItem->Visible				= true;
	newFolderToolStripMenuItem->Visible				= true;
	applyChangesToolStripMenuItem->Visible			= true;
	revertChangesToolStripMenuItem->Visible			= true;
	addToCollectionToolStripMenuItem->Visible		= true;
	cutToolStripMenuItem->Visible					= true;
	copyToolStripMenuItem->Visible					= true;
	removeItemToolStripMenuItem->Visible			= true;
	renameItemToolStripMenuItem->Visible			= true;
	pasteToolStripMenuItem->Enabled					= m_copied_node != nullptr;
	addToCollectionToolStripMenuItem->Enabled		= m_parent->active_document != nullptr && m_parent->active_document->collection_node( ) != nullptr;

	if( m_raw_files_tree_view->selected_nodes->Count > 0 )
	{
		if( m_raw_files_tree_view->selected_nodes[0]->m_node_type == tree_node_type::single_item )
			addToolStripMenuItem->Visible				= false;
		else
			addToCollectionToolStripMenuItem->Visible	= false;
	}
	else
	{
		applyChangesToolStripMenuItem->Visible		= false;
		revertChangesToolStripMenuItem->Visible		= false;
		addToCollectionToolStripMenuItem->Visible	= false;
		cutToolStripMenuItem->Visible				= false;
		copyToolStripMenuItem->Visible				= false;
		removeItemToolStripMenuItem->Visible		= false;
		renameItemToolStripMenuItem->Visible		= false;
	}
}

void animation_collections_view_panel::new_file_click(Object^, EventArgs^)
{
	int nodes_count = m_raw_files_tree_view->selected_nodes->Count;
	if( nodes_count > 0 && m_raw_files_tree_view->selected_nodes[0]->m_node_type == tree_node_type::single_item )
		return;
	
	String^ node_path = (nodes_count>0) ? m_raw_files_tree_view->selected_nodes[0]->FullPath + "/new_collection" : "new_collection";
	int i = 1;
	String^ absolute_path = animation_editor::animation_collections_absolute_path + "/";
	String^ file_path = absolute_path + node_path + i + extension;
	while( System::IO::File::Exists( file_path ) || m_parent->find_collection( node_path + i ) != nullptr )
	{
		i++;
		file_path = absolute_path + node_path + i + extension;
	}

	node_path += i;
	tree_node^ n = m_raw_files_tree_view->add_item(node_path, editor_base::node_resource);
	do_fs_operation(fs_operation_type::file_create, node_path, nullptr);
	n->BeginEdit();
}

void animation_collections_view_panel::new_folder_click(Object^, EventArgs^)
{
	int nodes_count = m_raw_files_tree_view->selected_nodes->Count;
	if(nodes_count>0 && m_raw_files_tree_view->selected_nodes[0]->m_node_type==tree_node_type::single_item)
		return;
	
	String^ node_path		= ( nodes_count > 0 ) ? m_raw_files_tree_view->selected_nodes[0]->FullPath + "/new_folder" : "new_folder";
	String^ absolute_path	= animation_editor::animation_collections_absolute_path;
	String^ folder_path		= absolute_path + node_path;
	int i = 1;
	while( System::IO::Directory::Exists( folder_path + i ) )
		i++;

	node_path		+= i;
	tree_node^ n	= m_raw_files_tree_view->add_group( node_path, editor_base::folder_closed, editor_base::folder_open );
	do_fs_operation ( fs_operation_type::folder_create, node_path, nullptr );
	n->BeginEdit	( );
}

void animation_collections_view_panel::apply_changes_click( Object^, EventArgs^ )
{
	/*List<String^>^ list = gcnew List<String^>();
	for each(tree_node^ n in m_raw_files_tree_view->selected_nodes)
		list->Add(n->FullPath);

	m_parent->apply_changes(list);*/
}

void animation_collections_view_panel::revert_changes_click(Object^, EventArgs^)
{
	/*List<String^>^ list = gcnew List<String^>();
	for each(tree_node^ n in m_raw_files_tree_view->selected_nodes)
		list->Add(n->FullPath);

	m_parent->revert_changes(list);*/
}

void animation_collections_view_panel::add_to_collection_click(Object^, EventArgs^)
{
	List<String^>^ list = gcnew List<String^>();
	for each( tree_node^ n in m_raw_files_tree_view->selected_nodes )
		list->Add( n->FullPath );

	m_parent->active_document->insert_items_into_collection( list, animation_item_type::animation_collection_item );
}

void animation_collections_view_panel::cut_click(Object^, EventArgs^)
{
	if(m_raw_files_tree_view->selected_nodes->Count==0)
		return;

	m_copied_node = m_raw_files_tree_view->selected_nodes[0];
	m_is_cutting_node = true;
}

void animation_collections_view_panel::copy_click(Object^, EventArgs^)
{
	if(m_raw_files_tree_view->selected_nodes->Count==0)
		return;

	m_copied_node = m_raw_files_tree_view->selected_nodes[0];
}

void animation_collections_view_panel::paste_click(Object^, EventArgs^)
{
	if( m_copied_node == nullptr )
		return;

	tree_node^ destination_node = ( m_raw_files_tree_view->selected_nodes->Count == 0 ) ? 
		m_raw_files_tree_view->root : 
		m_raw_files_tree_view->selected_nodes[0];
	
	copy_nodes_impl( m_copied_node, destination_node, m_is_cutting_node );
}

void animation_collections_view_panel::remove_click(Object^, EventArgs^)
{
	if(m_raw_files_tree_view->selected_nodes->Count==0)
		return;

	if(m_raw_files_tree_view->selected_nodes->Contains(m_copied_node))
		m_copied_node = nullptr;

	for each(tree_node^ n in m_raw_files_tree_view->selected_nodes)
	{
		String^ node_path = n->FullPath;
		if(m_editor->is_opened(node_path))
			continue;

		do_fs_operation((n->m_node_type==tree_node_type::single_item) ? 
				fs_operation_type::file_remove : 
				fs_operation_type::folder_remove, 
			nullptr, 
			node_path
		);
		m_raw_files_tree_view->remove_item(node_path);
	}
	m_raw_files_tree_view->deselect_all( );
}

void animation_collections_view_panel::rename_click(Object^, EventArgs^)
{
	if(m_raw_files_tree_view->selected_nodes->Count==0)
		return;

	m_raw_files_tree_view->selected_nodes[0]->BeginEdit();
}

void animation_collections_view_panel::copy_nodes_impl( tree_node^ source_node, tree_node^ destination_node, bool delete_old )
{
	R_ASSERT(source_node!=nullptr);
	if(destination_node!=nullptr && destination_node->m_node_type == tree_node_type::single_item)
		destination_node = destination_node->Parent;

	String^ absolute_path		= animation_editor::animation_collections_absolute_path;
	String^ source_node_path	= source_node->FullPath;
	String^ dest_node_path		= ( destination_node != nullptr ) ? ( destination_node->FullPath + "/" + source_node->Name ) : source_node->Name;
	String^ dest_file_path		= absolute_path + "/" + dest_node_path;

	for( Int32 sufix = 0; ; ++sufix )
	{
		String^ dest_node_path_fixed = dest_node_path;

		if( sufix != 0 )
			dest_node_path_fixed = dest_node_path + sufix;

		if( File::Exists( absolute_path + "/" + dest_node_path_fixed + extension ) || Directory::Exists( absolute_path + "/" + dest_node_path_fixed ) )
			continue;

		dest_node_path	= dest_node_path_fixed;
		dest_file_path	= absolute_path + "/" + dest_node_path_fixed;
		break;
	}
	
	if( destination_node != nullptr && check_parent( destination_node->FullPath, source_node->Name ) )
	{
		MessageBox::Show( this, "Can't copy ["+source_node_path+"] into ["+dest_node_path+"].", 
			"Animation editor", MessageBoxButtons::OK, MessageBoxIcon::Error );
		return;
	}

	if( delete_old )
	{
		do_fs_operation( ( source_node->m_node_type == tree_node_type::single_item ) ? 
				fs_operation_type::file_rename :
				fs_operation_type::folder_rename, 
			dest_node_path, 
			source_node_path
		);
		m_raw_files_tree_view->remove_item( source_node_path );
	}
	else
	{
		do_fs_operation( ( source_node->m_node_type == tree_node_type::single_item ) ?
				fs_operation_type::file_copy :
				fs_operation_type::folder_copy, 
			dest_node_path, 
			source_node_path
		);
	}

	if( source_node->m_node_type == tree_node_type::single_item )
		m_raw_files_tree_view->add_item( dest_node_path, editor_base::node_resource );
	else
		add_folders_and_files( source_node->nodes, dest_node_path, editor_base::folder_closed, editor_base::folder_open, editor_base::node_resource );

	m_copied_node				= nullptr;
	m_is_cutting_node			= false;
	m_raw_files_tree_view->Sort	( );
	track_active_item			( dest_node_path );
}

void animation_collections_view_panel::remove_collections_tree( tree_node^ node )
{
	R_ASSERT( node != nullptr );
	if( node->m_node_type == tree_node_type::single_item )
	{
		m_parent->remove_collection( node->FullPath );
	}
	else
	{
		for each( tree_node^ sub_node in node->nodes )
			remove_collections_tree( sub_node );
	}
}

void animation_collections_view_panel::do_fs_operation( animation_collections_view_panel::fs_operation_type t, String^ new_node_path, String^ old_node_path )
{
	if( new_node_path != nullptr )
		new_node_path = new_node_path->ToLower( );

	if( old_node_path != nullptr )
		old_node_path = old_node_path->ToLower( );

	String^ new_fs_item_path	= animation_editor::animation_collections_absolute_path + "/" + new_node_path;
	String^ old_fs_item_path	= animation_editor::animation_collections_absolute_path + "/" + old_node_path;
	try
	{
		switch( t )
		{
		case fs_operation_type::file_create:
			{
				m_parent->new_collection	( new_node_path );
				track_active_item			( new_node_path );
				break;
			}
		case fs_operation_type::file_rename:
			{
				m_parent->rename_collection	( old_node_path, new_node_path );
				animation_collection_document^ doc = safe_cast<animation_collection_document^>( m_editor->get_document( old_node_path ) );
				if( doc != nullptr )
				{
					doc->Name					= new_node_path;
					doc->update_tree_names		( );
					doc->Text					= doc->Name;

					/*u32 index = new_node_path->LastIndexOf( '/' );
						
						doc->Text					= new_node_path->Substring(  );*/
				}
				m_parent->update_properties	( );
				if( File::Exists( old_fs_item_path + extension ) )
					File::Move					( old_fs_item_path + extension, new_fs_item_path + extension );
				break;
			}
		case fs_operation_type::file_remove:
			{
				m_parent->remove_collection	( old_node_path );
				File::Delete				( old_fs_item_path + extension );
				break;
			}
		case fs_operation_type::file_copy:
			{
				File::Copy					( old_fs_item_path + extension, new_fs_item_path + extension );
				break;
			}
		case fs_operation_type::folder_create:
			{
				Directory::CreateDirectory	( new_fs_item_path );
				track_active_item			( new_node_path );
				break;
			}
		case fs_operation_type::folder_rename:
			{
				remove_collections_tree		( m_raw_files_tree_view->get_node( new_node_path ) );
				Directory::Move				( old_fs_item_path, new_fs_item_path );
				break;
			}
		case fs_operation_type::folder_remove:
			{
				remove_collections_tree		( m_raw_files_tree_view->get_node( old_node_path ) );
				Directory::Delete			( old_fs_item_path, true );
				break;
			}
		case fs_operation_type::folder_copy:
			{
				DirectoryInfo^ di	= gcnew DirectoryInfo( old_node_path );
				iterate_folders		( di, new_fs_item_path );
				delete di;
				break;
			}
		default:
			NODEFAULT( );
		}
	}
	catch(IOException^ e)
	{
		R_ASSERT(false, "%s", e->ToString());
	}
}

ReadOnlyCollection<tree_node^>^ animation_collections_view_panel::get_selected_nodes()
{
	return m_raw_files_tree_view->selected_nodes;
}

bool animation_collections_view_panel::is_selected_node(String^ path)
{
	tree_node^ n = m_raw_files_tree_view->get_node( path );
	if( n == nullptr )
		return false;

	return m_raw_files_tree_view->selected_nodes->Contains( n );
}

} // namespace animation_editor
} // namespace xray