////////////////////////////////////////////////////////////////////////////
//	Created		: 25.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "physics_world.h"
#include "shell.h"
#include "element.h"
#include "body_ode.h"
#include "joint_fixed.h"
#include "joint_ball.h"
#include "joint_euler_hinge.h"
#include "joint_wheel.h"
#include "joint_slider_hinge.h"

#include <xray/configs.h>
#include <xray/collision/api.h>
#include <xray/collision/shell_creation_utils.h>
#include <xray/collision/geometry_instance.h>
#include <xray/collision/bone_collision_data.h>

#include "generic_joint_create_lua_config.h"

namespace xray {
namespace physics {

template < class ConfigValueType >
non_null< body> ::ptr new_body	( ConfigValueType const& table, float4x4 const& transform )
{
	body* new_ode_body			= XRAY_NEW_IMPL( xray::physics::g_ph_allocator, body_ode );
	R_ASSERT					( new_ode_body );
	
	//float		mass			= table["mass"];
	float3		mass_center		= table["mass_center"];
	//mass_center.z = -mass_center.z;
	
	float4x4 pose				=  math::create_translation( mass_center ) * transform;
	new_ode_body->set_pose		( pose );
	return						new_ode_body;
}

template < class ConfigValueType >
non_null< element >::ptr new_element( ConfigValueType const& table )
{
	float4x4						transform; 
	float3							scale;
	collision::read_transform		( table, transform, scale );

	non_null< body >::ptr body				= new_body( table, transform );
	non_null< collision::geometry_instance >::ptr instance	= collision::new_element_collision( table["collision"], transform, xray::physics::g_ph_allocator );
	non_null< element >::ptr element_ptr	= XRAY_NEW_IMPL( xray::physics::g_ph_allocator, element )( *body, *instance );
	element_ptr->set_mass					( table["mass"] );
	
	return							element_ptr;
}

template < class ConfigValueType >
static u32 find_index		( ConfigValueType const& section, pcstr element )
{
	u32 const section_size	= section.size();
	for ( u32 i = 0; i < section_size; ++i )
		if ( strings::equal( pcstr( section[i]["name"] ), element ) )
			return i;
		
	ASSERT					( false );
	return					u32(-1);
}

template < class ConfigValueType >
joint* new_fixed_joint	( ConfigValueType const& config, body* b0, body* b1, float4x4 const& transform )
{
	XRAY_UNREFERENCED_PARAMETERS	( &config, &transform );
	joint* result					= XRAY_NEW_IMPL(xray::physics::g_ph_allocator, joint_fixed );
	result->connect					( b0, b1 );
	return							result;

}

template < class ConfigValueType >
joint* new_ball_joint	( ConfigValueType const& config, body* b0, body* b1, float4x4 const& transform )
{
	XRAY_UNREFERENCED_PARAMETERS	( &config, &transform );
	joint* result					= XRAY_NEW_IMPL(xray::physics::g_ph_allocator, joint_ball );
	result->connect					( b0, b1 );
	return							result;
}

template < class ConfigValueType >
joint* new_wheel_joint	( ConfigValueType const& config, body* b0, body* b1, float4x4 const& transform )
{
	XRAY_UNREFERENCED_PARAMETERS	( &config, &transform );
	joint* result					= XRAY_NEW_IMPL(xray::physics::g_ph_allocator, joint_wheel );
	result->connect					( b0, b1 );
	return							result;
}

template < class ConfigValueType >
joint* new_slider_joint	( ConfigValueType const& config, body* b0, body* b1, float4x4 const& transform )
{
	XRAY_UNREFERENCED_PARAMETERS	( &config, &transform );
	joint* result					=  XRAY_NEW_IMPL(xray::physics::g_ph_allocator, joint_slider_hinge );
	result->connect					( b0, b1 );
	return							result;
}

template < class ConfigValueType >
joint* new_joint	( ConfigValueType const& config, body* b0, body* b1, float4x4 const& transform )
{
	if ( strings::equal( config["type"], "fixed" ) )
		return		new_fixed_joint( config, b0, b1, transform );
	
	else if ( strings::equal( config["type"], "ball" ) )
		return		new_ball_joint( config, b0, b1, transform );
	
	else if ( strings::equal( config["type"], "generic" ) )
		return		new_generic_joint( config, b0, b1, transform );
	
	else if ( strings::equal( config["type"], "wheel" ) )
		return		new_wheel_joint( config, b0, b1, transform );
	
	else if ( strings::equal( config["type"], "slider" ) )
		return		new_slider_joint( config, b0, b1, transform );

	UNREACHABLE_CODE( return new_fixed_joint( config, b0, b1, transform ) );
}

template < class ConfigValueType >
joint* new_joint		( ConfigValueType const& config, ConfigValueType const& elements, ::shell& shell_body )
{
	float4x4					transform; 
	float3						scale;
	collision::read_transform	( config, transform, scale );

	shell_element& e0			= shell_body.get_element( find_index( elements, pcstr( config["body0"] ) ) );
	shell_element& e1			= shell_body.get_element( find_index( elements, pcstr( config["body1"] ) ) );
	body* b0					= e0.get_body();
	body* b1					= e1.get_body();
	ASSERT						( b0 );
	ASSERT						( b1 );

	joint* result				= new_joint( config, b0, b1, transform );

	result->set_anchor			( transform.c.xyz() );
//	sh.add_joint				( res );
	return						result;
}

template < class ConfigValueType >
non_null< physics::shell >::ptr physics_world::new_shell_impl( ConfigValueType const& config )
{
	::shell* shell						= XRAY_NEW_IMPL(xray::physics::g_ph_allocator, ::shell )(*xray::physics::g_ph_allocator);
	
	ConfigValueType const bodies_table	= config["bodies"];
	ConfigValueType const joints_table	= config["joints"];
	u32 const bodies_counter			= bodies_table.size();

	ASSERT								( bodies_counter > 0, "Bad physics shell settings: no bodies!" );

	for ( u32 bi = 0; bi < bodies_counter; ++bi )
	{
		shell->add_element				( new_element( bodies_table[bi] ) );
	}

	u32 const joints_counter			= joints_table.size();
	for ( u32 ji = 1; ji < joints_counter; ++ji )
	{
		shell->add_joint				( new_joint( joints_table[ji], bodies_table, *shell ) );
	}
	
	shell->space_insert					( *m_space );
	shell->wake_up						( *m_scheduler );
	return								shell;
}

#ifndef MASTER_GOLD
	non_null< physics::shell >::ptr physics_world::new_physics_shell( configs::lua_config_value const& config )
	{
		return							new_shell_impl( config );
	}
#endif // #ifndef MASTER_GOLD

non_null< physics::shell >::ptr physics_world::new_physics_shell( configs::binary_config_value const& config )
{
	return new_shell_impl				( config );
}

} // namespace physics
} // namespace xray