////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_selector_source.h"
#include "resource_editor.h"

using namespace System::Windows::Forms;

namespace xray{
namespace editor{


void	stage_selector_source::on_fs_iterator_ready	(xray::vfs::vfs_locked_iterator const & fs_it)
{
	m_parent->nodes->Clear();
	process_fs(fs_it.children_begin(), "");
	m_parent->Sort();
	m_parent->on_items_loaded();
}

void	stage_selector_source::process_fs				(xray::vfs::vfs_iterator const & in_it,  System::String^ file_path)
{
	vfs::vfs_iterator it				=	in_it;
	while( !it.is_end() )
	{
		if(it.is_folder())
		{
			System::String^ folder_name = gcnew System::String(it.get_name());
			TreeNode^ node = m_parent->add_group(file_path+folder_name, xray::editor_base::folder_closed, xray::editor_base::folder_open);
			process_fs	(it.children_begin(), file_path+folder_name+"/");
			if(node->Nodes->Count == 0)
				m_parent->Nodes->Remove(node);
		}
		else
		{
			System::String^ path = gcnew System::String(it.get_name());
			if(Path::GetExtension(path) == ".stage")
			{
				System::String^ file_name		= Path::GetFileNameWithoutExtension(path);
				bool	is_existing		= false;
				for each(System::String^ exist_item in existing_items)
				{
					if(exist_item == file_name)
						is_existing = true;
				}
				if(!is_existing)
					m_parent->add_item(file_path+file_name, xray::editor_base::node_resource);
			}
		}
		++it;
	}
}

void	stage_selector_source::refresh					()
{
	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &stage_selector_source::on_fs_iterator_ready), g_allocator);
	resources::query_vfs_iterator(	unmanaged_string(c_source_effects_path).c_str(), 
		boost::bind(&fs_iterator_delegate::callback, q, _1), g_allocator);
}

} // namespace editor
} // namespace xray