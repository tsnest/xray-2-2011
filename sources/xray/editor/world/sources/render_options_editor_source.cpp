////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "render_options_editor_source.h"
#include "resource_editor.h"

namespace xray {
namespace editor {

render_options_editor_source::render_options_editor_source()
{
	
}

render_options_editor_source::render_options_editor_source(Object^ /*filter*/)
{
	
}

	

void render_options_editor_source::on_fs_iterator_ready( vfs::vfs_locked_iterator const & fs_it )
{
	process_fs					( fs_it.children_begin(), "" );
	m_parent->Sort				( );
	m_parent->on_items_loaded	( );
}

void render_options_editor_source::process_fs( vfs::vfs_iterator const & in_it, System::String^ file_path )
{
	vfs::vfs_iterator it				=	in_it;

	while( !it.is_end() )
	{
		if(it.is_folder())
		{
			System::String^ folder_name		= gcnew System::String(it.get_name());
			TreeNode^ node					= m_parent->add_group(file_path+folder_name, xray::editor_base::folder_closed, xray::editor_base::folder_open);
			process_fs						( it.children_begin(), file_path+folder_name+"/" );
			
			if(node->Nodes->Count == 0)
				m_parent->Nodes->Remove(node);
		}else
		{
			System::String^ extension		= Path::GetExtension(gcnew System::String(it.get_name()));
			if(extension == ".cfg" )
			{
				System::String^ file_name	= Path::GetFileNameWithoutExtension(gcnew System::String(it.get_name()));
				m_parent->add_item		( file_path+file_name, xray::editor_base::node_texture );
			}
		}
		++it;
	}
}

void	render_options_editor_source::refresh					()
{
	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &render_options_editor_source::on_fs_iterator_ready), g_allocator);
	resources::query_vfs_iterator(	unmanaged_string(c_source_textures_path).c_str(), 
									boost::bind(&fs_iterator_delegate::callback, q, _1), 
									g_allocator,
									resources::recursive_true );
	LOG_WARNING("query fs");
}


}//namespace editor
}//namespace xray