////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_groups_controllers_tree.h"
#include "animation_groups_controllers_tree_files_source.h"
#include "animation_groups_editor.h"

using xray::animation_editor::animation_groups_controllers_tree;
using xray::animation_editor::animation_groups_editor;
using namespace System::Collections::ObjectModel;

animation_groups_controllers_tree::animation_groups_controllers_tree(animation_groups_editor^ ed)
:super(ed->multidocument_base),
m_groups_editor(ed)
{
	xray::editor_base::image_loader	loader;
	tree_view_image_list = loader.get_shared_images16();
	tree_view_image_list->TransparentColor = System::Drawing::Color::Transparent;

	m_raw_files_tree_view->ImageList = tree_view_image_list;
	m_raw_files_tree_view->source = gcnew animation_groups_controllers_tree_files_source();
	m_raw_files_tree_view->refresh();
	extension = ".lua";
	HideOnClose = true;
}

void animation_groups_controllers_tree::on_node_double_click(System::Object^, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e)
{
	if(safe_cast<tree_node^>(e->Node)->m_node_type==tree_node_type::single_item)
	{
		System::String^ full_path = e->Node->FullPath;
		m_editor->load_document(full_path);
	}
}

void animation_groups_controllers_tree::track_active_item(System::String^ path)
{
	m_raw_files_tree_view->track_active_node(path);
}