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

#include "joint_ball.h"

#include <xray/configs.h>
#include <xray/collision/api.h>
#include <xray/collision/geometry_instance.h>

non_null<body>::ptr	new_body( const xray::configs::binary_config_value& cfg, 
							 const xray::configs::binary_config_value& root )
{
	XRAY_UNREFERENCED_PARAMETER	( root );
	body *b			= XRAY_NEW_IMPL( xray::physics::g_ph_allocator, body_ode );
	float4x4 pose	= math::create_rotation(cfg["angles"]);
	pose.c.xyz()	= ( cfg["mass_center"] );
	b->set_pose		( pose );

	return			b;
}

non_null<xray::collision::geometry_instance>::ptr	new_sphere_collision( const xray::configs::binary_config_value	&cfg , const xray::configs::binary_config_value &root )
{
	XRAY_UNREFERENCED_PARAMETER	( root );

	float4x4 pose = float4x4().identity();//

	pose.c.xyz() = cfg["position"] ;
	return collision::new_sphere_geometry_instance( xray::physics::g_ph_allocator, pose, cfg["radius"] );
}

non_null<xray::collision::geometry_instance>::ptr	new_box_collision( const xray::configs::binary_config_value	&cfg , const xray::configs::binary_config_value &root )
{
	XRAY_UNREFERENCED_PARAMETER	( root );
	return collision::new_box_geometry_instance( xray::physics::g_ph_allocator, math::create_scale(cfg["half_sides"]) * math::create_translation(cfg["position"]) );
}

non_null<xray::collision::geometry_instance>::ptr	new_cylinder_collision( const xray::configs::binary_config_value& cfg, const xray::configs::binary_config_value &root )
{
	XRAY_UNREFERENCED_PARAMETER	( root );
	float4x4 pose =float4x4().identity();
	pose.c.xyz() = cfg["position"] ;
	return collision::new_cylinder_geometry_instance( xray::physics::g_ph_allocator, pose, cfg["radius"], cfg["half_length"] );
}

non_null<xray::collision::geometry_instance>::ptr	new_collision( const xray::configs::binary_config_value	&cfg , const xray::configs::binary_config_value &root)
{
	if( strings::compare( cfg["type"], "sphere" ) == 0 )
		return new_sphere_collision( cfg,root);
	else if(strings::compare( cfg["type"], "box" ) == 0 )
		return new_box_collision( cfg,root);
	else if(strings::compare( cfg["type"], "cylinder" ) == 0 )
		return new_cylinder_collision( cfg,root);
	UNREACHABLE_CODE(return new_sphere_collision(cfg,root));
}

non_null<element>::ptr	new_element( const xray::configs::binary_config_value& cfg, const xray::configs::binary_config_value& root)
{
	
	non_null<body>::ptr body = new_body( root[ pcstr(cfg["body"]) ], root );

	non_null<xray::collision::geometry_instance>::ptr cf = new_collision( root[ pcstr( cfg["geometry"] ) ] , root );
	
	non_null<element>::ptr  e = XRAY_NEW_IMPL( xray::physics::g_ph_allocator, element )( *body, *cf );
	
	e->set_mass( cfg["mass"] );
	return e;
}

static u32 find_index( const xray::configs::binary_config_value	&section, pcstr element )
{
	xray::configs::binary_config_value::const_iterator b = section.begin();
	xray::configs::binary_config_value::const_iterator i = section.begin();
	xray::configs::binary_config_value::const_iterator e = section.end();
	for( ; i!=e; ++i )
		if( strings::compare( pcstr(*i), element ) )
			return u32( i - b );
		
	ASSERT(false);
	return u32(-1);
}

joint* new_joint(const xray::configs::binary_config_value &cfg, const xray::configs::binary_config_value &elements, shell &sh )
{
	shell_element& e0	= sh.get_element( find_index( elements, pcstr(cfg["e0"]) ) );
	shell_element& e1	= sh.get_element( find_index( elements, pcstr(cfg["e1"]) ) );
	joint* res			= XRAY_NEW_IMPL( xray::physics::g_ph_allocator, joint_ball)();
	body* b0			= e0.get_body();
	body* b1			= e1.get_body();
	ASSERT				( b0 );
	ASSERT				( b1 );
	//res->set_anchor	( cfg["position"] );
	res->connect		( b0, b1 );
	res->set_anchor		( cfg["position"] );
//	sh.add_joint		( res );
	return				res;
}
/*
non_null<xray::physics::shell>::ptr	physics_world::new_physics_shell	( const xray::configs::binary_config_value	&root  )
{
	
	const xray::configs::binary_config_value cfg = root["shell"];

	shell	*s =NEW( shell );
	const xray::configs::binary_config_value	&elements = root[pcstr(cfg["elements"])];
	{
	
	xray::configs::binary_config_value::const_iterator i = elements.begin();
	xray::configs::binary_config_value::const_iterator e = elements.end();
	for( ; i!=e; ++i )
		s->add_element( new_element( root[pcstr(*i)], root ) );
	}
	if( strings::compare( pcstr( cfg["joints"] ), "none" ) !=0 )
	{
		const xray::configs::binary_config_value	&joints = root[pcstr(cfg["joints"])];
		xray::configs::binary_config_value::const_iterator i = joints.begin();
		xray::configs::binary_config_value::const_iterator e = joints.end();
		for( ; i!=e; ++i )
			s->add_joint( new_joint(  root[pcstr(*i)], elements, *s ) );
	}

	s->space_insert( *m_space );
	s->wake_up( *m_scheduler );
	return s;
}

*/
