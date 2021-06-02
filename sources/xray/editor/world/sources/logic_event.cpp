////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "logic_event.h"
#include "object_scene.h"
#include "logic_event_subscriber.h"
#include "logic_event_properties.h"
#include "logic_event_link_chooser_form.h"
#include "project.h"

using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor {

logic_event::logic_event( object_scene^ parent_scene )
:m_parent_scene(parent_scene)
{
	m_logic_event_view		= gcnew wpf_logic_event( );	
	m_linked_events			= gcnew linked_events;
	m_linked_to_me_events	= gcnew linked_events;

	m_logic_event_view->event_name_changed			+= gcnew Action<String^>( this, &logic_event::on_event_name_changed );	
	m_logic_event_view->on_delete_event				= gcnew Action( this, &logic_event::on_before_delete );
	m_logic_event_view->on_view_event_properties	= gcnew Action( this, &logic_event::on_view_event_properties );
}

void logic_event::on_event_name_changed( String^ )
{
	if ( m_on_event_name_changed != nullptr )
		m_on_event_name_changed( this );	
}

void logic_event::save( configs::lua_config_value config )
{
	config["name"]		= unmanaged_string(m_logic_event_view->event_name).c_str();
	config["position"]	= (int)m_logic_event_view->logical_position;
	int i = 0;
	for each( logic_event^ e in m_linked_events )
	{
		configs::lua_config_value link_cfg = config["links"][unmanaged_string( "link" + i ).c_str()];
		i++;
		link_cfg["scene"] = unmanaged_string( e->parent_scene->owner_project_item->get_full_name() ).c_str();
		link_cfg["event"] = unmanaged_string( e->logic_event_view->event_name ).c_str();
	}

}
void logic_event::load( configs::lua_config_value config )
{
	m_logic_event_view->event_name	= gcnew System::String(config["name"]);

	if ( m_logic_event_view->event_name == "#StartScene" )
		logic_event_view->Visibility = System::Windows::Visibility::Collapsed;
	
	if (config.value_exists("position"))
		m_logic_event_view->logical_position	= (int)config["position"];

	m_loaded_link_names = gcnew Collections::Generic::List<String^>();

	int i = 0;
	for ( ; config["links"].value_exists( unmanaged_string( "link" + i ).c_str() );i++ )
	{
		configs::lua_config_value link_cfg = config["links"][unmanaged_string( "link" + i ).c_str()];
		project^ prj = m_parent_scene->get_project();
		System::String^ linked_event_scene_name = gcnew String( link_cfg["scene"] );
		System::String^ linked_event_name		= gcnew String( link_cfg["event"] );

		m_loaded_link_names->Add( linked_event_scene_name + "/" + linked_event_name );

		prj->query_project_item( linked_event_scene_name, gcnew queried_object_loaded( this, &logic_event::linked_event_scene_loaded ) );
	}
	
	if ( m_loaded_link_names->Count == 0 )
		m_loaded_link_names = nullptr;
}

void logic_event::linked_event_scene_loaded	( project_item_base^ scene )
{	
	if ( m_loaded_link_names == nullptr )
		return;

	object_scene^ obj_scene  = safe_cast<object_scene^>( scene->get_object( ) );
	
	for each( logic_event^ l_event in obj_scene->events )
	{
		if ( m_loaded_link_names->Contains( l_event->event_full_name ) )
		{
			m_linked_events->Add( l_event );

			l_event->subscribe_on_changed( this );
			l_event->linked_to_me_events->Add( this );

			m_loaded_link_names->Remove( l_event->event_full_name );
		}

	}
	if ( m_loaded_link_names->Count == 0 )
		m_loaded_link_names = nullptr;
}

void logic_event::on_before_delete( )
{
	m_parent_scene->events->Remove( this );
	if ( m_on_before_deletion != nullptr )
		m_on_before_deletion( this );
}

void logic_event::subscribe_on_changed		( logic_event_subscriber^ subscriber )
{
	m_on_before_deletion	+= gcnew on_before_event_deletion		( subscriber, &logic_event_subscriber::on_subscribed_logic_event_deletion );
	m_on_event_name_changed += gcnew on_logic_event_name_changed	( subscriber, &logic_event_subscriber::on_subscribed_logic_event_name_changed );
}

void logic_event::unsubscribe_on_changed	( logic_event_subscriber^ subscriber )
{
	m_on_before_deletion	-= gcnew on_before_event_deletion		( subscriber, &logic_event_subscriber::on_subscribed_logic_event_deletion );
	m_on_event_name_changed -= gcnew on_logic_event_name_changed	( subscriber, &logic_event_subscriber::on_subscribed_logic_event_name_changed );
}

void logic_event::on_view_event_properties( )
{
	m_properties_form = gcnew logic_event_properties();

	m_properties_form->property_grid_host->selected_object = get_properties_container( );

	m_properties_form->ShowDialog();

	m_properties_form = nullptr;
}


System::String^	logic_event::event_full_name::get()
{ 
	return parent_scene->owner_project_item->get_full_name() + "/" + logic_event_view->event_name ; 
}

property_container^	logic_event::get_properties_container( )
{
	property_container^ result = gcnew property_container;

	property_container_collection^ event_handlers_container = gcnew property_container_collection( "event{0}" );

	property_descriptor^ event_handlers_descriptor = gcnew property_descriptor("Links", "","", event_handlers_container ); 

	for each ( logic_event^ e in m_linked_events )
	{
		property_descriptor^ event_descriptor	= gcnew property_descriptor( e, "event_full_name" );
		event_descriptor->is_read_only			= true;
		event_descriptor->add_button			( "X", "", gcnew Action<button^>( this, &logic_event::remove_link_clicked ) )->tag = e;
		event_handlers_container->properties->add( event_descriptor );		
	}

	event_handlers_descriptor->add_button		( "+", "", gcnew Action<button^>( this, &logic_event::add_link_clicked ) );
	event_handlers_descriptor->is_expanded = true;
	result->properties->add( event_handlers_descriptor );		
	return result;
}

void	logic_event::add_link_clicked	( button^ )
{
	logic_event_link_chooser_form^ frm = gcnew logic_event_link_chooser_form;
	System::String^ initial_selected = /*(m_linked_event != nullptr) ? m_linked_event->parent_scene->owner_project_item->get_full_name() :*/ "";
	frm->initialize						( m_parent_scene->get_project() , initial_selected, "xray.editor.object_scene" );
	if ( frm->ShowDialog()==System::Windows::Forms::DialogResult::OK )
	{
		object_scene^ selected_scene = safe_cast<object_scene^>( m_parent_scene->get_project()->find_by_path( frm->selected_path, true )->get_object() );
		logic_event^ selected_logic_event = selected_scene->get_event_by_name( frm->selected_event );
		m_linked_events->Add( selected_logic_event );
		selected_logic_event->subscribe_on_changed( this );
		selected_logic_event->linked_to_me_events->Add( this );

	}
	m_properties_form->property_grid_host->selected_object = get_properties_container( );
}



void	logic_event::remove_link_clicked	( button^ btn )
{
	logic_event^ selected_logic_event =	safe_cast<logic_event^>( btn->tag );
	selected_logic_event->unsubscribe_on_changed( this );
	m_linked_events->Remove( selected_logic_event );
	selected_logic_event->linked_to_me_events->Remove( this );
	m_properties_form->property_grid_host->selected_object = get_properties_container( );
}


void logic_event::on_subscribed_logic_event_deletion		( logic_event^ event )
{
	if ( m_linked_events->Contains( event ) ){
		m_linked_events->Remove( event );
		event->unsubscribe_on_changed( this );
	}
}
void logic_event::on_subscribed_logic_event_name_changed	( logic_event^ /*event*/ )
{
}

} // namespace editor
} // namespace xray