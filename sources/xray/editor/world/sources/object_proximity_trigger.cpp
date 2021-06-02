////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_proximity_trigger.h"
#include "tool_misc.h"
#include "project_items.h"
#include "tool_scripting.h"
#include "level_editor.h"
#include "project.h"
#include <xray/editor/base/collision_object_types.h>

#pragma managed( push, off )
#	include <xray/collision/collision_object.h>
#pragma managed( pop )


using xray::editor::wpf_controls::property_descriptor;
using xray::editor::wpf_controls::property_editors::attributes::external_editor_event_handler;

namespace xray{
namespace editor{

object_proximity_trigger::object_proximity_trigger ( tool_misc^ tool ): super( tool )
{
	max_tracked_objects_count	= 20;
	image_index					= xray::editor_base::misc_tree_icon;
}
object_proximity_trigger::~object_proximity_trigger ( )
{	

}

void object_proximity_trigger::load_contents ( )
{
	ASSERT( !m_collision->initialized( ) );

	collision::geometry_instance* geom		= &*collision::new_sphere_geometry_instance( &debug::g_mt_allocator, float4x4().identity(), 0.5f );
	m_collision->create_from_geometry		( true, this, geom, xray::editor_base::collision_object_type_dynamic | xray::editor_base::collision_object_type_touch );
	m_collision->insert( &get_transform( ) );
}
void object_proximity_trigger::unload_contents ( Boolean is_destroy )
{
	XRAY_UNREFERENCED_PARAMETER( is_destroy );

	if( m_collision->initialized( ) )
		m_collision->destroy( &debug::g_mt_allocator );
}

void object_proximity_trigger::load_props ( configs::lua_config_value const& config_value )
{
	super::load_props( config_value );

	max_tracked_objects_count = config_value["max_tracked_objects_count"];

	String^ selected_collision_geometry = gcnew String( config_value["collision_geometry"] );
	if ( selected_collision_geometry != "")
		owner_tool( )->get_level_editor( )->get_project( )->query_project_item( selected_collision_geometry, gcnew xray::editor::queried_object_loaded( this, &object_proximity_trigger::collision_geometry_loaded ) );
}

void object_proximity_trigger::save	( configs::lua_config_value config_value )
{
	super::save		( config_value );

	config_value["max_tracked_objects_count"]	= max_tracked_objects_count;
	config_value["game_object_type"]			= "proximity_trigger";
	config_value["collision_geometry"]			= unmanaged_string( collision_geometry ).c_str( );
}

property_container^ object_proximity_trigger::get_property_container	( )
{
	property_container^ prop_container = super::get_property_container( );

	prop_container->properties["general/collision geometry"]->set_external_editor_style( 
		gcnew external_editor_event_handler( this, &object_proximity_trigger::select_collision_geometry ), false );

	return prop_container;
}

////////////////		P U B L I C   P R O P E R T I E S		////////////////

String^ object_proximity_trigger::collision_geometry::get( )
{
	return ( m_collision_geometry == nullptr ) ? "" : m_collision_geometry->get_full_name( ); 
}

void object_proximity_trigger::collision_geometry::set( String^ full_name )
{
	if( full_name == "" )
		m_collision_geometry = nullptr;
	else
		m_collision_geometry = owner_tool( )->get_level_editor( )->get_project( )->find_by_path( full_name, true );
}

////////////////		P R I V A T E   M E T H O D S			////////////////

void object_proximity_trigger::select_collision_geometry	( wpf_controls::property_editors::property^ prop , Object^ )
{
	String^ selected;
	bool result = editor_base::resource_chooser::choose(
		"project_item",
		collision_geometry,
		"xray.editor.object_collision_geometry",
		selected,
		true
	);
	if ( result )
	{
		project_item_object^ project_item = safe_cast<project_item_object^>( owner_tool( )->get_level_editor( )->get_project( )->find_by_path( selected, true ) );
		project_item->m_object_base->before_manual_delete += gcnew Action( this, &object_proximity_trigger::collision_deleting );
		prop->value						= selected;
	}
}

void object_proximity_trigger::collision_deleting			( )
{
	m_collision_geometry = nullptr;
}

void object_proximity_trigger::collision_geometry_loaded	( project_item_base^ collision_geometry )
{
	m_collision_geometry				= collision_geometry;
	safe_cast<project_item_object^>( collision_geometry )->m_object_base->before_manual_delete += gcnew Action( this, &object_proximity_trigger::collision_deleting );
}

} // namespace editor
} // namespace xray
