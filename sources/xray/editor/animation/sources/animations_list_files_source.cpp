////////////////////////////////////////////////////////////////////////////
//	Created		: 12.01.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animations_list_files_source.h"
#include <xray/fs_watcher.h>
#include "animation_setup_manager.h"
#include "animation_editor.h"

using xray::animation_editor::animations_list_files_source;
using System::Windows::Forms::TreeNode;
using System::Reflection::BindingFlags;
using xray::animation_editor::animations_list_node_sorter;
using xray::animation_editor::animation_setup_manager;
using xray::editor::controls::tree_node;
using xray::editor::controls::tree_node_type;

////////////////////////////////////////////////////////////////////////////
//- class animations_list_node_sorter -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
int animations_list_node_sorter::Compare(Object^ x, Object^ y)
{
	tree_node^ left = safe_cast<tree_node^>(x);
	tree_node^ right = safe_cast<tree_node^>(y);
	if(left->m_node_type!=tree_node_type::single_item)
	{
		if(right->m_node_type != tree_node_type::single_item)
			return (left->Text->CompareTo(right->Text));

		return (false);
	}

	if(right->m_node_type != tree_node_type::single_item)
		return (true);

	return (left->Text->CompareTo(right->Text));
}
////////////////////////////////////////////////////////////////////////////
//- class animations_list_files_source ------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
animations_list_files_source::animations_list_files_source()
{
	m_animations_list = gcnew System::Collections::ArrayList();
	m_callback = NEW(file_watcher_delegate)(gcnew file_watcher_delegate::Delegate(this, &animations_list_files_source::fs_callback));
	m_subscribe_id = xray::resources::subscribe_watcher(
		unmanaged_string( animation_editor::single_animations_path ).c_str( ), 
		boost::bind(&file_watcher_delegate::callback, m_callback, _1));
}

animations_list_files_source::~animations_list_files_source()
{
	xray::resources::unsubscribe_watcher(m_subscribe_id);
	DELETE(m_callback);
}

void animations_list_files_source::fs_callback(xray::vfs::vfs_notification const & info)
{
	xray::fs_new::virtual_path_string file_path = info.virtual_path;
	u32 pos = file_path.rfind('.');
	u32 l = file_path.length();
	if(pos==l-8)
		return;

	if(info.type!=xray::vfs::vfs_notification::type_modified)
		refresh();
}

tree_view^ animations_list_files_source::parent::get()
{
	return m_parent;
}

void animations_list_files_source::parent::set(tree_view^ value)
{
	m_parent = value;
}

void animations_list_files_source::refresh()
{
	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &animations_list_files_source::on_fs_iterator_ready), g_allocator);
	resources::query_vfs_iterator(unmanaged_string( animation_editor::single_animations_path ).c_str( ), 
		boost::bind(&fs_iterator_delegate::callback, q, _1), g_allocator, xray::resources::recursive_true);
}

void animations_list_files_source::on_fs_iterator_ready(xray::vfs::vfs_locked_iterator const & fs_it)
{
	m_parent->nodes->Clear();
	m_animations_list->Clear();
	m_parent->add_group("animations/", editor_base::folder_closed, editor_base::folder_open);
	process_fs(fs_it.children_begin(), "animations/");
	m_parent->TreeViewNodeSorter = gcnew animations_list_node_sorter();
	m_parent->Sort();
	TreeNode^ node = m_parent->root;
	node->FirstNode->Expand();
}

void animations_list_files_source::process_fs(xray::vfs::vfs_iterator const & in_it, String^ file_path)
{
	xray::vfs::vfs_iterator it			=	in_it;
	while(!it.is_end())
	{
		if(it.is_folder())
		{
			String^ folder_name = gcnew String(it.get_name());
			if(folder_name=="controllers" || 
				folder_name=="skeletons" || 
				folder_name=="viewer_scenes" || 
				folder_name=="animation_clips")
			{
				++it;
				continue;
			}

			m_parent->add_group(file_path+folder_name, editor_base::folder_closed, editor_base::folder_open);
			process_fs(it.children_begin(), file_path+folder_name+"/");
		}
		else
		{
			if(System::IO::Path::GetExtension(gcnew String(it.get_name())) == ".clip")
			{
				String^ file_name = System::IO::Path::GetFileNameWithoutExtension(gcnew String(it.get_name()));
				m_parent->add_item(file_path+file_name, editor_base::node_resource);
				System::String^ anim_name = (file_path+file_name)->Remove(0, 11);
				if(!(m_animations_list->Contains(anim_name)))
					m_animations_list->Add(anim_name);
			}
		}
		++it;
	}
}
