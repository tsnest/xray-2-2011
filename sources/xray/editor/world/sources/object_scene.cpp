\////////////////////////////////////////////////////////////////////////////
//	Created		: 24.11.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_scene.h"
#include "object_job.h"
#include "logic_event.h"
#include "tool_scripting.h"
#include "project_items.h"
#include "level_editor.h"
#include "logic_editor_panel.h"
#include "command_add_library_object.h"
#include "project.h"
#include "editor_world.h"
#include "window_ide.h"
#include "logic_scene_event_property_holder.h"
#include "logic_event_action_start_scene.h"
#include "command_delete_in_scene_logic_hypergraph.h"
#include "logic_event_action_raise_event.h"

#pragma managed( push, off )
#include <xray/render/facade/editor_renderer.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/collision/collision_object.h>
#pragma managed( pop )

using namespace xray::editor::wpf_controls;
using namespace xray::editor::wpf_controls::control_containers;

namespace xray{
namespace editor{

object_scene::object_scene( tool_scripting^ t, bool is_root_scene )
	:super(t),
	m_is_root_scene( is_root_scene )
{
	image_index			= xray::editor_base::folder_closed;
	m_jobs_list			= gcnew jobs_list();
	m_sub_scenes_list	= gcnew scenes_list();
	m_events_list		= gcnew events_list();
	m_event_handlers	= gcnew event_handlers_list();

	m_own_logic_view	= nullptr;
	m_scenes_view_height= 150.f;
}

void object_scene::initialize				( )
{
	logic_event^ base_event = gcnew logic_event( this );
	base_event->logic_event_view->event_name = "#StartScene";
	add_event( base_event );
}

object_scene::~object_scene()
{
	if (m_own_logic_view != nullptr)
		m_own_logic_view->Close();
	m_sub_scenes_list->Clear();
	m_jobs_list->Clear();
}

void object_scene::on_before_manual_delete	( )
{

	if ( m_parent_scene != nullptr )
		m_parent_scene->remove_scene( this );

	project^ proj = m_tool_scripting->get_level_editor()->get_project();

	scenes_list sub_scenes_lst( m_sub_scenes_list );

	for each(object_scene^ scene in  sub_scenes_lst ){
		proj->remove( project_item_base::object_by_id(scene->id())->get_full_name() , true );
	}

	m_sub_scenes_list->Clear();

	jobs_list jobs_lst(m_jobs_list);

	for each(object_job^ job in  jobs_lst ){
		if ( project_item_base::object_by_id(job->id())->get_full_name() != nullptr )
			proj->remove( project_item_base::object_by_id(job->id())->get_full_name() , true );
	}

	m_jobs_list->Clear();
	
	events_list events_list(m_events_list);
	for each ( logic_event^ l_event in events_list )
	{
		l_event->on_before_delete();
	}
}

void object_scene::load_props( configs::lua_config_value const& t )
{
	super::load_props		(t);
	
	if (t.value_exists("finalizing_event_name")){
		finalizing_event_name = gcnew System::String( t["finalizing_event_name"] );		
	}

	if ( t.value_exists( "hypergraph_node" ) )
	{
		float2 loaded_position	= (float2)t["hypergraph_node"]["position"];
		m_scene_node_position	= System::Windows::Point( loaded_position.x, loaded_position.y );

		if (t["hypergraph_node"].value_exists("scenes_view_height"))
			m_scenes_view_height= (float)t["hypergraph_node"]["scenes_view_height"];
	}

	configs::lua_config_iterator it		= (t)["events"].begin();
	configs::lua_config_iterator it_end	= (t)["events"].end();

	for(; it != it_end; ++it)
	{
		logic_event^ loaded_event = gcnew logic_event( this );
		loaded_event->load( *it );
		m_events_list->Add( loaded_event );
	}

	bool loading_with_jobs_order_check = t.value_exists("jobs_order");

	if ( loading_with_jobs_order_check )
	{
		it		= t["jobs_order"].begin();
		it_end	= t["jobs_order"].end();
	}
	else
	{
		it		= t["jobs"].begin();
		it_end	= t["jobs"].end();
	}

	for(; it != it_end; ++it)
	{
		pcstr job_guid						= ( loading_with_jobs_order_check ) ? *it : it.key();
		configs::lua_config_value current	= ( loading_with_jobs_order_check ) ? t["jobs"][job_guid] : *it;
		
		add_child_from_cfg( gcnew System::String( job_guid ), current );
	}

	it		= (t)["sub_scenes"].begin();
	it_end	= (t)["sub_scenes"].end();

	for(; it != it_end; ++it)
	{
		pcstr scene_guid			= (pcstr)*it;		
		add_child_from_cfg( gcnew System::String(scene_guid), t.get_parent()[scene_guid] );
	}

	int i = 0;
	for( ; t["event_handlers"].value_exists(unmanaged_string( "event" + i ).c_str( )); i++)
	{
		if ( !t["event_handlers"][unmanaged_string( "event" + i ).c_str( )].value_exists("handler_type") )
			continue;

		logic_scene_event_property_holder^ holder = gcnew logic_scene_event_property_holder( this );
		holder->load( t["event_handlers"][unmanaged_string( "event" + i ).c_str( )]);	
		if ( holder->event_handler_type == "parent_view_handler" )
		{
			holder->own_property_descriptor->dynamic_attributes->add
			(
				gcnew editor::wpf_controls::property_editors::attributes::combo_box_items_attribute
				( 
					"event",
					gcnew System::Func< System::String^ , Generic::IEnumerable<System::String^>^>( holder, &logic_scene_event_property_holder::get_available_scenes_events ) 
				)
			);
		}

		m_event_handlers->Add( holder );
	}
}


void object_scene::save( configs::lua_config_value t )
{
	super::save				( t );

	t["game_object_type"]	= "scene";
	
	if ( finalizing_event_name != nullptr )
		t["finalizing_event_name"]	= unmanaged_string( finalizing_event_name ).c_str();
	
	t["hypergraph_node"]["position"] = float2( (float)scene_node_position.X , (float)scene_node_position.Y );
	t["hypergraph_node"]["scenes_view_height"] = scenes_view_height;

//	t["registry_name"]		= unmanaged_string( m_project_item->get_full_name()).c_str();



	configs::lua_config_value jobs = t["jobs"].create_table();
	
	int index = 1;
	for each(object_job^ job in  m_jobs_list )
	{
		project_item_object^ pi = job->owner_project_item;
		t["jobs_order"][index] = unmanaged_string( pi->m_guid.ToString() ).c_str();
		configs::lua_config_value current = jobs[unmanaged_string(pi->m_guid.ToString()).c_str()];
		job->save				( current );
		index++;
	}
	
	configs::lua_config_value sub_scenes = t["sub_scenes"].create_table();
	index = 1;
	for each(object_scene^ scene in  m_sub_scenes_list )
	{
		project_item_object^ pi = scene->owner_project_item;
		sub_scenes[index] = unmanaged_string( pi->m_guid.ToString() ).c_str();
		configs::lua_config_value current = t.get_parent()[unmanaged_string(pi->m_guid.ToString()).c_str()];
		scene->save				( current );
		index++;
	}
	
	configs::lua_config_value event_handlers = t["event_handlers"].create_table();
	int i = 0;
	for each( logic_scene_event_property_holder^ holder in m_event_handlers )
	{
		holder->save( event_handlers[unmanaged_string( "event" + i ).c_str( )] );
		i++;
	}

	configs::lua_config_value events = t["events"].create_table();
	index = 0;
	for each( logic_event^ event in  m_events_list )
	{
		event->save( events[ unmanaged_string("event" + index ).c_str() ] );

		if ( event->linked_to_me_events->Count != 0 )
		{
			int already_saved_event_handlers = event_handlers.size();
			configs::lua_config_value new_event_handler_cfg = event_handlers[unmanaged_string( "event" + already_saved_event_handlers ).c_str( )];
			new_event_handler_cfg["event_name"] = unmanaged_string( event->logic_event_view->event_name ).c_str();
			new_event_handler_cfg["is_global"]	= true;
			new_event_handler_cfg["actions"].create_table();
			int index = 0;
			for each( logic_event^ linked_event in event->linked_to_me_events )
			{
				configs::lua_config_value new_event_action_cfg = new_event_handler_cfg["actions"][unmanaged_string("action" + index).c_str()];
				new_event_action_cfg["action_type"]			= "raise_event";
				new_event_action_cfg["event_parent_scene"]	= unmanaged_string(linked_event->parent_scene->owner_project_item->get_full_name()).c_str();
				new_event_action_cfg["raised_event"]		= unmanaged_string(linked_event->logic_event_view->event_name).c_str();
				index++;
			}	
		}

		index++;
	}
}

void object_scene::unload_contents			( bool bdestroy ) 
{
	super::unload_contents( bdestroy );	
}

void object_scene::render( )
{
	if ( m_is_root_scene )
		return;

	super::render	();
 	if( m_project_item->get_selected() )
 	{
		float tmp = 0.5f;
		float3 position = m_transform->c.xyz();
		position.y += 1;
		get_debug_renderer().draw_sphere( get_editor_world().scene(), position, tmp, math::color(0xffffff00));
	}

	get_debug_renderer().draw_cross( get_editor_world().scene(), m_transform->c.xyz(), 1.f, math::color(0xffffff00) );
}

void object_scene::add_job		( project_item_object^ obj )
{
	object_job^ job = safe_cast<object_job^>(obj->get_object());
	job->job_type = m_own_logic_view->get_selected_job_type();
	m_jobs_list->Add( job );
	job->parent_scene = this ;
	job->initialize_logic_view();
	on_child_added( job );
	job->owner_project_item->fill_tree_view				( m_project_item->m_tree_node_->nodes );
	
}

object_logic^ object_scene::add_child_from_cfg		( System::String^ child_guid, configs::lua_config_value const& current )
{

	System::String^ child_library_name = gcnew System::String( current["lib_name"] );
	System::String^ new_child_name = gcnew System::String( current["name"] );
	
	if ( owner_project_item != nullptr )
		owner_project_item->check_correct_existance( new_child_name, 1 );

	object_logic^ new_child = nullptr;

	if ( child_library_name == "job" ){
		project_item_object^ item				= gcnew project_item_object( get_project() );
		item->m_guid							= System::Guid( child_guid );
		lib_item^ li							= tool->m_library->extract_library_item( current );
		System::String^ ln						= gcnew System::String((*(li->m_config))["lib_name"]);
		item->m_object_base						= tool->create_raw_object( ln );
		
		item->m_object_base->owner_project_item	= item;	
		safe_cast<object_logic^>(item->m_object_base)->parent_scene = this;

		item->m_object_base->load_props			( current );
		item->m_object_base->set_library_name	( li->full_path_name() );	

		item->assign_id( 0 );

		object_job^ job = safe_cast<object_job^>(item->get_object());
		job->set_name				( new_child_name, true );
		
		m_jobs_list->Add( job );
		on_child_added( job );		
		new_child = job;
	}
	else if ( child_library_name == "scene" )
	{
		project_item_object^ item	= gcnew project_item_object( get_project() );
		item->m_node_type			= controls::tree_node_type::group_item;
		System::String^ guid_str	= gcnew System::String ( child_guid );
		item->m_guid				= System::Guid(guid_str);

		lib_item^ li							= tool->m_library->extract_library_item( current );
		System::String^ ln						= gcnew System::String((*(li->m_config))["lib_name"]);
		item->m_object_base						= tool->create_raw_object( ln );
		item->m_object_base->owner_project_item	= item;
		safe_cast<object_logic^>(item->m_object_base)->parent_scene = this;
		item->m_object_base->load_props			( *li->m_config );
		item->m_object_base->load_props			( current );
		item->m_object_base->set_library_name	( li->full_path_name() );
		
		item->assign_id( 0 );

		object_scene^ scene = safe_cast<object_scene^>(item->get_object());
		scene->set_name				( new_child_name, true );

		m_sub_scenes_list->Add( scene );

		on_child_added( scene );	

		if ( m_own_logic_view != nullptr )
		{
			add_scene_to_logic_view( scene, true );
		}

		new_child = scene;
	}
	else
	{
		UNREACHABLE_CODE();
	}
// 
// 	if ( owner_project_item != nullptr ){
// 		System::String^ new_child_name = new_child->get_name();
// 		if (  owner_project_item->check_correct_existance( new_child_name, 1 ) )
// 			new_child->set_name				( new_child_name, true );	
// 	}

	return new_child;
}


void object_scene::add_scene	( project_item_object^ obj )
{
	object_scene^ scene = safe_cast<object_scene^>(obj->get_object());
	m_sub_scenes_list->Add( scene );
	scene->parent_scene = this ;
	on_child_added( scene );
	scene->owner_project_item->m_node_type = controls::tree_node_type::group_item;
	scene->owner_project_item->fill_tree_view( m_project_item->m_tree_node_->nodes );
	
	if ( m_own_logic_view != nullptr )
	{
		add_scene_to_logic_view( scene, true );
	}
}

void object_scene::add_scene_to_logic_view( object_scene^ scene , bool is_new_scene )
{
	hypergraph::properties_node^ node = m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->create_node(scene->get_name(), scene->get_properties_container_for_parent_view( ) , ( !is_new_scene ) ? scene->scene_node_position : System::Windows::Point( ( 210 ) * ( m_sub_scenes_list->Count ), 10 ) );			
	node->name = scene->get_name();
	node->border_min_width = 200;
	node->Tag = scene;

	node->type_id = "logic_scene";

	node->input_link_place_enabled = true;

	scene->own_logic_node_in_parent_view = node;
}

void object_scene::add_event ( logic_event^ event )
{
	if ( event->logic_event_view->event_name == nullptr ){
		Generic::List<String^>^ event_names_list = gcnew Generic::List<String^>( m_events_list->Count );
		for each ( logic_event^ logic_event in m_events_list )
		{
			ASSERT( !event_names_list->Contains( logic_event->logic_event_view->event_name ) , "Wrong event name detected! Already in use!" );
			event_names_list->Add( logic_event->logic_event_view->event_name );
		}

		int index = 0;

		for( ; event_names_list->Contains("new_event"+index); index++ ){ }
		
		event->logic_event_view->event_name = "new_event" + index;
	}
	m_events_list->Add( event );
	event->logic_event_view->set_default_position();	
	if ( m_own_logic_view != nullptr ){
		m_own_logic_view->cached_logic->logic_view->add_event( event->logic_event_view );		
	}

}


void object_scene::remove_job		( object_job^ obj )
{
	if (m_own_logic_view != nullptr)
		m_own_logic_view->remove_logic_view(obj);
	m_jobs_list->Remove( obj );
}

void object_scene::remove_scene	( object_scene^ scene )
{
	m_sub_scenes_list->Remove(scene);
	if ( m_own_logic_view != nullptr ){
		Generic::List<link^> links( own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->links->Values );
		for each ( link^ lnk in links )
		{
			if ( lnk->id->input_node_id == scene->own_logic_node_in_parent_view->id || lnk->id->output_node_id == scene->own_logic_node_in_parent_view->id )
			{
				safe_cast<logic_scene_event_property_holder^>(lnk->source_tag)->remove_start_scene_action( scene );
				own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->remove_link( lnk->id );
			}
		}
		if ( scene->own_logic_node_in_parent_view != nullptr )
			own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->remove_node( scene->own_logic_node_in_parent_view->id );
	}
}

void object_scene::queried_object_loaded( project_item_base^ obj )
{
	object_logic^ obj_logic = safe_cast<object_logic^>( obj->get_object() );
	if ( obj_logic->is_scene( ) )
		add_scene( safe_cast<project_item_object^>( obj ) );
	else if( obj_logic->is_job( ) )
		add_job( safe_cast<project_item_object^>( obj ) );	
}

void object_scene::on_selected				( bool bselected )
{
	super::on_selected( bselected );
}

Generic::IEnumerable<String^>^ object_scene::get_children_by_type	(String^ children_type)
{
	Generic::List<String^>^ ret_lst = gcnew Generic::List<String^>();
	if (children_type == "event")
	{	
		for each( logic_event^ event in m_events_list )
		{
			ret_lst->Add(event->logic_event_view->event_name);
		}
	}
	return ret_lst;
}

logic_event^ object_scene::get_event_by_name		( String^ event_name )
{
	for each(logic_event^ event in m_events_list)
	{
		if (event->logic_event_view->event_name == event_name)
			return event;
	}	
	return nullptr;
}

void object_scene::owner_project_item::set( project_item_object^ pi )
{ 
	m_project_item = pi;
	for each( object_job^ job in m_jobs_list ){		
		m_project_item->add( job->owner_project_item );
	}
	for each( object_scene^ scene in m_sub_scenes_list ){		
		m_project_item->add( scene->owner_project_item );		
	}
}

void object_scene::show_logic				( )
{
	if ( m_own_logic_view == nullptr )
	{
		m_own_logic_view = gcnew logic_editor_panel( tool->get_level_editor( ) );
		m_own_logic_view->show_object_logic( this );
		m_own_logic_view->cached_logic->logic_view->scenes_view->logic_view_height = m_scenes_view_height;
		
		m_own_logic_view->HideOnClose = false;
		m_own_logic_view->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler( this, &object_scene::on_logic_view_closing );
		tool->get_level_editor()->ide()->add_window( m_own_logic_view, WeifenLuo::WinFormsUI::Docking::DockState::Document );
		
		m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->link_creating		+= gcnew System::EventHandler< link_event_args^ >( this, &object_scene::on_scene_link_creating);
		
		m_add_new_scene_item		= m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->add_context_menu_item( "Add new scene" );
		m_delete_selected_item		= m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->add_context_menu_item( "Delete selected" );

		m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->selecting			+= gcnew System::EventHandler<selection_event_args^>( this, &object_scene::on_hypergraph_selecting );

		m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->context_menu_opening	+= gcnew System::Action(this, &object_scene::logic_hypergraph_context_menu_opening);
		m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->menu_item_click		+= gcnew System::EventHandler<menu_event_args^>(this, &object_scene::logic_hypergraph_menu_item_click);
	}
	else
		m_own_logic_view->Show();

	m_project_item->m_tree_node_->Expand();

	wpf_controls::logic_view::logic_entity_view^ selected_view =  ( m_own_logic_view->cached_logic->logic_view->get_selected_views()->Count != 0 ) ? m_own_logic_view->cached_logic->logic_view->get_selected_views()[0] : nullptr ;
	if ( selected_view != nullptr ){
		System::Windows::Point selected_item_position = selected_view->TranslatePoint( System::Windows::Point( 0, 0 ), m_own_logic_view->cached_logic->logic_view->logic_entities_list );
		m_own_logic_view->cached_logic->logic_view->field_scroller->ScrollToVerticalOffset( selected_item_position.Y );
	}
}

void object_scene::on_logic_view_closing	( System::Object^ , System::Windows::Forms::FormClosingEventArgs^ )
{
	for each( object_scene^ scene in m_sub_scenes_list )
	{
		scene->scene_node_position = scene->own_logic_node_in_parent_view->position;
		scene->own_logic_node_in_parent_view = nullptr;
	}
	m_scenes_view_height = (float)m_own_logic_view->cached_logic->logic_view->scenes_view->ActualHeight;
	m_own_logic_view->show_object_logic(nullptr);

	m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->link_creating		-= gcnew System::EventHandler<link_event_args^>(this, &object_scene::on_scene_link_creating);

	m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->selecting			-= gcnew System::EventHandler<selection_event_args^>( this, &object_scene::on_hypergraph_selecting );

	m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->context_menu_opening	-= gcnew System::Action(this, &object_scene::logic_hypergraph_context_menu_opening);
	m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->menu_item_click		-= gcnew System::EventHandler<menu_event_args^>(this, &object_scene::logic_hypergraph_menu_item_click);
	
	own_logic_node_in_own_view = nullptr;

	delete m_own_logic_view;
	m_own_logic_view = nullptr;	
}

void object_scene::on_scene_link_creating(System::Object^, link_event_args^ e)
{

	if (e->link_id->input_link_point_id != "input" )
		return;

	if (e->link_id->output_link_point_id == "input")
		return;

	m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->create_link(e->link_id);
	logic_scene_event_property_holder^ event_holder	= safe_cast<logic_scene_event_property_holder^>(e->source_tag);
	object_scene^ destination_scene		= (safe_cast<object_scene^>(e->destination_tag));
	event_holder->add_start_scene_action(gcnew logic_event_action_start_scene( event_holder, destination_scene ) );
}

void object_scene::logic_hypergraph_context_menu_opening	( )
{

}
void object_scene::logic_hypergraph_menu_item_click		( System::Object^, menu_event_args^ args )
{
	if ( args->menu_item == m_add_new_scene_item )
	{
		object_base^ o					= tool->create_library_object_instance( "scene", 0, 0 );
		project_item_object^ new_item	= gcnew project_item_object( get_project(), o );
		new_item->assign_id				( 0 );
		new_item->m_guid				= System::Guid::NewGuid();

		o->set_name( "scene", true );

		add_scene( new_item );
	}
	else if ( args->menu_item == m_delete_selected_item )
	{
		tool->get_level_editor()->get_command_engine()->run(gcnew command_delete_in_scene_logic_hypergraph( this ));
	}
}

void object_scene::on_hypergraph_selecting( System::Object^, selection_event_args^ e )
{
	m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->deselect_all_links();
	m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->deselect_all_nodes();

	for each( wpf_controls::hypergraph::link_id^ lnk_id in e->selected_link_ids )
	{
		m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->select_link( lnk_id );
	}
	for each( String^ node_id in e->selected_node_ids )
	{
		m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->select_node( node_id );
	}
}


void object_scene::remove_link				( link^ lnk )
{
	logic_scene_event_property_holder^ event_holder = safe_cast<logic_scene_event_property_holder^>( lnk->source_tag );
	event_holder->remove_start_scene_action( safe_cast<object_scene^>(lnk->destination_tag ) );
	m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->remove_link( lnk->id );
}

float object_scene::scenes_view_height::get()
{ 
	return ( own_logic_view != nullptr ) ? (float)own_logic_view->cached_logic->logic_view->scenes_view->ActualHeight : m_scenes_view_height; 
}


property_container^	object_scene::get_properties_container_for_parent_view	( )
{
	property_container^ result = gcnew property_container;

	property_descriptor^ finalizing_event_name_prop_descriptor = gcnew property_descriptor( this, "finalizing_event_name" ); 
	finalizing_event_name_prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));
	finalizing_event_name_prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::property_editors::attributes::combo_box_items_attribute
		( 
		"event",
		gcnew System::Func< System::String^ , Generic::IEnumerable<System::String^>^>( m_parent_scene, &object_scene::get_children_by_type ) 
		) ); 

	result->properties->add( finalizing_event_name_prop_descriptor );	
	
	property_container_collection^ event_handlers_container = gcnew property_container_collection( "event{0}" );

	property_descriptor^ event_handlers_descriptor = gcnew property_descriptor("Events", "","", event_handlers_container ); 
	event_handlers_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));

	for each ( logic_scene_event_property_holder^ holder in m_event_handlers )
	{
		if (holder->event_handler_type == "parent_view_handler")
			event_handlers_container->properties->add( holder->own_property_descriptor );		
	}

	event_handlers_descriptor->add_button		( "+", "", gcnew Action<button^>( this, &object_scene::add_event_handler_to_parent_view_clicked ) );
	event_handlers_descriptor->is_expanded = true;
	result->properties->add( event_handlers_descriptor );		
	return result;
}

