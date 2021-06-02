////////////////////////////////////////////////////////////////////////////
//	Created		: 23.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "material_editor_source.h"
#include "resource_editor.h"

namespace xray {
namespace editor {


void material_editor_source::on_fs_iterator_ready( vfs::vfs_locked_iterator const & fs_it )
{
	m_items.Clear				( );
	process_fs					( fs_it.children_begin(), "" );
	m_parent->Sort				( );
	m_parent->on_items_loaded	( );
}

void material_editor_source::process_fs( vfs::vfs_iterator const & in_it,  System::String^ file_path )
{
	vfs::vfs_iterator it				= in_it;
	while( !it.is_end() )
	{
		if(it.is_folder())
		{
			System::String^ folder_name = gcnew System::String(it.get_name());
			
			m_parent->add_group(	file_path+folder_name, 
									editor_base::folder_closed, 
									editor_base::folder_open );

			process_fs					( it.children_begin(), file_path+folder_name+"/" );
		}else
		{
			System::String^ path = gcnew System::String(it.get_name());
			if(Path::GetExtension(path) == m_source_ext )
			{
				System::String^ file_name	= file_path + Path::GetFileNameWithoutExtension(path);
				m_parent->add_item			( file_name, editor_base::node_material );
				m_items.Add					( file_name );
			}
		}
		++it;
	}
}

void material_editor_source::refresh( )
{
	fs_iterator_delegate* q			= NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &material_editor_source::on_fs_iterator_ready), g_allocator);
	
	resources::query_vfs_iterator(	
							unmanaged_string(m_source_path).c_str(), 
							boost::bind(&fs_iterator_delegate::callback, q, _1), 
							g_allocator,
							resources::recursive_true
							);
}


}//namespace editor
}//namespace xray