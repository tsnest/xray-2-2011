////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "tree_view_files_source.h"
#include <xray/fs_watcher.h>
#include "sound_editor.h"

using xray::sound_editor::tree_view_files_source;
using xray::sound_editor::tree_view_files_node_sorter;
using System::Windows::Forms::TreeNode;
using System::Reflection::BindingFlags;
using xray::editor::controls::tree_node;
using xray::editor::controls::tree_node_type;
//-------------------------------------------------------------------------------------------
//- class tree_view_files_node_sorter -------------------------------------------------------
//-------------------------------------------------------------------------------------------
int tree_view_files_node_sorter::Compare(Object^ x, Object^ y)
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
//-------------------------------------------------------------------------------------------
//- class tree_view_files_source ------------------------------------------------------------
//-------------------------------------------------------------------------------------------
tree_view_files_source::tree_view_files_source(System::Windows::Forms::ContextMenuStrip^ context_menu)
{
	m_callback = NEW(file_watcher_delegate)(gcnew file_watcher_delegate::Delegate(this, &tree_view_files_source::fs_callback));
	String^ res_path = sound_editor::sound_resources_path;
	unmanaged_string unm_res_path(res_path);
	m_watcher_subscriber_id	=	xray::resources::subscribe_watcher(unm_res_path.c_str(), boost::bind(&file_watcher_delegate::callback, m_callback, _1));
	mode = tree_view_files_source_mode::show_single;
	m_context_menu = context_menu;
	do_not_refresh = false;
}

tree_view_files_source::~tree_view_files_source()
{
	String^ res_path = sound_editor::sound_resources_path;
	unmanaged_string unm_res_path(res_path);
	xray::resources::unsubscribe_watcher(m_watcher_subscriber_id);
	DELETE(m_callback);
}

void tree_view_files_source::fs_callback(xray::vfs::vfs_notification const& info)
{
	xray::fs_new::virtual_path_string file_path = info.virtual_path;
	u32 pos = file_path.rfind('.');
	u32 l = file_path.length();
	if(mode==tree_view_files_source_mode::show_single && pos!=l-4)
		return;

	if(mode==tree_view_files_source_mode::show_composite && pos!=l-16)
		return;

	if(mode==tree_view_files_source_mode::show_collections && pos!=l-17)
		return;

	if(info.type!=xray::vfs::vfs_notification::type_modified && !do_not_refresh)
		refresh();

	do_not_refresh = false;
}

tree_view^ tree_view_files_source::parent::get()
{
	return m_parent;
}

void tree_view_files_source::parent::set(tree_view^ value)
{
	m_parent = value;
}

void tree_view_files_source::refresh()
{
	String^ res_path = sound_editor::sound_resources_path;
	unmanaged_string unm_res_path(res_path);
	xray::fs_new::virtual_path_string path = unm_res_path.c_str();
	if(mode==tree_view_files_source_mode::show_single)
		path += "single";
	else if(mode==tree_view_files_source_mode::show_composite)
		path += "composite";
	else if(mode==tree_view_files_source_mode::show_collections)
		path += "collections";

	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &tree_view_files_source::on_fs_iterator_ready), g_allocator);
	resources::query_vfs_iterator(path.c_str(), boost::bind(&fs_iterator_delegate::callback, q, _1), g_allocator, resources::recursive_true);
}

void tree_view_files_source::on_fs_iterator_ready(xray::vfs::vfs_locked_iterator const &  fs_it)
{
	m_parent->nodes->Clear();
	process_fs(fs_it.children_begin(), "");
	m_parent->TreeViewNodeSorter = gcnew tree_view_files_node_sorter();
	m_parent->Sort();
}

void tree_view_files_source::process_fs(xray::vfs::vfs_iterator const & in_it, String^ file_path)
{
	xray::vfs::vfs_iterator it				=	in_it;
	while(!it.is_end())
	{
		if(it.is_folder())
		{
			String^ folder_name = gcnew String(it.get_name());
			tree_node^ n = m_parent->add_group(file_path+folder_name, editor_base::folder_closed, editor_base::folder_open);
			n->ContextMenuStrip = m_context_menu;
			process_fs(it.children_begin(), file_path+folder_name+"/");
		}
		else
		{
			System::Collections::Generic::List<String^>^ ext_lst = gcnew System::Collections::Generic::List<String^>();
			if(mode==tree_view_files_source_mode::show_single || mode==tree_view_files_source_mode::show_all)
				ext_lst->Add(".wav");
			if(mode==tree_view_files_source_mode::show_composite || mode==tree_view_files_source_mode::show_all)
				ext_lst->Add(".composite_sound");
			if(mode==tree_view_files_source_mode::show_collections || mode==tree_view_files_source_mode::show_all)
				ext_lst->Add(".sound_collection");

			for each(String^ ext in ext_lst)
			{
				if(System::IO::Path::GetExtension(gcnew String(it.get_name())) == ext)
				{
					String^ file_name = System::IO::Path::GetFileNameWithoutExtension(gcnew String(it.get_name()));
					tree_node^ n = m_parent->add_item(file_path+file_name, editor_base::node_sound_file);
					n->ContextMenuStrip = m_context_menu;
				}
			}
		}

		++it;
	}
}