void	object_scene::add_event_handler_to_parent_view_clicked	( button^ )
{

	logic_scene_event_property_holder^ property_value = gcnew logic_scene_event_property_holder( this );

	property_value->event_handler_type = "parent_view_handler";

	property_value->own_property_descriptor->dynamic_attributes->add
		(
		gcnew editor::wpf_controls::property_editors::attributes::combo_box_items_attribute
			( 
				"event",
				gcnew System::Func< System::String^ , Generic::IEnumerable<System::String^>^>( property_value, &logic_scene_event_property_holder::get_available_scenes_events ) 
			)
		);	

	m_event_handlers->Add( property_value );

	for each ( logic_scene_event_property_holder^ holder in m_event_handlers )
	{	
		if (holder->event_handler_type == "parent_view_handler" )
			holder->remove_logic_links();		
	}

	own_logic_node_in_parent_view->source_object = get_properties_container_for_parent_view();

	for each ( logic_scene_event_property_holder^ holder in m_event_handlers )
	{
		if (holder->event_handler_type == "parent_view_handler" )
			holder->add_logic_links();		
	}
}



void	object_scene::remove_event_handler_from_parent_view_clicked	( button^ btn )
{
	logic_scene_event_property_holder^ holder = safe_cast<logic_scene_event_property_holder^>( btn->tag );

	holder->on_before_remove();
	
	m_event_handlers->Remove( holder );	
	
	if (holder->event_handler_type == "own_view_handler")
		own_logic_node_in_own_view->source_object = get_properties_container_for_own_view();
	else if (holder->event_handler_type == "parent_view_handler")
		own_logic_node_in_parent_view->source_object = get_properties_container_for_parent_view();
}


