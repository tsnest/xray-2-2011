////////////////////////////////////////////////////////////////////////////
//	Created		: 12.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_light.h"
#include "game_world.h"
#include "game.h"
#include <xray/render/world.h>
#include <xray/render/facade/scene_renderer.h>

namespace stalker2{

static u32 light_ids = 1000000;

object_light::object_light( game_world& w ) 
:super			( w ),
m_current_state	( false )
{
	m_light_id	= ++light_ids;
	m_scene = m_game_world.get_game().get_render_scene();
	R_ASSERT(m_scene);
}

object_light::~object_light( )
{
}

void object_light::load( configs::binary_config_value const& t )
{
	super::load										(t);
	math::float3 clr								= t["color"];
	
	if ( t.value_exists("is_cast_shadow") )			m_props.does_cast_shadows				= t["is_cast_shadow"];
	if ( t.value_exists("sun_shadow_map_size") )	m_props.sun_shadow_map_size				= t["sun_shadow_map_size"];
	if ( t.value_exists("shadow_map_size_index") )	m_props.shadow_map_size_index			= t["shadow_map_size_index"];
	if ( t.value_exists("num_sun_cascades") )		m_props.num_sun_cascades				= t["num_sun_cascades"];
	if ( t.value_exists("is_cast_shadow_x") )		m_props.shadow_distribution_sides[0]	= (bool)t["is_cast_shadow_x"];
	if ( t.value_exists("is_cast_shadow_neg_x") )	m_props.shadow_distribution_sides[1]	= (bool)t["is_cast_shadow_neg_x"];
	if ( t.value_exists("is_cast_shadow_y") )		m_props.shadow_distribution_sides[2]	= (bool)t["is_cast_shadow_y"];
	if ( t.value_exists("is_cast_shadow_neg_y") )	m_props.shadow_distribution_sides[3]	= (bool)t["is_cast_shadow_neg_y"];
	if ( t.value_exists("is_cast_shadow_z") )		m_props.shadow_distribution_sides[4]	= (bool)t["is_cast_shadow_z"];
	if ( t.value_exists("is_cast_shadow_neg_z") )	m_props.shadow_distribution_sides[5]	= (bool)t["is_cast_shadow_neg_z"];
	if ( t.value_exists("z_bias") )					m_props.local_light_z_bias				= (float)t["z_bias"];
	if ( t.value_exists("shadow_transparency") )	m_props.shadow_transparency				= (float)t["shadow_transparency"];

	if ( t.value_exists("lighting_model") )
		m_props.lighting_model				= t["lighting_model"];
	else
		m_props.lighting_model				= 1;
	
	m_props.range							= (float)t["range"];
	m_props.color							= math::color_rgba(clr.x, clr.y, clr.z, 1.0f);
	m_props.transform						= m_transform;
	m_props.type							= (render::light_type)(int)t["light_type"];

	if ( t.value_exists("attenuation_power") )
		m_props.attenuation_power			= t["attenuation_power"];
	else
		m_props.attenuation_power			= float3(t["attenuation"]).x;

	m_props.intensity						= t["intensity"];
	m_props.spot_umbra_angle				= t["spot_umbra_angle"];
	m_props.spot_penumbra_angle				= t["spot_penumbra_angle"];
	m_props.spot_falloff					= t["spot_falloff"];
	
	m_props.diffuse_influence_factor		= t.value_exists("diffuse_influence_factor")  ? float(t["diffuse_influence_factor"])  : 1.0f;
	m_props.specular_influence_factor		= t.value_exists("specular_influence_factor") ? float(t["specular_influence_factor"]) : 1.0f;

	m_props.shadower						= t.value_exists("is_shadower") ? bool(t["is_shadower"]) : false;
	m_props.use_with_lpv					= t.value_exists("use_with_lpv") ? bool(t["use_with_lpv"]) : false;
	
}

void object_light::unload_contents( )
{
	if(!get_current_behaviour())
		remove_from_scene();
}

void object_light::load_contents( )
{
	if(!get_current_behaviour())
		add_to_scene();
}

void object_light::update_props( )
{
	if(m_current_state)
	{
		m_game_world.get_game().renderer().scene().update_light	( m_scene, m_light_id, m_props );
	}
}

void object_light::add_to_scene( )
{
	if(m_current_state)
		return;

	m_game_world.get_game().renderer().scene().add_light	( m_scene, m_light_id, m_props );
	m_current_state = true;
}

void object_light::remove_from_scene( )
{
	if(!m_current_state)
		return;

	m_game_world.get_game().renderer().scene().remove_light	( m_scene, m_light_id );
	m_current_state = false;
}

} // namespace stalker2