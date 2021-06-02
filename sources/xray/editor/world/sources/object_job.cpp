////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "project.h"
#include "object_job.h"
#include "object_logic.h"
#include "object_composite.h"
#include "object_scene.h"
#include "tool_scripting.h"
#include "project_items.h"
#include "level_editor.h"
#include "logic_editor_panel.h"
#include "i_job_resource.h"
#include "job_behaviour_base.h"
#include "job_behaviour_wrapper.h"
#include "job_behaviour_camera_track.h"
#include "job_behaviour_camera_from_object.h"
#include "logic_event_property_holder.h"
#include "logic_event_action_switch.h"
#include "command_delete_in_logic_hypergraph.h"
#include "editor_world.h"
#include "logic_event_property_holder.h"
#include "logic_event_properties_container.h"
#include "logic_event_action_raise_event.h"
#include "command_select.h"

#pragma managed( push, off )
#include <xray/render/facade/debug_renderer.h>
#pragma managed( pop )


namespace xray{
namespace editor{

object_job::object_job( tool_scripting^ t )
	:super(t)
{
	image_index				= xray::editor_base::npc_tree_icon;
	m_behaviours_list		= gcnew behaviours_list();
	m_behaviours_query		= gcnew query_behaviours_list();
	m_start_event_name		= "#StartScene";
	m_logic_view_height		= 250.f;
}

object_job::~object_job()
{

}

void object_job::on_before_manual_delete	( )
{
	if (m_parent_scene != nullptr )
		m_parent_scene->remove_job( this );
}

void object_job::load_props( configs::lua_config_value const& t )
{
	super::load_props		(t);

	if (!t.value_exists("job_type"))
		return;

	if (t.value_exists("start_behaviour_name"))
		m_start_behaviour_name = gcnew System::String( t["start_behaviour_name"] );

	if (t.value_exists("start_event_name")){
		m_start_event_name = gcnew System::String( t["start_event_name"] );
		if ( m_start_event_name == "StartScene" )
			m_start_event_name = "#StartScene";		
	}

	if (t.value_exists("job_type"))
		job_type = gcnew System::String( t["job_type"] );

	if (t.value_exists("logic_view_height"))
		logic_view_height = t["logic_view_height"];

	if ( t.value_exists("job_resource") && !has_static_resource( ) ){
		if ( parent_composite == nullptr )	
			tool->get_level_editor()->get_project()->query_project_item(gcnew System::String(t["job_resource"]), gcnew xray::editor::queried_object_loaded(this, &object_job::on_job_resource_loaded) );
		else{
			System::String^ loaded_resource_name = gcnew System::String( t[ "job_resource" ] );
			if ( loaded_resource_name != "" )
			{
				m_job_resource = parent_composite->get_child_by_name( loaded_resource_name )->owner_project_item;
			}
		}
	
	}

	if ( t.value_exists("behaviours") )
	{
		configs::lua_config_iterator behaviours_begin	= t["behaviours"].begin();
		configs::lua_config_iterator behaviours_end		= t["behaviours"].end();

		for ( ;behaviours_begin != behaviours_end ; behaviours_begin++)
		{
			System::String^ behaviour_type	= gcnew System::String((*behaviours_begin)["behaviour_type"]);
			job_behaviour_base^ behaviour	= job_behaviour_wrapper::create_behaviour( behaviour_type, this );
		
			behaviour->behaviour_name = gcnew System::String(behaviours_begin.key());

			m_behaviours_list->Add( behaviour );		

			behaviour->load( *behaviours_begin );						
		}		
	}

	return_queried_behaviours( );

//on_parent_scene_changed += gcnew xray::editor::on_parent_scene_changed( this, &object_job::add_cached_links );
}


void object_job::save( configs::lua_config_value t )
{
	super::save				( t );

	t["job_type"]			= unmanaged_string(job_type).c_str();
		
	if (start_behaviour_name != nullptr)
		t["start_behaviour_name"]	= unmanaged_string(start_behaviour_name).c_str();

	t["start_event_name"]		= unmanaged_string(m_start_event_name).c_str();

	t["logic_view_height"]	= (int)(logic_view_height);
	if ( has_static_resource() )
		t["job_resource"] = unmanaged_string(m_static_resource_name).c_str();		
	else if ( m_job_resource == nullptr ) 
		t["job_resource"] = "";
	else
	{
		if ( parent_composite == nullptr )
			t["job_resource"] = unmanaged_string(m_job_resource->get_full_name()).c_str();
		else
		{
			t["job_resource"]	= unmanaged_string( m_job_resource->get_name() ).c_str();
		}
	}

	t["behaviours"].create_table();

	for each ( job_behaviour_base^ behaviour in m_behaviours_list )
	{
		configs::lua_config_value save_to_val = t["behaviours"][unmanaged_string(behaviour->behaviour_name).c_str()];
		behaviour->save( save_to_val );	
	}
}

void object_job::render( )
{
	super::render	();

 	if( m_project_item->get_selected() )
 	{
		math::color clr		( 128, 0, 128, 255 );
		float tmp			= 0.5f;
		float3 position		= m_transform->c.xyz();
		position.y			+= 1;
		get_debug_renderer().draw_sphere( get_editor_world().scene(), position, tmp, clr);
	}

	get_debug_renderer().draw_cross( get_editor_world().scene(), m_transform->c.xyz(), 1.f, math::color(0xffffff00) );
}

void object_job::on_selected				( bool bselected )
{
	super::on_selected( bselected );
	
	if ( m_logic_view != nullptr )
		m_logic_view->is_selected = bselected;

	if ( m_parent_scene->own_logic_view != nullptr )
	{
		System::Windows::Point selected_item_position = logic_view->TranslatePoint( System::Windows::Point( 0, 0 ), m_parent_scene->own_logic_view->cached_logic->logic_view->logic_entities_list );
		if ( m_parent_scene->own_logic_view->cached_logic->logic_view->field_scroller->VerticalOffset > selected_item_position.Y )
			m_parent_scene->own_logic_view->cached_logic->logic_view->field_scroller->ScrollToVerticalOffset( selected_item_position.Y );

		Double bottom_offset = ( selected_item_position.Y + logic_view_height ) - ( m_parent_scene->own_logic_view->cached_logic->logic_view->field_scroller->VerticalOffset + m_parent_scene->own_logic_view->cached_logic->logic_view->field_scroller->ActualHeight );

		if ( bottom_offset > 0 )
			m_parent_scene->own_logic_view->cached_logic->logic_view->field_scroller->ScrollToVerticalOffset( m_parent_scene->own_logic_view->cached_logic->logic_view->field_scroller->VerticalOffset + bottom_offset );

	}
}

void object_job::on_logic_view_property_changed			( System::Object^, System::ComponentModel::PropertyChangedEventArgs^ e )
{
	if ( e->PropertyName == "is_selected" && owner_project_item->m_tree_node_ != nullptr )
		if ( m_logic_view->is_selected )
			tool->get_level_editor()->get_command_engine()->run( gcnew command_select( tool->get_level_editor()->get_project(), owner_project_item, enum_selection_method_set ) );
		else
			tool->get_level_editor()->get_command_engine()->run( gcnew command_select( tool->get_level_editor()->get_project() ) );
		//tool->get_level_editor()->get_project( )->select_object( owner_project_item, enum_selection_method_set );
}

void object_job::initialize_logic_view		( )
{
	m_logic_view			= gcnew wpf_logic_entity_view(  );
	
	m_logic_view->logic_view_height = m_logic_view_height;

	m_logic_view->item_text->Text =  job_type;
	
	

	if ( has_static_resource( ) ){
		m_logic_view->logic_entity_resource_button->IsEnabled = false;
		m_logic_view->selected_resource->Text = "Selected Resource: " + m_static_resource_name;
		m_logic_view->logic_entity_resource_button_border->BorderBrush = System::Windows::Media::Brushes::DarkGray;
	}
	else
		m_logic_view->selected_resource->Text = ( m_job_resource != nullptr ) ? "Selected Resource: " + m_job_resource->get_full_name() : "Selected Resource: none" ;

	m_logic_view->get_events_list = gcnew System::Func<Generic::List<System::String^>^>( this, &object_job::get_parent_scene_events_list );
	m_logic_view->set_start_event = gcnew System::Action<System::String^>( this, &object_job::start_event_name::set );

	/*m_logic_view->start_event_combobox->Items->Add(m_start_event_name);*/
	m_logic_view->start_event_combobox->SelectedItem = m_start_event_name;

	m_set_start_behaviour_item	= m_logic_view->logic_hypergraph->add_context_menu_item( "Set selected as start behaviour" );
	m_add_new_behaviour_item	= m_logic_view->logic_hypergraph->add_context_menu_item( "Add new behaviour" );
	m_delete_selected_item		= m_logic_view->logic_hypergraph->add_context_menu_item( "Delete selected" );

	m_logic_view->select_object_button_clicked			= gcnew System::Action(this, &object_job::select_resource_button_clicked);
	m_logic_view->add_behaviour_button_clicked			= gcnew System::Action(this, &object_job::add_new_behaviour );
	m_logic_view->logic_hypergraph->link_creating		+= gcnew System::EventHandler<link_event_args^>(this, &object_job::on_link_creating);
	m_logic_view->logic_hypergraph->selecting			+= gcnew System::EventHandler<selection_event_args^>( this, &object_job::on_hypergraph_selecting );

	m_logic_view->logic_hypergraph->context_menu_opening+= gcnew System::Action(this, &object_job::logic_hypergraph_context_menu_opening);
	m_logic_view->logic_hypergraph->menu_item_click		+= gcnew System::EventHandler<menu_event_args^>(this, &object_job::logic_hypergraph_menu_item_click);
	m_logic_view->PropertyChanged						+= gcnew System::ComponentModel::PropertyChangedEventHandler( this, &object_job::on_logic_view_property_changed );


	for each( job_behaviour_base^ behaviour in m_behaviours_list ){
		job_behaviour_wrapper^ behaviour_wrapper = gcnew job_behaviour_wrapper( this, true );

		behaviour_wrapper->current_behaviour = 	behaviour;	
		behaviour->get_properties( behaviour_wrapper->properties_container );

		wpf_property_container^ prop_container = gcnew wpf_property_container();
		wpf_property_descriptor^ prop_descriptor = gcnew wpf_property_descriptor("behaviour","","", behaviour_wrapper->properties_container );
		prop_descriptor->dynamic_attributes->add( gcnew xray::editor::wpf_controls::hypergraph::node_property_attribute( false, false ));
		prop_container->properties->add( prop_descriptor );
		prop_descriptor->is_expanded = true;

		if ( behaviour->logic_node_position.Y < 0 )
			behaviour->logic_node_position.Y = 0;

		if ( behaviour->logic_node_position.X < 0 )
			behaviour->logic_node_position.X = 0;

		wpf_hypergpaph_node^ node = m_logic_view->logic_hypergraph->create_node( behaviour->behaviour_name, prop_container, behaviour->logic_node_position );

		node->Tag = behaviour_wrapper;

		node->type_id = "logic_behaviour";

		node->input_link_place_enabled = true;

		node->name = behaviour->behaviour_name;

		if (node->id == m_start_behaviour_name)
			node->name = "*" + node->name;

		behaviour_wrapper->hypergraph_node = node;
		behaviour_wrapper->current_behaviour->hypergraph_node = node;
	}	
	for each( job_behaviour_base^ behaviour in m_behaviours_list )
	{
		for each ( logic_event_property_holder^ holder in behaviour->events_container->m_events_list )
		{
			holder->add_logic_links();
		}
	}
}

void object_job::clear_logic_view	( )
{
	for each ( job_behaviour_base^ behaviour in m_behaviours_list )
	{
		behaviour->logic_node_position = behaviour->hypergraph_node->position;
	}
	logic_view_height = (float)m_logic_view->logic_view_height;
	delete m_logic_view;
	m_logic_view = nullptr;
}

void object_job::on_link_creating(System::Object^, link_event_args^ e)
{

	if (e->link_id->input_link_point_id != "input" )
		return;

	if (e->link_id->output_link_point_id == "input")
		return;
	
	m_logic_view->logic_hypergraph->create_link(e->link_id);
	logic_event_property_holder^ event_holder = safe_cast<logic_event_property_holder^>(e->source_tag);
	job_behaviour_base^ destination_behaviour = 	(safe_cast<job_behaviour_wrapper^>(e->destination_tag))->current_behaviour;
	event_holder->add_switch_action(gcnew logic_event_action_switch( event_holder, destination_behaviour ) );
}

void object_job::add_new_behaviour			( )
{
	Generic::List<String^>^ behaviour_names_list = gcnew Generic::List<String^>( );
	for each ( job_behaviour_base^ behaviour in m_behaviours_list )
	{
		ASSERT( !behaviour_names_list->Contains( behaviour->behaviour_name ) , "Wrong event name detected! Already in use!" );
		behaviour_names_list->Add( behaviour->behaviour_name );
	}
	
	int index = 0;
	
	for( ; behaviour_names_list->Contains("behaviour" + index); index++ ){ }

	System::String^ new_behaviour_name = "behaviour" + index;

	job_behaviour_wrapper^ behaviour_wrapper = gcnew job_behaviour_wrapper( this, false );

	behaviour_wrapper->current_behaviour->behaviour_name = new_behaviour_name;
		
	wpf_property_container^ prop_container = gcnew wpf_property_container();
	wpf_property_descriptor^ prop_descriptor = gcnew wpf_property_descriptor("behaviour","","", behaviour_wrapper->properties_container );
	prop_descriptor->dynamic_attributes->add( gcnew xray::editor::wpf_controls::hypergraph::node_property_attribute( false, false ));
	prop_container->properties->add( prop_descriptor );
	prop_descriptor->is_expanded = true;

 	wpf_hypergpaph_node^ node = m_logic_view->logic_hypergraph->create_node( new_behaviour_name, prop_container, System::Windows::Point( ( 220 ) * ( m_behaviours_list->Count - 1 ), 10 ) );

	node->Tag = behaviour_wrapper;

	node->border_min_width = 210;
 
 	node->type_id = "logic_behaviour";
 
 	node->input_link_place_enabled = true;
 
 	node->name = new_behaviour_name;

	if (m_logic_view->logic_hypergraph->nodes->Count == 1)
		start_behaviour_name = node->id;

	behaviour_wrapper->hypergraph_node = node;
	behaviour_wrapper->current_behaviour->hypergraph_node = node;
}
void object_job::return_queried_behaviours				( )
{
	for each( query_behaviour^ query_beh in m_behaviours_query )
	{
		bool success = false;
		System::String^ query_name = query_beh->behaviour_name;
		for each(job_behaviour_base^ b in m_behaviours_list)
		{
			if(b->behaviour_name==query_name)
			{
				query_beh->delegate_on_loaded( b );
				success = true;
				break;
			}
		}
		R_ASSERT(success, "behaviour not found" );
	}
	m_behaviours_query->Clear();
}

void object_job::select_resource_button_clicked	()
{
	System::String^ selected;
	
	System::String^ current_selected = nullptr;

	current_selected = "";

	if (m_job_resource != nullptr)
		current_selected = m_job_resource->get_full_name();

	bool result = false;
	if ( parent_composite != nullptr )
	{
		current_selected = parent_composite->get_full_name() + "," + current_selected;
		result = editor_base::resource_chooser::choose( "compound_item", current_selected, tool_scripting::job_resource_type_by_job_type[job_type], selected, true );
	}
	else
		result = editor_base::resource_chooser::choose( "project_item", current_selected, tool_scripting::job_resource_type_by_job_type[job_type], selected, true );
	if ( result )
	{
		m_job_resource = tool->get_level_editor()->get_project()->find_by_path(selected, true);
		m_logic_view->selected_resource->Text = "Selected Resource: " + selected;
	}
	else
	{
		m_job_resource = nullptr;
		m_logic_view->selected_resource->Text = "Selected Resource: none";
	}
	
	
}

void object_job::on_job_resource_loaded	( project_item_base^ object )
{
	m_job_resource = object; 	
}

void object_job::query_logic_behaviour		( System::String^ behaviour_name, queried_behaviour_loaded^ delegate_on_loaded ){
	query_behaviour^ query_obj = gcnew query_behaviour();
	query_obj->behaviour_name = behaviour_name;
	query_obj->delegate_on_loaded = delegate_on_loaded;
	m_behaviours_query->Add(query_obj);
}

void object_job::show_logic						( )
{
	m_parent_scene->show_logic( );
}

void object_job::logic_hypergraph_context_menu_opening	( )
{
	m_set_start_behaviour_item->Visibility	= ( m_logic_view->logic_hypergraph->selected_nodes->Count != 1 ) ? System::Windows::Visibility::Collapsed : System::Windows::Visibility::Visible;
	m_delete_selected_item->Visibility		= ( m_logic_view->logic_hypergraph->selected_nodes->Count == 0 && m_logic_view->logic_hypergraph->selected_links->Count == 0 ) ? System::Windows::Visibility::Collapsed : System::Windows::Visibility::Visible;

}
void object_job::logic_hypergraph_menu_item_click		( System::Object^, menu_event_args^ args )
{
	if ( args->menu_item == m_set_start_behaviour_item )
	{
		wpf_controls::hypergraph::node^ selected_node = nullptr;
		for each( wpf_controls::hypergraph::node^ node in m_logic_view->logic_hypergraph->selected_nodes )
		{
			selected_node = node;
			break;
		}
		start_behaviour_name = selected_node->id;		
	}
	else if ( args->menu_item == m_add_new_behaviour_item )
	{
		add_new_behaviour();
	}
	else if ( args->menu_item == m_delete_selected_item )
	{
		tool->get_level_editor()->get_command_engine()->run(gcnew command_delete_in_logic_hypergraph( this ));
	}
}

void object_job::start_behaviour_name::set( System::String^ value )
{
	if (m_start_behaviour_name == value)
		return;

	if (m_start_behaviour_name != nullptr && m_logic_view->logic_hypergraph->nodes->ContainsKey(m_start_behaviour_name))
	{
		wpf_hypergpaph_node^ node = (wpf_hypergpaph_node^)m_logic_view->logic_hypergraph->nodes[m_start_behaviour_name]; 
		node->name = node->name->Substring(1, node->name->Length - 1);
	}

	m_start_behaviour_name = value;
	wpf_hypergpaph_node^ new_node = (wpf_hypergpaph_node^)m_logic_view->logic_hypergraph->nodes[m_start_behaviour_name]; 	
	new_node->name = "*" + new_node->name;
}

Generic::List<System::String^>^ object_job::get_parent_scene_events_list()
{
	return safe_cast<Collections::Generic::List<String^>^>(parent_scene->get_children_by_type("event"));
}

void object_job::remove_behaviour( System::String^ behaviour_id )
{
	job_behaviour_base^ behaviour_to_remove = nullptr;

	for each( job_behaviour_base^ behaviour in m_behaviours_list )
	{
		if ( behaviour->behaviour_name == behaviour_id ){
			behaviour_to_remove = behaviour;
			break;
		}
	}

	Generic::List<link^>^ links_to_delete = gcnew Generic::List<link^>();
	for each ( link^ lnk in m_logic_view->logic_hypergraph->links->Values )
	{
		if ( lnk->id->input_node_id == behaviour_id || lnk->id->output_node_id == behaviour_id )
		{
			links_to_delete->Add( lnk );
		}
	}

	for each( link^ lnk in links_to_delete )
	{
		remove_link( lnk );
	}

	m_logic_view->logic_hypergraph->remove_node( behaviour_id );
	
	m_behaviours_list->Remove(behaviour_to_remove);
	if (behaviour_id == start_behaviour_name){
		if (m_behaviours_list->Count != 0)
			start_behaviour_name = m_behaviours_list[0]->behaviour_name;
		else
			m_start_behaviour_name = nullptr;
	}
}
void object_job::remove_link				( link^ lnk )
{
	logic_event_property_holder^ event_holder = safe_cast<logic_event_property_holder^>( lnk->source_tag );
	event_holder->remove_switch_action();
	//m_logic_view->logic_hypergraph->remove_link( lnk->id );
}

bool object_job::has_static_resource						()
{
	if ( job_type == "camera_director" || job_type == "timing" )
		return true;
	return false;
}

void object_job::job_type::set( System::String^ value ){ 
	m_job_type = value;
	if ( has_static_resource( ) ){
		m_static_resource_name = job_type;		
	}
}

void object_job::on_hypergraph_selecting( System::Object^, selection_event_args^ e )
{
	m_logic_view->logic_hypergraph->deselect_all_links();
	m_logic_view->logic_hypergraph->deselect_all_nodes();

	for each( wpf_controls::hypergraph::link_id^ lnk_id in e->selected_link_ids )
	{
		m_logic_view->logic_hypergraph->select_link( lnk_id );
	}
	for each( String^ node_id in e->selected_node_ids )
	{
		m_logic_view->logic_hypergraph->select_node( node_id );
	}
}


List<logic_event^>^	object_job::get_connected_logic_events()
{
	List<logic_event^>^ events_lst = gcnew List<logic_event^>( );
	for each( job_behaviour_base^ behaviour in m_behaviours_list )
	{
		for each( logic_event_property_holder^ holder in behaviour->events_container->m_events_list )
		{
			if ( holder->get_property_type() != "" )
			{	
				logic_event^ evnt = parent_scene->get_event_by_name( holder->get_property_type() );
				if ( !events_lst->Contains( evnt ) )
					events_lst->Add( evnt );
			}
			for each ( logic_event_action_base^ event_action in holder->actions_list )
			{
				if ( event_action->GetType() == logic_event_action_raise_event::typeid )
				{
					logic_event^ evnt = safe_cast< logic_event_action_raise_event^ >( event_action )->raised_event;
					if ( !events_lst->Contains( evnt ) )
							events_lst->Add( evnt );
				}
			}
		}
	}
	return events_lst;
}

}
}

