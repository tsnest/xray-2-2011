#include "pch.h"
#include "light_tool_tab.h"
#include "tool_light.h"
#include "level_editor.h"
#include "library_objects_tree.h"

namespace xray{
namespace editor{

void light_tool_tab::post_constructor( )
{
	m_library_objects_tree							= gcnew library_objects_tree( m_tool );
	m_library_objects_tree->show_empty_folders		= true;
	m_library_objects_tree->sync_tool_selected_name = true;
	m_library_objects_tree->allow_folder_creation	= false;
	m_library_objects_tree->allow_items_reparent	= false;
	m_library_objects_tree->allow_items_rename		= false;
	m_library_objects_tree->allow_items_delete		= false;
	
	m_library_objects_tree->Dock					= System::Windows::Forms::DockStyle::Fill;
	objects_tree_place->Controls->Add				( m_library_objects_tree );
}

void light_tool_tab::fill_objects_list( )
{
	m_library_objects_tree->fill_objects_list( nullptr );
}

void light_tool_tab::on_activate()
{
	m_tool->get_level_editor()->set_active_tool( m_tool );
}

void light_tool_tab::on_deactivate()
{
	m_tool->get_level_editor()->set_active_tool( nullptr );
}

} //namespace xray
} //namespace editor
