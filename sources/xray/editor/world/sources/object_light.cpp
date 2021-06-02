////////////////////////////////////////////////////////////////////////////
//	Created		: 17.03.2009
//	Author		: 
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_light.h"
#include "tool_light.h"
#include <xray/editor/base/collision_object_types.h>

#pragma managed( push, off )
#include <xray/collision/collision_object.h>
#pragma managed( pop )

namespace xray{
namespace editor{

object_light::object_light( tool_light^ t, render::scene_ptr const& scene ) :
	super					( t ),
	m_tool_light			( t ),
	m_scene					( NEW(render::scene_ptr) (scene) ),
	m_spot_penumbra_angle	( math::pi_d4 ),
	m_spot_umbra_angle		( math::pi_d8 ),
	m_spot_falloff			( 2.f ),
	m_type					( render::light_type_point ),
	m_attenuation_power		( 2 ),
	m_intensity				( 1.f ),
	m_diffuse_influence_factor( 1.f ),
	m_specular_influence_factor( 1.f ),
	m_sun_shadow_map_size	( 2048 ),
	m_shadow_map_size_index	( 0 ),
	m_num_sun_cascades		( 4 ),
	m_z_bias				( 0.0001f ),
	m_lighting_model		( 1 ),
	m_is_shadower			( false ),
	m_use_with_lpv			( false )
{
	image_index				= xray::editor_base::object_light;
	m_is_cast_shadow		= false;
	m_is_cast_shadow_x		= true;
	m_is_cast_shadow_neg_x	= true;
	m_is_cast_shadow_y		= true;
	m_is_cast_shadow_neg_y	= true;
	m_is_cast_shadow_z		= true;
	m_is_cast_shadow_neg_z	= true;

	m_year					= 2011;
	m_month					= 10;
	m_day					= 31;
	m_hours					= 12;
	m_minutes				= 20;
	m_seconds				= 0;
	m_use_auto_lacation		= false;
	m_time_of_day			= 50.0f;
}

object_light::~object_light( )
{
	destroy_collision				( );

	DELETE					( m_scene );
}

void object_light::destroy_collision	( )
{
	if ( m_collision->initialized() )
		m_collision->destroy( g_allocator );
}

void object_light::load_defaults( )
{
	super::load_defaults( );
	m_range				= 1.0f;
	m_color.set_rgba	(1.0f, 1.0f, 1.0f, 1.0f );
}

void object_light::load_props( configs::lua_config_value const& t )
{
	super::load_props	(t);

	m_type			= (render::light_type)(int)t["light_type"];
	
	if(m_type==render::light_type_parallel)
		image_index				= xray::editor_base::sun_light;
	else
		image_index				= xray::editor_base::object_light;

	float3 c		= t["color"];
	m_color.r		= c.x;
	m_color.g		= c.y;
	m_color.b		= c.z;

	m_range			= t["range"];

	if ( t.value_exists("attenuation_power") )
		m_attenuation_power	= t["attenuation_power"];
	else {
		if ( t.value_exists("attenuation") )
			m_attenuation_power	= float3(t["attenuation"]).x;
	}

	if ( t.value_exists("lighting_model") )
		m_lighting_model	= t["lighting_model"];

	if ( t.value_exists("intensity") )
		m_intensity	= t["intensity"];

	if ( t.value_exists("spot_umbra_angle") )
		m_spot_umbra_angle	= t["spot_umbra_angle"];

	if ( t.value_exists("spot_penumbra_angle") )
		m_spot_penumbra_angle	= t["spot_penumbra_angle"];

	if ( t.value_exists("spot_falloff") )
		m_spot_falloff	= t["spot_falloff"];

	if ( t.value_exists("diffuse_influence_factor") )
		m_diffuse_influence_factor	= t["diffuse_influence_factor"];

	if ( t.value_exists("use_with_lpv") )
		m_use_with_lpv	= bool(t["use_with_lpv"]);

	
	if ( t.value_exists("is_shadower") )
		m_is_shadower	= t["is_shadower"];
	
	if ( t.value_exists("sun_shadow_map_size") )	m_sun_shadow_map_size	= t["sun_shadow_map_size"];
	if ( t.value_exists("shadow_map_size_index") )	m_shadow_map_size_index	= t["shadow_map_size_index"];
	if ( t.value_exists("num_sun_cascades") )		m_num_sun_cascades		= t["num_sun_cascades"];
	
	if ( t.value_exists("is_cast_shadow") )			m_is_cast_shadow		= t["is_cast_shadow"];
	if ( t.value_exists("is_cast_shadow_x") )		m_is_cast_shadow_x		= t["is_cast_shadow_x"];
	if ( t.value_exists("is_cast_shadow_neg_x") )	m_is_cast_shadow_neg_x	= t["is_cast_shadow_neg_x"];
	if ( t.value_exists("is_cast_shadow_y") )		m_is_cast_shadow_y		= t["is_cast_shadow_y"];
	if ( t.value_exists("is_cast_shadow_neg_y") )	m_is_cast_shadow_neg_y	= t["is_cast_shadow_neg_y"];
	if ( t.value_exists("is_cast_shadow_z") )		m_is_cast_shadow_z		= t["is_cast_shadow_z"];
	if ( t.value_exists("is_cast_shadow_neg_z") )	m_is_cast_shadow_neg_z	= t["is_cast_shadow_neg_z"];
	if ( t.value_exists("z_bias") )					m_z_bias				= t["z_bias"];
	if ( t.value_exists("shadow_transparency") )	m_shadow_transparency	= t["shadow_transparency"];
}


void object_light::save( configs::lua_config_value t )
{
	super::save						( t );
	
	float3 c						(m_color.r, m_color.g, m_color.b );
	
	t["game_object_type"]			= "light";
	t["light_type"]					= (int)m_type;
	t["color"]						= c;
	t["range"]						= m_range;
	t["attenuation_power"]			= m_attenuation_power;
	t["lighting_model"]				= m_lighting_model;
	t["intensity"]					= m_intensity;
	t["spot_umbra_angle"]			= m_spot_umbra_angle;
	t["spot_penumbra_angle"]		= m_spot_penumbra_angle;
	t["spot_falloff"]				= m_spot_falloff;
	t["sun_shadow_map_size"]		= m_sun_shadow_map_size;
	t["shadow_map_size_index"]		= m_shadow_map_size_index;
	t["num_sun_cascades"]			= m_num_sun_cascades;
	t["is_cast_shadow"]				= m_is_cast_shadow;
	t["is_cast_shadow_x"]			= m_is_cast_shadow_x;
	t["is_cast_shadow_neg_x"]		= m_is_cast_shadow_neg_x;
	t["is_cast_shadow_y"]			= m_is_cast_shadow_y;
	t["is_cast_shadow_neg_y"]		= m_is_cast_shadow_neg_y;
	t["is_cast_shadow_z"]			= m_is_cast_shadow_z;
	t["is_cast_shadow_neg_z"]		= m_is_cast_shadow_neg_z;
	t["z_bias"]						= m_z_bias;
	t["shadow_transparency"]		= m_shadow_transparency;
	t["diffuse_influence_factor"]	= m_diffuse_influence_factor;
	t["use_with_lpv"]				= m_use_with_lpv;
	t["specular_influence_factor"]	= m_specular_influence_factor;
	t["is_shadower"]				= m_is_shadower;
}

//!
//void object_light::update_collision_transform( )
//{
//	float4x4 m = create_translation(get_position());
//	m_collision->set_matrix		( &m );
//}

void object_light::initialize_collision( )
{
	ASSERT( !m_collision->initialized() );

	m_collision->create_from_geometry	(
					false,
					this,
					&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), m_tool_light->m_object_geometry ),
					editor_base::collision_object_type_dynamic | editor_base::collision_object_type_touch
				);
	m_collision->insert			( m_transform );
}

void object_light::unload_contents	( bool bdestroy )
{
	XRAY_UNREFERENCED_PARAMETERS	( bdestroy );

	destroy_collision				( );
}

} // namespace editor
} // namespace xray