void	object_scene::add_event_handler_to_own_view_clicked		( button^ )
{
	logic_scene_event_property_holder^ property_value = gcnew logic_scene_event_property_holder( this	);

	property_value->event_handler_type = "own_view_handler";

	m_event_handlers->Add( property_value );

	for each ( logic_scene_event_property_holder^ holder in m_event_handlers )
	{	
		if (holder->event_handler_type == "own_view_handler" )
			holder->remove_logic_links();		
	}

	own_logic_node_in_own_view->source_object = get_properties_container_for_own_view();

	for each ( logic_scene_event_property_holder^ holder in m_event_handlers )
	{
		if (holder->event_handler_type == "own_view_handler" )
			holder->add_logic_links();		
	}
}


void	object_scene::remove_event_handler_from_own_view_clicked		( button^ )
{

}

property_container^	object_scene::get_properties_container_for_own_view		( )
{
	property_container^ result = gcnew property_container;

	property_container_collection^ event_handlers_container = gcnew property_container_collection( "event{0}" );

	property_descriptor^ event_handlers_descriptor = gcnew property_descriptor("Events", "","", event_handlers_container ); 
	event_handlers_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));

	for each ( logic_scene_event_property_holder^ holder in m_event_handlers )
	{
		if (holder->event_handler_type == "own_view_handler" )
			event_handlers_container->properties->add( holder->own_property_descriptor );		
	}

	event_handlers_descriptor->add_button		( "+", "", gcnew Action<button^>( this, &object_scene::add_event_handler_to_own_view_clicked ) );
	event_handlers_descriptor->is_expanded = true;
	result->properties->add( event_handlers_descriptor );		
	return result;
}

}
}


