////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_files_view_panel.h"
#include "animation_files_source.h"
#include "animation_collections_editor.h"
#include "animation_collection_item.h"

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

animation_files_view_panel::animation_files_view_panel( animation_collections_editor^ parent )
	:xray::editor::controls::file_view_panel_base( parent->multidocument_base, "Single Animations" )
{
	m_parent		= parent;
	in_constructor	( );
	HideOnClose		= true;
}

void animation_files_view_panel::in_constructor()
{
	xray::editor_base::image_loader	loader;
	m_tree_view_image_list = loader.get_shared_images16();
	m_tree_view_image_list ->TransparentColor = System::Drawing::Color::Transparent;

	m_context_menu = gcnew System::Windows::Forms::ContextMenuStrip();
	m_raw_files_tree_view->ContextMenuStrip = m_context_menu;
	m_raw_files_tree_view->HideSelection = false;
	m_raw_files_tree_view->ImageList = m_tree_view_image_list;
	m_raw_files_tree_view->source = gcnew animation_files_source( );
	m_raw_files_tree_view->refresh( );
	extension = animation_collection_item::extension;
	HideOnClose = true;
}

void animation_files_view_panel::on_node_double_click(System::Object^ , System::Windows::Forms::TreeNodeMouseClickEventArgs^ )
{
	//if( safe_cast<tree_node^>(e->Node)->m_node_type == tree_node_type::single_item )
	//{
	//	System::String^ full_path = e->Node->FullPath->Remove(0,11);
	//	m_editor->load_document( full_path );
	//}
}

void animation_files_view_panel::on_node_key_down( System::Object^, System::Windows::Forms::KeyEventArgs^  )
{
	//if( e->KeyCode == Keys::Return)
	//{
	//	TreeNode^ n = m_raw_files_tree_view->SelectedNode;
	//	if( n->Nodes->Count == 0 )
	//	{
	//		System::String^ full_path = n->FullPath->Remove( 0, 11 );
	//		m_editor->load_document( full_path );
	//	}
	//}
}

void animation_files_view_panel::track_active_item( System::String^ path )
{
	m_raw_files_tree_view->track_active_node( "animations/" + path );
}

void animation_files_view_panel::on_tree_mouse_down( Object^, MouseEventArgs^ e )
{
	m_drag_node = safe_cast<tree_node^>( m_raw_files_tree_view->GetNodeAt( e->X, e->Y ) );
	if( !m_drag_node || m_drag_node->m_node_type != tree_node_type::single_item )
		m_parent->clear_properties( );
}

void animation_files_view_panel::on_tree_mouse_move( Object^, MouseEventArgs^ e )
{
	u32 count = m_raw_files_tree_view->selected_nodes->Count;
	if( count > 0 && m_drag_node != nullptr )
	{
		if( ( e->Button & ::MouseButtons::Left ) == ::MouseButtons::Left )
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
			data->SetData( "animation_item_type", animation_item_type::animation_single_item );
			if( ( ModifierKeys & Keys::Control ) == Keys::Control )
				m_raw_files_tree_view->DoDragDrop( data, DragDropEffects::Copy );
			else
				m_raw_files_tree_view->DoDragDrop( data, DragDropEffects::Move );

			GCHandle::FromIntPtr( IntPtr( handle ) ).Free( );
		}	
	}
}

} // namespace animation_editor
} // namespace xray