#include "pch.h"
#include "compound_tool_tab.h"
#include "tool_composite.h"
#include "tool_scripting.h"
#include "object_composite.h"
#include "object_scene.h"
#include "level_editor.h"
#include "select_library_class_form.h"
#include "library_objects_tree.h"
#include "project.h"
#include "window_ide.h"

namespace xray {
namespace editor {


typedef xray::editor_base::action_delegate action_delegate;
typedef xray::editor_base::execute_delegate_managed execute_delegate_managed;
typedef xray::editor_base::enabled_delegate_managed enabled_delegate_managed;
typedef xray::editor_base::checked_delegate_managed checked_delegate_managed;


void compound_tool_tab::post_constructor( )
{
	m_library_objects_tree							= gcnew library_objects_tree( m_tool );
	m_library_objects_tree->show_empty_folders		= true;
	m_library_objects_tree->sync_tool_selected_name = true;
	m_library_objects_tree->allow_folder_creation	= true;
	m_library_objects_tree->allow_items_reparent	= true;
	m_library_objects_tree->allow_items_rename		= true;
	m_library_objects_tree->allow_items_delete		= true;

	m_library_objects_tree->Dock					= System::Windows::Forms::DockStyle::Fill;
	objects_tree_place->Controls->Add				( m_library_objects_tree );

	m_library_objects_tree->m_current_path_changed	+= gcnew current_path_changed( this, &compound_tool_tab::library_current_path_changed );
	

	level_editor^ l_editor = m_tool->get_level_editor();
	xray::editor_base::gui_binder^ gui_binder = l_editor->get_gui_binder( );
	
	l_editor->ide()->create_tool_strip		( "CompositeEditToolStrip", 0);

	action_delegate^ a	= nullptr;
	System::String^ action_name			= "New composite from selection";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &compound_tool_tab::new_from_selection_button_Click) );
	a->set_enabled	(gcnew enabled_delegate_managed(this, &compound_tool_tab::is_not_in_edit_mode));
	l_editor->get_input_engine()->register_action( a ,"");
	gui_binder->register_image( "base", action_name,	xray::editor_base::node_group );
	l_editor->ide()->add_action_button_item( gui_binder, action_name, "CompositeEditToolStrip", 10);

	action_name			= "Edit/Commit selected composite";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &compound_tool_tab::edit_selected_compound_button_Click) );	
	l_editor->get_input_engine()->register_action( a ,"");
	gui_binder->register_image( "base", action_name,	xray::editor_base::object_light );
	l_editor->ide()->add_action_button_item( gui_binder, action_name, "CompositeEditToolStrip", 10);
	

	action_name			= "Add selected to composite";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &compound_tool_tab::add_selected_to_compound_button_Click) );
	a->set_enabled	(gcnew enabled_delegate_managed(this, &compound_tool_tab::is_in_edit_mode));
	l_editor->get_input_engine()->register_action( a ,"");
	gui_binder->register_image( "base", action_name,	xray::editor_base::terrain_painter );
	l_editor->ide()->add_action_button_item( gui_binder, action_name, "CompositeEditToolStrip", 10);


	action_name			= "Add/Remove logic in composite";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &compound_tool_tab::on_add_logic_button_clicked) );
	a->set_enabled	(gcnew enabled_delegate_managed(this, &compound_tool_tab::is_in_edit_mode));
	l_editor->get_input_engine()->register_action( a ,"");
	gui_binder->register_image( "base", action_name,	xray::editor_base::node_joint );
	l_editor->ide()->add_action_button_item( gui_binder, action_name, "CompositeEditToolStrip", 10);


	action_name			= "Cancel changes in composite";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &compound_tool_tab::cancel_selected_compound_button_Click) );
	a->set_enabled	(gcnew enabled_delegate_managed(this, &compound_tool_tab::is_in_edit_mode));
	l_editor->get_input_engine()->register_action( a ,"");
	gui_binder->register_image( "base", action_name,	xray::editor_base::npc_tree_icon );
	l_editor->ide()->add_action_button_item( gui_binder, action_name, "CompositeEditToolStrip", 10);

	action_name			= "Ungroup composite";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &compound_tool_tab::ungroup_button_Click) );
	l_editor->get_input_engine()->register_action( a ,"");
	gui_binder->register_image( "base", action_name,	xray::editor_base::camera_track );
	l_editor->ide()->add_action_button_item( gui_binder, action_name, "CompositeEditToolStrip", 10);
	
}

void compound_tool_tab::fill_objects_list( )
{
	m_library_objects_tree->fill_objects_list( nullptr );
}

void compound_tool_tab::on_project_clear( )
{
	switch_edit_mode	( false );
}

void compound_tool_tab::on_activate()
{
	m_tool->get_level_editor()->set_active_tool( m_tool );
}

void compound_tool_tab::on_deactivate()
{
	m_tool->get_level_editor()->set_active_tool( nullptr );
}

void compound_tool_tab::library_current_path_changed( System::String^ library_object_path, System::String^ prev_path )
{
	XRAY_UNREFERENCED_PARAMETERS			( prev_path );
	object_details_list->Items->Clear		( );

	if(	library_object_path->Length )
	{
		compound_details_list^ details_list		= gcnew compound_details_list;
		m_tool->get_library_item_details		( library_object_path, details_list );
		u32 cnt = details_list->Count;
		for( u32 idx=0; idx<cnt; idx+=2)
		{
			ListViewItem^ itm = gcnew ListViewItem(gcnew cli::array<System::String^>(2){details_list[idx], details_list[idx+1]} );
			object_details_list->Items->Add( itm );
		}
	}
}

