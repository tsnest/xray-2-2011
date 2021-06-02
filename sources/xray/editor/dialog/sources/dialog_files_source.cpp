////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_files_source.h"
#include <xray/fs_watcher.h>

using xray::dialog_editor::dialog_files_source;
using System::Windows::Forms::TreeNode;
using System::Reflection::BindingFlags;
using xray::dialog_editor::dialogs_node_sorter;
using xray::editor::controls::tree_node;
using xray::editor::controls::tree_node_type;
////////////////////////////////////////////////////////////////////////////
//- class dialogs_node_sorter ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
int dialogs_node_sorter::Compare(Object^ x, Object^ y)
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
//- class dialog_files_source ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
dialog_files_source::dialog_files_source()
{
	m_callback = NEW(file_watcher_delegate)(gcnew file_watcher_delegate::Delegate(this, &dialog_files_source::fs_callback));
	m_watcher_subscriber_id	=	xray::resources::subscribe_watcher("resources/dialogs/configs", boost::bind(&file_watcher_delegate::callback, m_callback, _1));
}

dialog_files_source::~dialog_files_source()
{
	xray::resources::unsubscribe_watcher(m_watcher_subscriber_id);
	DELETE(m_callback);
}

void dialog_files_source::fs_callback(xray::vfs::vfs_notification const & info)
{
	if(info.type!=xray::vfs::vfs_notification::type_modified)
		refresh();
}

tree_view^ dialog_files_source::parent::get()
{
	return m_parent;
}

void dialog_files_source::parent::set(tree_view^ value)
{
	m_parent = value;
}

void dialog_files_source::refresh()
{
	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &dialog_files_source::on_fs_iterator_ready), g_allocator);
	resources::query_vfs_iterator("resources/dialogs/configs", 
		boost::bind(&fs_iterator_delegate::callback, q, _1), g_allocator, xray::resources::recursive_true);
}

void dialog_files_source::on_fs_iterator_ready(xray::vfs::vfs_locked_iterator const & fs_it)
{
	m_parent->nodes->Clear();
	m_parent->add_group("dialogs/", editor_base::folder_closed, editor_base::folder_open);
	process_fs(fs_it.children_begin(), "dialogs/");
	m_parent->TreeViewNodeSorter = gcnew dialogs_node_sorter();
	m_parent->Sort();
	TreeNode^ node = m_parent->root;
	node->FirstNode->Expand();
}

void dialog_files_source::process_fs(xray::vfs::vfs_iterator const & in_it, String^ file_path)
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
			if(System::IO::Path::GetExtension(gcnew String(it.get_name())) == ".dlg")
			{
				String^ file_name = System::IO::Path::GetFileNameWithoutExtension(gcnew String(it.get_name()));
				m_parent->add_item(file_path+file_name, editor_base::node_resource);
			}
		}
		++it;
	}
}
