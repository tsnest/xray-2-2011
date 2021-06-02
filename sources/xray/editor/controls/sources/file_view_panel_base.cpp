////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "file_view_panel_base.h"
#include "document_editor_base.h"
#include "tree_node.h"
#include "tree_node_alphanum_comparer.h"

using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;
using System::Windows::Forms::TreeNode;

namespace xray {
namespace editor {
namespace controls{

void file_view_panel_base::in_constructor()
{
	m_raw_files_tree_view->TreeViewNodeSorter = gcnew tree_node_alphanum_comparer();
	m_raw_files_tree_view->ContextMenuStrip	= gcnew System::Windows::Forms::ContextMenuStrip;
}

void file_view_panel_base::track_active_item(System::String^ path)
{
	m_raw_files_tree_view->track_active_node(path);
}

void file_view_panel_base::add_folders_and_files(System::Windows::Forms::TreeNodeCollection^ col, System::String^ path, Int32 folder_icon_collapsed, Int32 folder_icon_expanded, Int32 file_icon)
{
	m_raw_files_tree_view->add_group(path, folder_icon_collapsed, folder_icon_expanded);
	for each(tree_node^ n in col)
	{
		if(n->m_node_type == tree_node_type::single_item)
			m_raw_files_tree_view->add_item(path+"/"+n->Name, file_icon);
		else
			add_folders_and_files(n->nodes, path+"/"+n->Name, folder_icon_collapsed, folder_icon_expanded, file_icon);
	}
}

void file_view_panel_base::iterate_folders(System::IO::DirectoryInfo^ di, System::String^ path)
{
	System::IO::Directory::CreateDirectory(path);

	cli::array<System::IO::FileSystemInfo^ >^ infos = di->GetFileSystemInfos();

	// Copy the files and subdirectories
	for each(System::IO::FileSystemInfo^ i in infos)
	{
		if(i->GetType()==System::IO::FileInfo::typeid)
			System::IO::File::Copy(i->FullName, path+"/"+i->Name);
		else
			iterate_folders(safe_cast<System::IO::DirectoryInfo^>(i), path+"/"+i->Name);
	}
	delete infos;
}

bool file_view_panel_base::check_parent(System::String^ dest_path, System::String^ node_name)
{
	cli::array<System::String^>^ segments = dest_path->Trim('/')->Split('/');
	for each(System::String^ segment in segments)
	{
		if(segment==node_name)
			return true;
	}
	return false;
}

void file_view_panel_base::refresh()
{
	m_raw_files_tree_view->refresh();
}

} // namespace controls
} // namespace editor
} // namespace xray