////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "volumetric_sound.h"
#include "tool_sound.h"
#include "project.h"
#include "project_items.h"
#include "level_editor.h"
#include <xray/render/facade/debug_renderer.h>
#include <xray/collision/collision_object.h>
#include <xray/editor/base/collision_object_types.h>
#include <xray/sound/sound_instance_proxy.h>
#include "editor_world.h"
#include <xray/sound/world.h>
#include "object_collision_geometry_mesh.h"
#include "object_collision_geometry.h"

namespace xray{
namespace editor{

using xray::editor::wpf_controls::property_descriptor;
using xray::editor::wpf_controls::property_editors::attributes::external_editor_event_handler;


volumetric_sound::volumetric_sound( tool_sound^ tool, render::scene_ptr const& scene )	:
	super							( tool ),
	m_tool_sound					( tool ),
	m_scene							( NEW(render::scene_ptr) (scene) ),
	m_sound_emitter_ptr				( NEW(sound::sound_emitter_ptr)() ),
	m_proxy							( NEW(sound::sound_instance_proxy_ptr)() ),
	m_volumetric_config_file_name	( gcnew System::String("") ),
	m_sound_file_name				( gcnew System::String("") ),
	m_radius						( 50.0f )
{
}

volumetric_sound::~volumetric_sound()
{
	if ( m_proxy->c_ptr() )
		*m_proxy					= 0;
	if ( m_sound_emitter_ptr->c_ptr() )
		*m_sound_emitter_ptr		= 0;
}


void volumetric_sound::on_selected(  bool selected )
{
	XRAY_UNREFERENCED_PARAMETER( selected );
}

void volumetric_sound::load_contents( )
{
	LOG_INFO("load_contents");
}

void volumetric_sound::load_sound( )
{
	if ( m_volumetric_config_file_name == "" )
		return;

	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &volumetric_sound::on_config_loaded), g_allocator);

	unmanaged_string	config_file_name("resources/sounds/volumetric/" + m_volumetric_config_file_name + ".volumetric_sound");

	resources::query_resource (
		config_file_name.c_str( ),
		resources::binary_config_class,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator 
		);
}

wpf_controls::property_container^ volumetric_sound::get_property_container( )
{

	property_container^ prop_container = super::get_property_container( );

	prop_container->properties["general/collision geometry"]->set_external_editor_style( 
		gcnew external_editor_event_handler( this, &volumetric_sound::select_collision_geometry ), false );

	prop_container->properties["sound/wav"]->dynamic_attributes->add( 
		gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( m_tool_sound->volumetric_sound_files( )  ) 
	);

	wpf_controls::property_descriptor^ desc = gcnew wpf_controls::property_descriptor("Playback");
	

	desc->add_button			( "Play looped", "SomeDescription", gcnew button_delegate(this, &volumetric_sound::play_looped_clicked) );

	prop_container->properties->add	( desc );


	return prop_container;
}

String^ volumetric_sound::collision_geometry::get( )
{
	return ( m_collision_geometry == nullptr ) ? "" : m_collision_geometry->get_full_name( ); 
}

void volumetric_sound::collision_geometry::set( String^ full_name )
{
	m_collision_geometry = owner_tool( )->get_level_editor( )->get_project( )->find_by_path( full_name, true );
}

void volumetric_sound::select_collision_geometry	( wpf_controls::property_editors::property^ prop , Object^ )
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
		prop->value			= selected;
	}
}

void volumetric_sound::play_looped_clicked	( button^ )
{
	object_collision_geometry^ geom					= (object_collision_geometry^)( m_collision_geometry->get_object( ));
	object_collision_geometry_mesh^	mesh			= geom->get_mesh( false, 0 );
	collision::geometry_instance* instance			= mesh->get_geometry_instance( );
	if ( (*m_sound_emitter_ptr).c_ptr() )
	{
		*m_proxy = ((*m_sound_emitter_ptr)->emit_volumetric_sound
		(
			get_editor_world().sound_scene(),
			get_editor_world().engine().get_sound_world().get_editor_world_user(),
			*instance,
			m_radius
		));
		(*m_proxy)->play( sound::looped );
	}
}

void volumetric_sound::on_sound_loaded( resources::queries_result& data )
{
	R_ASSERT					(data.is_successful());
	*m_sound_emitter_ptr		= static_cast_resource_ptr<sound::sound_emitter_ptr>(data[0].get_unmanaged_resource());
}

void volumetric_sound::on_config_loaded( resources::queries_result& data )
{
	R_ASSERT									(data.is_successful());
	configs::binary_config_ptr const cfg		= static_cast_resource_ptr<configs::binary_config_ptr>(data[0].get_unmanaged_resource());

	R_ASSERT									(cfg.c_ptr());
	configs::binary_config_value const& root	= cfg->get_root();
	configs::binary_config_value const& vol_snd	= root		["volumetric_sound"];
	radius										= vol_snd	["radius"];
	configs::binary_config_value const& snd		= vol_snd	["sound"];
	fs::path_string snd_filename				= snd		["filename"];
	u32 type									= snd		["resource_type"];

	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &volumetric_sound::on_sound_loaded), g_allocator);
	resources::query_resource(
		snd_filename.c_str(),
		(resources::class_id_enum)type,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator
	);
}

void volumetric_sound::update_radius		( )
{

}

void volumetric_sound::load_props ( configs::lua_config_value const& config_value )
{
	super::load_props( config_value );

	String^ selected_collision_geometry = gcnew String( config_value["collision_geometry"] );
	if ( selected_collision_geometry != "")
		owner_tool( )->get_level_editor( )->get_project( )->query_project_item( selected_collision_geometry, gcnew xray::editor::queried_object_loaded( this, &volumetric_sound::collision_geometry_loaded ) );
	String^ config_file_name = gcnew String( config_value["sound"] );

	if ( config_file_name != "" )
		wav_filename			= config_file_name;
}

void volumetric_sound::save			( configs::lua_config_value config_value )
{
	super::save		( config_value );

	config_value["game_object_type"]			= "volumetric_sound";
	config_value["collision_geometry"]			= unmanaged_string( collision_geometry ).c_str( );
	config_value["sound"]						= unmanaged_string( m_volumetric_config_file_name ).c_str( );
	config_value["radius"]						= radius;
}

void volumetric_sound::collision_geometry_loaded	( project_item_base^ geom )
{
	m_collision_geometry				= geom;
	safe_cast<project_item_object^>( geom )->m_object_base->before_manual_delete += gcnew Action( this, &volumetric_sound::collision_deleting );
}

void volumetric_sound::collision_deleting			( )
{
	m_collision_geometry = nullptr;
}

} // namespace editor
} // namespace xray
