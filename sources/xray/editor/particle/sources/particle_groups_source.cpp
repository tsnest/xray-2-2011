////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_groups_source.h"
#include "particle_editor.h"
#include "particle_graph_node.h"

using namespace System;
using namespace System::IO;

namespace xray {
namespace particle_editor {

void				particle_groups_source::refresh					()
{
	//m_parent->Nodes->Clear();
	
	ToolStripMenuItem^ copy_menu_item		= gcnew ToolStripMenuItem("Copy");
	ToolStripMenuItem^ paste_menu_item		= gcnew ToolStripMenuItem("Paste");
	
	m_parent_editor->gui_binder->bind(copy_menu_item,		"Copy in TreeView");
	m_parent_editor->gui_binder->bind(paste_menu_item,		"Paste in TreeView");

	System::Windows::Forms::ContextMenuStrip^	context_menu		= gcnew System::Windows::Forms::ContextMenuStrip();
	context_menu->Items->AddRange					(gcnew array<ToolStripMenuItem^>{
			gcnew ToolStripMenuItem("New Document", nullptr, gcnew EventHandler(m_parent_editor, &particle_editor::on_tree_view_new_document)),
			gcnew ToolStripMenuItem("New Folder",	nullptr, gcnew EventHandler(m_parent_editor, &particle_editor::on_tree_view_new_folder)),
			copy_menu_item,
			paste_menu_item
	});
	//context_menu->Opening += gcnew System::ComponentModel::CancelEventHandler(&process_clipboard);
	m_parent->ContextMenuStrip = context_menu;


	
	m_node_to_refresh_path = gcnew String("particles") + "/";
	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &particle_groups_source::on_fs_iterator_ready), g_allocator);
	resources::query_vfs_iterator(	unmanaged_string(particle_editor::particle_resources_path + "/particles" ).c_str(), 
		boost::bind(&fs_iterator_delegate::callback, q, _1), g_allocator, xray::resources::recursive_true);
}

void				particle_groups_source::refresh					(String^ path)
{
	//m_parent->get_node(path)->nodes->Clear();
	m_node_to_refresh_path = path + "/";
	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &particle_groups_source::on_fs_iterator_ready), g_allocator);
	resources::query_vfs_iterator(	unmanaged_string(particle_editor::particle_resources_path + "/" + path).c_str(), 
		boost::bind(&fs_iterator_delegate::callback, q, _1), g_allocator, xray::resources::recursive_true);
}

void particle_groups_source::on_fs_iterator_ready(vfs::vfs_locked_iterator const & fs_it)
{
	m_parent_editor->add_context_menu_to_tree_view_node(m_parent->add_group(m_node_to_refresh_path, editor_base::folder_closed, editor_base::folder_open));
	process_fs(fs_it.children_begin(), m_node_to_refresh_path);

	m_node_to_refresh_path = "";

	m_parent->on_items_loaded();
}

void				particle_groups_source::process_fs				(vfs::vfs_iterator const &  in_it, String^ file_path)
{
	vfs::vfs_iterator	it				=	in_it;
	while( !it.is_end() )
	{
		if(it.is_folder())
		{
			String^ folder_name = gcnew String(it.get_name());
			tree_node^ t_node = m_parent->add_group(file_path+folder_name, editor_base::folder_closed, editor_base::folder_open);

			m_parent_editor->add_context_menu_to_tree_view_node(t_node);

			//t_node->ContextMenuStrip->Opening += gcnew System::ComponentModel::CancelEventHandler(&process_clipboard);

			process_fs	(it.children_begin(), file_path+folder_name+"/");
		}
		else
		{
			if(Path::GetExtension(gcnew String(it.get_name())) == ".particle")
			{
				String^ file_name = Path::GetFileNameWithoutExtension(gcnew String(it.get_name()));
				tree_node^ t_node = m_parent->get_node(file_path + file_name);
				if ( t_node == nullptr){
					t_node = m_parent->add_item(file_path + file_name, editor_base::particle_tree_icon);
					m_parent_editor->add_context_menu_to_tree_view_node(t_node);
					tree_node^ lod_node = t_node->add_node_file_part("LOD 0");
					m_parent_editor->add_context_menu_to_tree_view_node(lod_node);
				}
				t_node->nodes->Clear();
				m_parent_editor->add_context_menu_to_tree_view_node(t_node->add_node_file_part("LOD 0"));
				t_node->Collapse();
				t_node->first_expand_processed = false;
			}
		}
		++it;
	}
}



}//namespace particle_editor
}//namespace xray