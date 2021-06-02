////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "lights_db.h"
#include "light.h"
#include <xray/collision/api.h>
#include <xray/collision/space_partitioning_tree.h>
#include "sun_position.h"
#include "moon_position.h"
#include <xray/console_command.h>

namespace xray {
namespace render {

lights_db::lights_db			( ) :
	m_lights_tree				( 0 )
{
	//initialize_sun				( );

	m_lights_tree				= &*collision::new_space_partitioning_tree( g_allocator, 1.f, 1024 );
}

lights_db::~lights_db							( )
{
	collision::delete_space_partitioning_tree	( m_lights_tree );
}

light* lights_db::create		( tree_operation_enum const operation ) const
{
	return						NEW ( light ) ( operation == tree_operation_add ? m_lights_tree : 0 );
}

void lights_db::initialize_sun	( light_data& light_to_add)
{
	//R_ASSERT					( !m_sun );
	m_sun						= light_to_add.light;//create( tree_operation_no_operation );
	
	m_sun->flags.is_static		= true;
	//m_sun->set_type				( render::light_type_parallel );
	//m_sun->set_cast_shadows		( true );
	//m_sun->set_color			( math::color(0.89578169f, 0.88f, 0.87f, 1.0f), 1.f );
	
	//m_sun->set_orientation		( (float3(0.0f, 0.0f, 0.0f) - m_sun->position).normalize(), float3(1.f,0.f,0.f) );
	//m_sun->set_position			( float3(0.f, 500.f, 0.f) );
}

static u32		s_v0_value = 2011;
static u32		s_v1_value = 10;
static u32		s_v2_value = 31;
static u32		s_v3_value = 12;
static u32		s_v4_value = 0;
static u32		s_v5_value = 0;
static float	s_v6_value = 50.27f;
static float	s_v7_value = 30.30f;

static math::float3 get_sun_direction(light_props const& props)
{
	static xray::console_commands::cc_u32 s_v0("year",			s_v0_value, 0, 3000, false, xray::console_commands::command_type_engine_internal);
	static xray::console_commands::cc_u32 s_v1("month",			s_v1_value, 1, 12, false, xray::console_commands::command_type_engine_internal);
	static xray::console_commands::cc_u32 s_v2("day",			s_v2_value, 1, 31, false, xray::console_commands::command_type_engine_internal);
	static xray::console_commands::cc_u32 s_v3("hours",			s_v3_value, 0, 24, false, xray::console_commands::command_type_engine_internal);
	static xray::console_commands::cc_u32 s_v4("minutes",		s_v4_value, 0.0f, 59.0f, false, xray::console_commands::command_type_engine_internal);
	static xray::console_commands::cc_u32 s_v5("seconds",		s_v5_value, 0.0f, 59.0f, false, xray::console_commands::command_type_engine_internal);
	static xray::console_commands::cc_float s_v6("latitude",	s_v6_value, -90.0f, 90.0f, false, xray::console_commands::command_type_engine_internal);
	static xray::console_commands::cc_float s_v7("longitude",	s_v7_value, -90.0f, 90.0f, false, xray::console_commands::command_type_engine_internal);
	
	cTime c_time;
	c_time.iYear		= props.m_year;//s_v0_value;//2011;
	c_time.iMonth		= props.m_month;//s_v1_value;//10;
	c_time.iDay			= props.m_day;//s_v2_value;//31;
	c_time.dHours		= props.m_hours - 2;//s_v3_value - 2;//12.0;
	c_time.dMinutes		= props.m_minutes;//s_v4_value;//0.00;
	c_time.dSeconds		= props.m_seconds;//s_v5_value;//0.00;
	
	/*
		Киев
		Координаты:
		Широта: 50° 27' с.ш.
		Долгота: 30° 30' в.д.
	*/
	
	cLocation c_location;
	c_location.dLatitude	= s_v6_value;//50.27;
	c_location.dLongitude	= s_v7_value;//30.30;
	
	cSunCoordinates out_sun_coordinates;
	
	calculate_sun_position(c_time, c_location, &out_sun_coordinates);
	
	time_parameters moon_time_parameters;
	
	moon_time_parameters.year		= props.m_year;
	moon_time_parameters.month		= props.m_month;
	moon_time_parameters.day		= props.m_day;
	moon_time_parameters.hours		= props.m_hours - 2;
	moon_time_parameters.minutes	= props.m_minutes;
	moon_time_parameters.seconds	= props.m_seconds;
	
	double azimuth = 0.0, zenith = 0.0;
	
	calculate_moon_position(moon_time_parameters, s_v6_value, s_v7_value, azimuth, zenith);
	
	// x = r sinZ cosA
	// y = r sinZ sinA
	// z = r cosZ
	// Z: zenith
	// A: azimuth
	// r: 1
	
	math::float3 dir;
	dir.x = math::sin(math::deg2rad(out_sun_coordinates.dZenithAngle)) * math::cos(math::deg2rad(out_sun_coordinates.dAzimuth));
	dir.y = math::cos(math::deg2rad(out_sun_coordinates.dZenithAngle));
	dir.z = -math::sin(math::deg2rad(out_sun_coordinates.dZenithAngle)) * math::sin(math::deg2rad(out_sun_coordinates.dAzimuth));
	
	//dir.x = math::sin(math::deg2rad(zenith)) * math::cos(math::deg2rad(azimuth));
	//dir.y = math::cos(math::deg2rad(zenith));
	//dir.z = -math::sin(math::deg2rad(zenith)) * math::sin(math::deg2rad(azimuth));
	
	return -math::normalize(dir);
}

static void fill_light			( light& light, light_props const& props )
{
	float4x4 transform			= props.transform;
	switch ( props.type ) {
		case light_type_spot : {
			// since spot light looks down(along y axis, not along z axis),
			// we need to rotate it -90 degrees around axis X
			float3 const translation = transform.c.xyz();
			transform.c.xyz()			= float3( 0.f, 0.f, 0.f );
			transform					= math::create_rotation_x( -math::pi_d2 ) * transform;
			transform.c.xyz()			= translation;

			light.spot_penumbra_angle	= props.spot_penumbra_angle;
			light.spot_umbra_angle		= props.spot_umbra_angle;
			light.spot_falloff			= props.spot_falloff;
			break;
		}
		case light_type_capsule : {
			// since capsule looks down(along y axis, not along z axis),
			// we need to rotate it -90 degrees around axis X
			float3 const translation	= transform.c.xyz();
			transform.c.xyz()			= float3( 0.f, 0.f, 0.f );
			transform					= math::create_rotation_x( -math::pi_d2 ) * transform;
			transform.c.xyz()			= translation;

			float3 const& scale = props.transform.get_scale();
			if ( scale.x != scale.z )
				LOG_WARNING				( "invalid capsule passed, x=%f, z=%f", scale.x, scale.z );

			break;
		}
		case light_type_plane_spot : {
			light.spot_penumbra_angle	= props.spot_penumbra_angle;
			light.spot_umbra_angle		= props.spot_umbra_angle;
			light.spot_falloff			= props.spot_falloff;
			break;
		}
	}
	
	light.set_type				( props.type );
	light.set_cast_shadows		( props.does_cast_shadows );
	light.set_position			( transform.c.xyz() );
	light.set_orientation		( transform.k.xyz(), transform.i.xyz() );
	light.set_range				( props.range );
	light.set_color				( math::color(props.color), props.intensity );
	light.set_attenuation_power	( props.attenuation_power );
	light.lighting_model		= props.lighting_model;
	light.set_scale				( transform.get_scale() );
	light.diffuse_influence_factor	= props.diffuse_influence_factor;
	light.use_with_lpv				= props.use_with_lpv;
	
	light.specular_influence_factor	= props.specular_influence_factor;
	light.is_shadower				= props.shadower;
	light.shadow_z_bias				= props.local_light_z_bias;
	light.shadow_map_size			= props.shadow_map_size;
	light.shadow_map_size_index		= props.shadow_map_size_index;
	light.shadow_transparency		= props.shadow_transparency;
	
	xray::memory::copy(
		&light.shadow_distribution_sides[0], 
		sizeof(light.shadow_distribution_sides), 
		&props.shadow_distribution_sides[0], 
		sizeof(light.shadow_distribution_sides)
	);
	
	switch ( props.type ) {
		case light_type_parallel : {
			light.set_position			(math::float3(0.0f,500.0f,0.0f));
			light.sun_shadow_map_size	= props.sun_shadow_map_size;
			light.num_sun_cascades		= props.num_sun_cascades;
			
			//if (props.m_use_auto_lacation)
			//	light.direction				= get_sun_direction(props);
			
			break;
		}
	}
	light.on_properties_changed	( );
}

void lights_db::add_light		( u32 const id, render::light_props const& props )
{
	light_data light_to_add	( id );
	
	lights_type::iterator it	= std::lower_bound( m_lights.begin(), m_lights.end(), light_to_add);
	ASSERT						( (it == m_lights.end()) || (it->id != id) );
		
	light_to_add.light			= create( tree_operation_add);
	fill_light					( *light_to_add.light, props );
	m_lights.insert				( it, light_to_add );
	
	if (light_to_add.light->get_type() == light_type_parallel)
	{
		initialize_sun(light_to_add);
		return;
	}
}

void lights_db::update_light	( u32 id, render::light_props const& props )
{
	lights_type::iterator found	= std::lower_bound( m_lights.begin(), m_lights.end(), id );
	ASSERT						( found != m_lights.end() && (*found).id == id );
	fill_light					( *(*found).light, props );
}

void lights_db::remove_light	( u32 const id )
{
	lights_type::iterator found = std::find( m_lights.begin(), m_lights.end(), id );
	R_ASSERT					( found != m_lights.end() );
	
	if (found->light->get_type()==light_type_parallel)
		m_sun = 0;
	
	m_lights.erase				( found );
	
}

} // namespace render 
} // namespace xray