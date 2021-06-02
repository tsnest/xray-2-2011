////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_editor_source.h"
#include "resource_editor.h"

xray::render::texture_options::ETType get_texture_type_from_name(pcstr);

namespace xray {
namespace editor {

void texture_editor_source::filter::set(Object^ value)
{ 
	m_filter_type = get_texture_type_from_name(unmanaged_string(safe_cast<System::String^>(value)).c_str());
}

texture_editor_source::texture_editor_source()
{
	m_filter_type = xray::render::texture_options::ETType(-1);
}

texture_editor_source::texture_editor_source(Object^ filter)
{
	if(filter == nullptr)
		m_filter_type = xray::render::texture_options::ETType(-1);
	else
		m_filter_type = get_texture_type_from_name(unmanaged_string(safe_cast<System::String^>(filter)).c_str());
}

	

void texture_editor_source::on_fs_iterator_ready( vfs::vfs_locked_iterator const & fs_it )
{
	process_fs					( fs_it.children_begin(), "" );
	m_parent->Sort				( );
	m_parent->on_items_loaded	( );
}

void texture_editor_source::process_fs( vfs::vfs_iterator const & in_it, System::String^ file_path )
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
			if(extension == ".tga" )
			{
				System::String^ file_name	= Path::GetFileNameWithoutExtension(gcnew System::String(it.get_name()));
				
				if(m_filter_type != xray::render::texture_options::ETType(-1) )
				{
					if(get_texture_type_from_name(unmanaged_string(file_name).c_str()) == m_filter_type )
						m_parent->add_item	( file_path+file_name, xray::editor_base::node_texture );
				}else
				{
					m_parent->add_item		( file_path+file_name, xray::editor_base::node_texture );
				}
			}
		}
		++it;
	}
}

void	texture_editor_source::refresh					()
{
	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &texture_editor_source::on_fs_iterator_ready), g_allocator);
	resources::query_vfs_iterator(	unmanaged_string(c_source_textures_path).c_str(), 
									boost::bind(&fs_iterator_delegate::callback, q, _1), 
									g_allocator,
									resources::recursive_true );
	LOG_WARNING("query fs");
}


}//namespace editor
}//namespace xray