void compound_tool_tab::cancel_selected_compound_button_Click(  )
{
	if(m_tool->current_edit_object)
	{
		switch_edit_mode								( false );
		m_tool->undo_current_edit_object				( );
	}
}

void compound_tool_tab::switch_edit_mode( bool b )
{
	if(b)
	{
//		edit_selected_compound_button->Text				= "Commit Current";
//		edit_selected_compound_button->BackColor		= System::Drawing::Color::Red;

//		add_selected_to_compound_button->Enabled		= true;
//		cancel_selected_compound_button->Enabled		= true;
//		m_add_logic_button->Enabled						= true;
//		new_from_selection_button->Enabled				= false;

//		m_add_logic_button->Text = ( m_tool->current_edit_object && m_tool->current_edit_object->logic_scene ) ? "Remove logic" : "Add logic";

	}else
	{
		label1->Text									= "<nothing>";
// 		edit_selected_compound_button->Text				= "Edit Selected";
//		edit_selected_compound_button->BackColor		= System::Drawing::SystemColors::Control;

//		add_selected_to_compound_button->Enabled		= false;
//		cancel_selected_compound_button->Enabled		= false;
//		m_add_logic_button->Enabled						= false;
//		new_from_selection_button->Enabled				= true;
	}
}

void compound_tool_tab::edit_selected_compound_button_Click( )
{
	if(m_tool->current_edit_object)
	{
		if(m_tool->commit_compound_object( m_tool->current_edit_object ))
		{
			switch_edit_mode								( false );
			m_tool->current_edit_object = nullptr;
		}
		return;
	}

	object_base_list^ selection_list = m_tool->get_level_editor()->selected_objects();
	
	if(selection_list->Count ==1)
		m_tool->current_edit_object	= dynamic_cast<object_composite^>(selection_list[0]);

	if(m_tool->current_edit_object)
	{
		label1->Text		= m_tool->current_edit_object->get_name();
		switch_edit_mode								( true );
	}else
	{
		m_tool->get_level_editor()->ShowMessageBox( "Select compound object in scene.",
													MessageBoxButtons::OK,
													MessageBoxIcon::Exclamation );
	}
}

void compound_tool_tab::add_selected_to_compound_button_Click(  )
{
	if(!m_tool->current_edit_object)
		return;

	object_base_list^ selection_list = m_tool->get_level_editor()->selected_objects();

	add_to_current_compound						( selection_list );
}

void compound_tool_tab::add_to_current_compound( object_base_list^ list )
{
	if( !m_tool->current_edit_object || list->Count==0 )
		return;

	for each(object_base^ o in list)
		m_tool->current_edit_object->edit_add_object( o );
}

void compound_tool_tab::new_from_selection_button_Click( )
{
	object_base_list^ selection_list = m_tool->get_level_editor()->selected_objects();
	
	if(m_tool->current_edit_object)
	{
		// not allowed
		return;
	}

	if(selection_list->Count)
	{
		math::float3 p = selection_list[0]->get_position	( );
		m_tool->get_level_editor()->add_library_object		( p, m_tool, "#empty" );

		edit_selected_compound_button_Click					( /*sender, e*/ );
		add_to_current_compound								( selection_list );
	}else
	{
		m_tool->get_level_editor()->ShowMessageBox( "Select object(s) in scene.",
													MessageBoxButtons::OK,
													MessageBoxIcon::Exclamation );
	}
}

void compound_tool_tab::ungroup_button_Click( )
{
	m_tool->get_level_editor()->ShowMessageBox( "not implemented yet",
												MessageBoxButtons::OK,
												MessageBoxIcon::Exclamation );
}

void compound_tool_tab::on_add_logic_button_clicked( )
{
	if ( m_tool->current_edit_object->logic_scene == nullptr ){
		tool_scripting^ scripting_tool = safe_cast< tool_scripting^ >( m_tool->get_level_editor()->get_tool("scripting") );
		object_base^ o				= scripting_tool->create_raw_object( "base:root_scene" );
		o->set_library_name			( "base:root_scene" );
		o->set_name					( "root_scene", false );
		o->load_defaults			( );
		project_item_object^ scene_item	= gcnew project_item_object( m_tool->get_level_editor()->get_project(), o );
		scene_item->assign_id		( 0 );
		scene_item->m_guid			= System::Guid::NewGuid();
		o->set_visible				( true );	
		safe_cast<object_scene^>( o )->initialize( );
		m_tool->current_edit_object->logic_scene = safe_cast<object_scene^>( o );
		m_tool->current_edit_object->logic_scene->parent_composite = m_tool->current_edit_object;
		m_tool->current_edit_object->edit_add_object( o );
//		m_add_logic_button->Text = "Remove logic";
	}
	else
	{
		m_tool->current_edit_object->owner_project_item->remove( m_tool->current_edit_object->logic_scene->owner_project_item );
		m_tool->current_edit_object->logic_scene->owner_project_item->m_tree_node_->Remove			( );
		m_tool->current_edit_object->logic_scene = nullptr;
//		m_add_logic_button->Text = "Add logic";
	}
}


bool compound_tool_tab::is_in_edit_mode							()
{
	return m_tool->current_edit_object != nullptr; 
}
bool compound_tool_tab::is_not_in_edit_mode				() 
{
	return !is_in_edit_mode(); 
} 


} //namespace xray
} //namespace editor
