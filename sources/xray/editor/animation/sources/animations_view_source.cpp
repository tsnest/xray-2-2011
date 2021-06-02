////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animations_view_source.h"
#include "animation_editor.h"

using xray::animation_editor::animations_view_source;
using System::Windows::Forms::TreeNode;
using System::Reflection::BindingFlags;
using xray::animation_editor::animations_node_sorter;
using xray::editor::controls::tree_node;
using xray::editor::controls::tree_node_type;

////////////////////////////////////////////////////////////////////////////
//- class animations_node_sorter ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
int animations_node_sorter::Compare(Object^ x, Object^ y)
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
//- class animations_view_source ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
animations_view_source::animations_view_source()
{
}

animations_view_source::~animations_view_source()
{
}

tree_view^ animations_view_source::parent::get()
{
	return m_parent;
}

void animations_view_source::parent::set(tree_view^ value)
{
	m_parent = value;
}

void animations_view_source::refresh()
{
	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &animations_view_source::on_fs_iterator_ready), g_allocator);
	resources::query_vfs_iterator(unmanaged_string( animation_editor::single_animations_path ).c_str( ), 
		boost::bind(&fs_iterator_delegate::callback, q, _1), g_allocator, xray::resources::recursive_true);
}

void animations_view_source::on_fs_iterator_ready(xray::vfs::vfs_locked_iterator const & fs_it)
{
	m_parent->nodes->Clear();
	m_parent->add_group("animations/", editor_base::folder_closed, editor_base::folder_open);
	process_fs(fs_it.children_begin(), "animations/");
	m_parent->TreeViewNodeSorter = gcnew animations_node_sorter();
	m_parent->Sort();
	TreeNode^ node = m_parent->root;
	node->FirstNode->Expand();
}

void animations_view_source::process_fs(xray::vfs::vfs_iterator const & in_it, String^ file_path)
{
	xray::vfs::vfs_iterator it			=	in_it;
	while(!it.is_end())
	{
		if(it.is_folder())
		{
			String^ folder_name = gcnew String(it.get_name());
			
			m_parent->add_group(file_path+folder_name, editor_base::folder_closed, editor_base::folder_open);
			process_fs(it.children_begin(), file_path+folder_name+"/");
		}
		else
		{
			System::String^ ext = System::IO::Path::GetExtension(gcnew String(it.get_name()));
			if(ext == ".clip" || ext == ".anim_clip")
			{
				String^ file_name = System::IO::Path::GetFileNameWithoutExtension(gcnew String(it.get_name()));
				tree_node^ n = m_parent->add_item(file_path+file_name, editor_base::node_resource);
				n->Tag = (ext==".anim_clip");
			}
		}

		++it;
	}
}
