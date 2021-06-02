////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_collection_files_source.h"
#include "animation_collections_editor.h"
#include "animation_collection_item.h"
#include "animation_editor.h"

using System::Windows::Forms::TreeNode;
using xray::editor::controls::tree_node;
using System::IO::Path;

namespace xray{
namespace animation_editor{

animation_collection_files_source::animation_collection_files_source( )
	: m_subscribe_id(0)
{
	m_callback = NEW(file_watcher_delegate)(
		gcnew file_watcher_delegate::Delegate(this, &animation_collection_files_source::fs_callback)
		);
	
	m_subscribe_id = xray::resources::subscribe_watcher(
		unmanaged_string( animation_editor::animation_collections_path ).c_str( ), 
		boost::bind(&file_watcher_delegate::callback, m_callback, _1)
		);
}

animation_collection_files_source::~animation_collection_files_source( )
{
	xray::resources::unsubscribe_watcher( m_subscribe_id );
	DELETE( m_callback );
}

void animation_collection_files_source::fs_callback( xray::vfs::vfs_notification const & info )
{
	switch( info.type )
	{
		case xray::vfs::vfs_notification::type_added:
		{
			Boolean is_file		= resources::get_physical_path_info( info.physical_path ).is_file( );
			String^ path		= ( gcnew String( info.physical_path ) )->Replace( Path::GetFullPath( animation_editor::animation_collections_absolute_path ) + "\\", "" )->Replace( animation_collection_item::extension, "" )->Replace( "\\", "/" );
			tree_node^ node		= m_parent->get_node( path );

			if( node == nullptr )
			{
				if( is_file )
					m_parent->add_item	( path, editor_base::node_resource );
				else
					m_parent->add_group	( path, editor_base::folder_closed, editor_base::folder_open );
			}

			break;
		}
		case xray::vfs::vfs_notification::type_removed:
		{
			String^ path		= ( gcnew String( info.physical_path ) )->Replace( Path::GetFullPath( animation_editor::animation_collections_absolute_path ) + "\\", "" )->Replace( animation_collection_item::extension, "" )->Replace( "\\", "/" );
			tree_node^ node		= m_parent->get_node( path );

			if( node )
				node->Remove( );

			break;
		}
		case xray::vfs::vfs_notification::type_renamed:
		{
			String^ old_path	= ( gcnew String( info.physical_old_path ) )->Replace( Path::GetFullPath( animation_editor::animation_collections_absolute_path ) + "\\", "" )->Replace( animation_collection_item::extension, "" )->Replace( "\\", "/" );
			String^ new_path	= ( gcnew String( info.physical_new_path ) )->Replace( Path::GetFullPath( animation_editor::animation_collections_absolute_path ) + "\\", "" )->Replace( animation_collection_item::extension, "" )->Replace( "\\", "/" );
			tree_node^ node		= m_parent->get_node( old_path );

			if( node )
			{
				node->Name			= Path::GetFileName( new_path );
				node->Text			= node->Name;
			}
			
			break;
		}
	}
}

tree_view^ animation_collection_files_source::parent::get( )
{
	return m_parent;
}

void animation_collection_files_source::parent::set( tree_view^ value )
{
	m_parent = value;
}

void animation_collection_files_source::refresh( )
{
	fs_iterator_delegate* q = NEW( fs_iterator_delegate )( gcnew fs_iterator_delegate::Delegate( this, &animation_collection_files_source::on_fs_iterator_ready ), g_allocator );
	resources::query_vfs_iterator( unmanaged_string( animation_editor::animation_collections_path ).c_str( ), boost::bind( &fs_iterator_delegate::callback, q, _1 ), g_allocator, xray::resources::recursive_true );
}

void animation_collection_files_source::on_fs_iterator_ready( xray::vfs::vfs_locked_iterator const & fs_it )
{
	m_parent->nodes->Clear( );
	m_parent->deselect_all( );
	process_fs( fs_it.children_begin( ), "" );
	m_parent->Sort( );
}

void animation_collection_files_source::process_fs( xray::vfs::vfs_iterator const & fs_it, String^ file_path )
{
	xray::vfs::vfs_iterator	it	=	fs_it;
	while( !it.is_end( ) )
	{
		if( it.is_folder( ) )
		{
			String^ folder_name = gcnew String( it.get_name( ) );
			m_parent->add_group( file_path + folder_name, editor_base::folder_closed, editor_base::folder_open );
			process_fs( it.children_begin( ), file_path + folder_name + "/" );
		}
		else
		{
			if( System::IO::Path::GetExtension( gcnew String( it.get_name( ) ) ) == animation_collection_item::extension )
			{
				String^ file_name = System::IO::Path::GetFileNameWithoutExtension( gcnew String( it.get_name( ) ) );
				m_parent->add_item( file_path+file_name, editor_base::node_resource );
			}
		}
		++it;
	}
}

} // namespace animation_editor
} // namespace xray