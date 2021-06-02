////////////////////////////////////////////////////////////////////////////
//	Created		: 13.10.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_volumetric_sound.h"
#include "object_collision_geometry.h"
#include "game_world.h"
#include "game.h"
#include <xray/sound/world.h>

namespace stalker2 {

object_volumetric_sound::object_volumetric_sound		( game_world& w ) :
	super				( w ),
	m_collision_geometry( 0 ),
	m_world_user		( w.get_game().sound_world().get_logic_world_user() ),
	m_sound_scene		( w.get_game().get_sound_scene() ),
	m_radius			( 0.0f )
{}

object_volumetric_sound::~object_volumetric_sound		( )
{
	unload_contents			( );
}

void object_volumetric_sound::load						( configs::binary_config_value const& config_value )
{
	super::load							( config_value );

	pcstr	collision_geometry_name		= config_value["collision_geometry"];

	m_game_world.query_object_by_name(
		collision_geometry_name,
		object_loaded_callback_type( this, &object_volumetric_sound::on_collision_object_loaded )
	);
	m_sound_name						= config_value["sound"];
}

void object_volumetric_sound::load_contents				( )
{
	fs::path_string						config_path;
	config_path.assignf					( "%s%s%s", "resources/sounds/volumetric/", m_sound_name.c_str(), ".volumetric_sound" );
	resources::query_resource			(  
		config_path.c_str( ),
		resources::binary_config_class,
		boost::bind(&object_volumetric_sound::on_config_loaded, this, _1),
		g_allocator
	);

}

void object_volumetric_sound::unload_contents			( )
{
	m_emitter											= 0;
	m_proxy												= 0;
	if ( m_collision_geometry )
	{
		m_collision_geometry->destroy						( g_allocator );
		XRAY_DELETE_IMPL									( g_allocator, m_collision_geometry );
	}
}

void object_volumetric_sound::on_collision_object_loaded( game_object_ptr_ const& object )
{
	object_collision_geometry* obj	= static_cast_checked<object_collision_geometry*>( object.c_ptr( ) );
	R_ASSERT													( obj );
	object_collision_geometry::instances_container const& cont	= obj->get_instances( );
	xray::collision::geometry* geom	= &*xray::collision::new_composite_geometry		( g_allocator, cont );
	m_collision_geometry			= &*xray::collision::new_composite_geometry_instance( g_allocator, float4x4().identity(), geom );
}

void object_volumetric_sound::on_config_loaded			( resources::queries_result& data )
{
	configs::binary_config_ptr const config				= static_cast_resource_ptr<configs::binary_config_ptr>( data[0].get_unmanaged_resource( ) );
	R_ASSERT											( config.c_ptr( ) );
	configs::binary_config_value const& root			= config->get_root( );
	configs::binary_config_value const& vol_snd			= root		["volumetric_sound"];
	m_radius											= vol_snd	["radius"];
	configs::binary_config_value const& snd				= vol_snd	["sound"];
	fs::path_string snd_filename						= snd		["filename"];
	u32 type											= snd		["resource_type"];

	resources::query_resource (
		snd_filename.c_str( ),
		(resources::class_id_enum)type,
		boost::bind( &object_volumetric_sound::on_sound_loaded, this, _1 ),
		g_allocator
	);
}

void object_volumetric_sound::on_sound_loaded			( resources::queries_result& data )
{
	m_emitter							= static_cast_resource_ptr<xray::sound::sound_emitter_ptr>( data[0].get_unmanaged_resource( ) );
	R_ASSERT							( m_emitter.c_ptr( ) );
	m_proxy								= m_emitter->emit( m_sound_scene, m_world_user );
	//m_proxy->set_collision_geometry		( *m_collision_geometry, m_radius );
	m_proxy->play						( xray::sound::looped );
}

} // namespace stalker2