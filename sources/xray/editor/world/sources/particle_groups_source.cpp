////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_groups_source.h"
#include <xray/editor/base/managed_delegate.h>
#include "particle_editor.h"
#include "particle_graph_node.h"

using namespace System;
using namespace System::IO;

namespace xray {
namespace editor {

void				particle_groups_source::refresh					()
{
	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &particle_groups_source::on_fs_iterator_ready));
	resources::query_fs_iterator(	unmanaged_string(particle_editor::particle_resources_path+"/particles").c_str(), 
		boost::bind(&fs_iterator_delegate::callback, q, _1), g_allocator);
}

void				particle_groups_source::on_fs_iterator_ready	(xray::resources::fs_iterator fs_it)
{
	process_fs(fs_it.children_begin(), "particles/");
	m_parent->on_items_loaded();
}

void				particle_groups_source::process_fs				(xray::resources::fs_iterator it, String^ file_path)
{
	while( !it.is_end() )
	{
		if(it.is_folder())
		{
			String^ folder_name = gcnew String(it.get_name());
			m_parent->add_group(file_path+folder_name);
			process_fs	(it.children_begin(), file_path+folder_name+"/");
		}
		else
		{
			if(Path::GetExtension(gcnew String(it.get_name())) == ".particle")
			{
				String^ file_name = Path::GetFileNameWithoutExtension(gcnew String(it.get_name()));

				m_parent->add_item(file_path+file_name);
			}
		}
		++it;
	}
}

}//namespace editor
}//namespace xray