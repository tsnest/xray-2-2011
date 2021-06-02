////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_files_source.h"
#include "animation_editor.h"
#include "animation_single_item.h"

using System::Windows::Forms::TreeNode;

namespace xray{
namespace animation_editor{

animation_files_source::animation_files_source( )
	:	m_subscribe_id(0)
{
	m_callback = NEW(file_watcher_delegate)(gcnew file_watcher_delegate::Delegate(this, &animation_files_source::fs_callback));
	m_subscribe_id = xray::resources::subscribe_watcher(unmanaged_string( animation_editor::single_animations_path ).c_str( ), boost::bind(&file_watcher_delegate::callback, m_callback, _1));
}

animation_files_source::~animation_files_source( )
{
	xray::resources::unsubscribe_watcher( m_subscribe_id );
	DELETE( m_callback );
}

void animation_files_source::fs_callback( xray::vfs::vfs_notification const & info )
{
	if( info.type != xray::vfs::vfs_notification::type_modified )
		refresh( );
}

tree_view^ animation_files_source::parent::get( )
{
	return m_parent;
}

void animation_files_source::parent::set( tree_view^ value )
{
	m_parent = value;
}

void animation_files_source::refresh( )
{
	fs_iterator_delegate* q = NEW(fs_iterator_delegate)( gcnew fs_iterator_delegate::Delegate( this, &animation_files_source::on_fs_iterator_ready ), g_allocator );
	resources::query_vfs_iterator( unmanaged_string( animation_editor::single_animations_path ).c_str( ), boost::bind(&fs_iterator_delegate::callback, q, _1 ), g_allocator, xray::resources::recursive_true );
}

void animation_files_source::on_fs_iterator_ready(xray::vfs::vfs_locked_iterator const & fs_it)
{
	m_parent->nodes->Clear();
	//m_parent->add_group("animations/", editor_base::folder_closed, editor_base::folder_open);
	process_fs(fs_it.children_begin(), "");
	//m_parent->TreeViewNodeSorter = gcnew channels_node_sorter();
	m_parent->Sort( );
	TreeNode^ node = m_parent->root;
	if( node->Nodes->Count == 1 )
		node->FirstNode->Expand( );
}

void animation_files_source::process_fs(xray::vfs::vfs_iterator const & fs_it, String^ file_path)
{
	xray::vfs::vfs_iterator it	=	fs_it;
	while( !it.is_end( ) )
	{
		if( it.is_folder( ) )
		{
			String^ folder_name = gcnew String( it.get_name( ) );
			if( folder_name == "controllers" || folder_name == "skeletons" || folder_name == "viewer_scenes" )
			{
				++it;
				continue;
			}

			m_parent->add_group( file_path + folder_name, editor_base::folder_closed, editor_base::folder_open );
			process_fs( it.children_begin( ), file_path + folder_name + "/" );
		}
		else
		{
			if( System::IO::Path::GetExtension( gcnew String( it.get_name( ) ) ) == animation_single_item::extension )
			{
				String^ file_name = System::IO::Path::GetFileNameWithoutExtension( gcnew String( it.get_name( ) ) );
				m_parent->add_item( file_path + file_name, editor_base::node_resource );
			}
		}
		++it;
	}
}

} // namespace animation_editor
} // namespace xray
