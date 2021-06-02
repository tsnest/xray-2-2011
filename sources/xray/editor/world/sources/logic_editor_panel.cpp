////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "level_editor.h"
#include "command_add_library_object.h"
#include "logic_editor_panel.h"
#include "object_logic.h"
#include "object_scene.h"
#include "object_job.h"
#include "logic_event.h"
#include "tool_scripting.h"
#include "project_items.h"
#include "editor_world.h"
#include "logic_scene_event_property_holder.h"
#include "logic_event_action_start_scene.h"


namespace xray {
namespace editor {



void logic_editor_panel::in_constructor					( )
{	
	for each(System::String^ object_type in tool_scripting::behaviour_types_by_object_types->Keys)
		add_job_type_combobox->Items->Add( object_type );

	add_job_type_combobox->SelectedIndex = 0;	

	//load_logic_editor_library	( );
}


void logic_editor_panel::on_add_job_button_click( Object^ , System::EventArgs^ )
{
	if ( !m_selected_object )
		return;
	
	object_base^ o					= m_selected_object->tool->create_library_object_instance( "job", m_editor->get_editor_world().scene(), m_editor->get_editor_world().scene_view() );
	project_item_object^ new_item	= gcnew project_item_object( m_selected_object->get_project(), o );
	new_item->assign_id			( 0 );
	new_item->m_guid			= System::Guid::NewGuid();

	o->set_name( get_selected_job_type() , true);

	m_selected_object->add_job( new_item );

// 	m_editor->get_command_engine()->run( gcnew command_add_library_object ( m_editor, m_selected_object->tool, "job", create_translation( m_selected_object->get_position()), 
// 										gcnew object_added_callback( safe_cast<object_scene^>(m_selected_object), &object_scene::add_job ), false ) );
}

void logic_editor_panel::on_add_subscene_button_click( Object^ , System::EventArgs^ )
{
	if ( !m_selected_object )
		return;

	object_base^ o					= m_selected_object->tool->create_library_object_instance( "scene", 0, 0 );
	project_item_object^ new_item	= gcnew project_item_object( m_selected_object->get_project(), o );
	new_item->assign_id				( 0 );
	new_item->m_guid				= System::Guid::NewGuid();

	o->set_name( "scene", true );

	safe_cast<object_scene^>( o )->initialize( );	

	m_selected_object->add_scene( new_item );


// 	m_editor->get_command_engine()->run( gcnew command_add_library_object ( m_editor, m_selected_object->tool, "scene", create_translation( m_selected_object->get_position()), 
// 										gcnew object_added_callback( safe_cast<object_scene^>(m_selected_object), &object_scene::add_scene ), false ) );
}

void		logic_editor_panel::add_event_button_click			( Object^, System::EventArgs^ )
{
	logic_event^ new_event = gcnew logic_event( m_selected_object );
	m_selected_object->add_event(new_event);	
}

void logic_editor_panel::on_scene_child_added( object_logic^ obj )
{
	if (!obj->is_job())
		return;
	
	object_job^ job = safe_cast< object_job^ >( obj );
	wpf_logic_entity_view^ logic_entity_view = job->logic_view;

	m_cached_logic->logic_view->add_logic( logic_entity_view );	
}

void logic_editor_panel::show_object_logic		( object_logic^ object )
{
	if ( object != nullptr )
	{
		Text = "Logic: " + object->owner_project_item->get_full_name();
		m_selected_object = safe_cast<object_scene^>( object );
		m_selected_object->on_child_added += gcnew on_child_added( this, &logic_editor_panel::on_scene_child_added );
		add_job_button->Enabled = true;
		add_sub_scene->Enabled = true;
		add_event_button->Enabled = true;
		
		m_cached_logic = gcnew wpf_logic_view( );

		m_cached_logic->logic_view->scenes_view->item_text->Text = m_selected_object->get_name();

		m_cached_logic->Dock = DockStyle::Fill;	

		for each( object_job^ job in safe_cast< object_scene^ >( m_selected_object )->jobs )
		{
			job->initialize_logic_view();
			m_cached_logic->logic_view->add_logic( job->logic_view );			
		}

		for each( logic_event^ event in safe_cast< object_scene^ >( m_selected_object )->events )
		{
			m_cached_logic->logic_view->add_event( event->logic_event_view );			
		}

		wpf_controls::hypergraph::properties_node^ node = m_cached_logic->logic_view->scenes_view->logic_hypergraph->create_node( "THIS", m_selected_object->get_properties_container_for_own_view( ) , System::Windows::Point(0,0) );			
		node->name = "THIS";
		node->border_min_width = 200;

		node->Tag = this;

		node->type_id = "logic_scene";

		node->is_movable	= false;
		node->is_selectable	= false;

		m_selected_object->own_logic_node_in_own_view = node;

		for each( object_scene^ scene in m_selected_object->scenes )
		{
			m_selected_object->add_scene_to_logic_view( scene , false );
		}
		
		for each( object_scene^ scene in m_selected_object->scenes )
		{
			for each ( logic_scene_event_property_holder^ holder in scene->event_handlers )
			{
				if (holder->event_handler_type == "own_view_handler")
					continue;
				holder->add_logic_links();
			}
		}

		for each ( logic_scene_event_property_holder^ holder in m_selected_object->event_handlers )
		{
			if (holder->event_handler_type == "parent_view_handler")
				continue;
			holder->add_logic_links();
		}

		this->Controls->Add( m_cached_logic );
		this->Controls->SetChildIndex			( m_cached_logic, 0 );
	}
	else
	{
		Text = "Logic: ";

		m_selected_object->on_child_added -= gcnew on_child_added( this, &logic_editor_panel::on_scene_child_added );

		for each( object_job^ job in safe_cast< object_scene^ >( m_selected_object )->jobs )
		{
			m_cached_logic->logic_view->remove_logic( job->logic_view );
			job->clear_logic_view( );			
		}

		for each( logic_event^ event in safe_cast< object_scene^ >( m_selected_object )->events )
		{
			m_cached_logic->logic_view->remove_event( event->logic_event_view );			
		}

		m_selected_object = nullptr;

		add_job_button->Enabled = false;
		add_sub_scene->Enabled = false;
		add_event_button->Enabled = false;
		this->Controls->Remove( m_cached_logic );
	}
}

void logic_editor_panel::remove_logic_view			( object_job^ job )
{
	m_cached_logic->logic_view->remove_logic( job->logic_view );
}

}
}
