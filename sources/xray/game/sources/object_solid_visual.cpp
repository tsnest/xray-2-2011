////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_solid_visual.h"
#include "game_world.h"
#include "game.h"
#include <xray/collision/space_partitioning_tree.h>
#include "collision_object_types.h"
#include <xray/render/facade/scene_renderer.h>
#include <xray/physics/rigid_body.h>


namespace stalker2{

object_visual::object_visual( game_world& w )
:super( w )
{
}

object_visual::~object_visual( )
{
	unload_contents( );
}

void object_visual::load( configs::binary_config_value const& t )
{
	super::load				( t );
	m_visual_name			= (pcstr)t["lib_name"];
}

object_solid_visual::object_solid_visual( game_world& w )
:super					( w ),
//m_collision				( NULL ),
m_physics_rigid_body	( NULL )
{}

void object_solid_visual::unload_contents( )
{
	if(m_physics_rigid_body)
	{
		m_game_world.get_physics_world()->remove_rigid_body( m_physics_rigid_body );
		physics::destroy_rigid_body		( m_physics_rigid_body );
		m_physics_rigid_body			= NULL;
		m_collision_shape				= NULL;
	}

	if( m_model )
	{
		m_game_world.get_game().renderer().scene().remove_model	( m_game_world.get_game().get_render_scene(), 
																	m_model->m_render_model );

		//m_game_world.get_collision_tree()->erase( m_collision );
		//m_collision->destroy( g_allocator );
		//DELETE			( m_collision );
	}
	m_model = NULL;
}

void object_solid_visual::load_contents( )
{
	if( m_model )
		return;

	fs::path_string model_collision_path;
	model_collision_path.assignf( "resources/models/%s.model/collision", m_visual_name.c_str() );

	resources::request r[]=
	{
		{ m_visual_name.c_str(), resources::static_model_instance_class },
		{ model_collision_path.c_str(), resources::collision_bt_shape_class_static },
	};

	resources::query_resources(
		r,
		boost::bind(&object_solid_visual::on_resources_ready, this, _1),
		g_allocator
	);
	
}

void object_solid_visual::on_resources_ready( resources::queries_result& data )
{
	if(!data.is_successful())
		return;

	if( m_model )
		return;

	m_model		= static_cast_resource_ptr<render::static_model_ptr>(data[0].get_unmanaged_resource());
	R_ASSERT	( m_model );

	m_game_world.get_game().renderer().scene().add_model( m_game_world.get_game().get_render_scene(), 
														m_model->m_render_model, m_transform );

	//m_collision	= NEW(collision::collision_object_geometry)(g_allocator, collision_object_static_model, m_model->m_collision_geom.c_ptr());

	//math::aabb					collision_aabb;
	//m_collision->get_object_geometry()->get_aabb	(collision_aabb);
	//float3 center				= collision_aabb.center	();
	//float3 extents				= collision_aabb.extents();
	//m_game_world.get_collision_tree()->insert(m_collision, center, extents);

	//m_collision->set_matrix	( m_transform );//world
	//m_game_world.get_collision_tree()->move( m_collision, m_collision->get_center(), m_collision->get_extents() );


	m_collision_shape = static_cast_resource_ptr<physics::bt_collision_shape_ptr>(data[1].get_unmanaged_resource());

	physics::bt_rigid_body_construction_info	info;
	info.m_collisionShape			= m_collision_shape;
	info.m_mass						= 0.0f;


	m_physics_rigid_body			= physics::create_rigid_body( info );

	float3 local_center				= m_physics_rigid_body->render_model_offset();
	float4x4 t						= m_transform;
	t.c.xyz()						+= local_center;
	m_physics_rigid_body->set_transform( t );
	
	m_game_world.get_physics_world()->add_rigid_body( m_physics_rigid_body );
}

object_dynamic_visual::object_dynamic_visual( game_world& w )
:super					( w ),
m_physics_rigid_body	( NULL )
{}

void object_dynamic_visual::unload_contents( )
{
	if(m_physics_rigid_body)
	{
		m_game_world.get_physics_world()->remove_rigid_body( m_physics_rigid_body );
		physics::destroy_rigid_body		( m_physics_rigid_body );
		m_physics_rigid_body			= NULL;
		m_collision_shape				= NULL;
	}

	if( m_model )
	{
		m_game_world.get_game().renderer().scene().remove_model	( m_game_world.get_game().get_render_scene(), 
																	m_model->m_render_model );
		m_model = NULL;
	}
}

void object_dynamic_visual::load_contents( )
{
	if( m_model )
		return;

	fs::path_string model_collision_path;
	model_collision_path.assignf( "resources/models/%s.model/collision", m_visual_name.c_str() );

	fs::path_string model_config_path;
	model_config_path.assignf( "resources/models/%s.model/settings", m_visual_name.c_str() );
		

	resources::request	r[]={
		{ m_visual_name.c_str(),		resources::static_model_instance_class },
		{ model_collision_path.c_str(), resources::collision_bt_shape_class_dynamic },
		{ model_config_path.c_str(),	resources::binary_config_class }
	};


	resources::query_resources(
		r,
		boost::bind(&object_dynamic_visual::on_resources_ready, this, _1),
		g_allocator
	);
	
}

void object_dynamic_visual::on_resources_ready( resources::queries_result& data )
{
	if(!data.is_successful())
		return;

	if( m_model )
		return;

	m_model		= static_cast_resource_ptr<render::static_model_ptr>(data[0].get_unmanaged_resource());
	R_ASSERT	( m_model );

	m_game_world.get_game().renderer().scene().add_model( m_game_world.get_game().get_render_scene(), 
															m_model->m_render_model, 
															m_transform );

	m_collision_shape				= static_cast_resource_ptr<physics::bt_collision_shape_ptr>(data[1].get_unmanaged_resource());
	configs::binary_config_ptr cfg = static_cast_resource_ptr<configs::binary_config_ptr>(data[2].get_unmanaged_resource());
	
	physics::bt_rigid_body_construction_info	info;
	info.m_collisionShape			= m_collision_shape;
	info.load						( cfg->get_root() );
	
	m_physics_rigid_body			= physics::create_rigid_body( info );

	m_physics_rigid_body->set_transform( m_transform );
	
	m_physics_rigid_body->set_moved_callback			( boost::bind(&object_dynamic_visual::physics_update_transform, this) );
	m_game_world.get_physics_world()->add_rigid_body	( m_physics_rigid_body );
}


void object_dynamic_visual::set_transform( float4x4 const& m )
{
	m_transform		= m;
	if(m_model)
	{
		m_game_world.get_game().renderer().scene().update_model( m_game_world.get_game().get_render_scene(), 
																m_model->m_render_model, 
																m_transform );
	}
}

void object_dynamic_visual::physics_update_transform( )
{
	float4x4 t			= m_physics_rigid_body->get_transform();
	set_transform		( t );
}

object_skeleton_visual::object_skeleton_visual( game_world& w )
:super( w )
{}

void object_skeleton_visual::unload_contents( )
{
	if( m_model )
		m_game_world.get_game().renderer().scene().remove_model( m_game_world.get_game().get_render_scene(), 
																m_model->m_render_model );
	
	m_model = NULL;
}

void object_skeleton_visual::load_contents( )
{
	if( m_model )
		return;

	resources::query_resource(
		m_visual_name.c_str(),
		resources::skeleton_model_instance_class,
		boost::bind(&object_skeleton_visual::on_visual_ready, this, _1),
		g_allocator
	);
}

void object_skeleton_visual::on_visual_ready( resources::queries_result& data )
{
	R_ASSERT	( data.is_successful() );
	if( !m_model )
	{
		m_model		= static_cast_resource_ptr<render::skeleton_model_ptr>(data[0].get_unmanaged_resource());
		R_ASSERT	( m_model );

		m_game_world.get_game().renderer().scene().add_model(	m_game_world.get_game().get_render_scene(), 
																m_model->m_render_model, 
																m_transform );
	}
}

object_particle_visual::object_particle_visual( game_world& w )
:super		( w ),
m_playing	( false )
{}

void object_particle_visual::unload_contents( )
{
	if (m_particle_system_instance_ptr)
		m_game_world.get_game().renderer().scene().remove_particle_system_instance( m_game_world.get_game().get_render_scene(), 
																					m_particle_system_instance_ptr );
	m_particle_system_instance_ptr	= NULL;
	m_playing						= false;
}

void object_particle_visual::load_contents( )
{
	if( m_playing )
		return;
	
	resources::user_data_variant* ud = NEW(resources::user_data_variant)();
	ud->set( &m_game_world.get_game().renderer().scene().particle_world( m_game_world.get_game().get_render_scene() ) );
	
	resources::query_resource(
		m_visual_name.c_str(),
		resources::particle_system_instance_class,
		boost::bind(&object_particle_visual::on_visual_ready, this, _1),
		g_allocator,
		ud
	);
	DELETE(ud);
}

void object_particle_visual::on_visual_ready( resources::queries_result& data )
{
	R_ASSERT	( data.is_successful() );
	
	if (!m_playing)
	{
		m_particle_system_instance_ptr = static_cast_resource_ptr<xray::particle::particle_system_instance_ptr>( data[0].get_unmanaged_resource());
		m_game_world.get_game().renderer().scene().play_particle_system( m_game_world.get_game().get_render_scene(), m_particle_system_instance_ptr, m_transform );
		m_playing = true;
	}
}

object_speedtree_visual::object_speedtree_visual( game_world& w )
:super( w )
{}

void object_speedtree_visual::unload_contents( )
{
	if (m_speedtree_instance_ptr.c_ptr())
		m_game_world.get_game().renderer().scene().remove_speedtree_instance(m_game_world.get_game().get_render_scene(), m_speedtree_instance_ptr, !m_game_world.is_loading_or_unloading() );	
	
	m_speedtree_instance_ptr = NULL;
}

void object_speedtree_visual::load_contents( )
{
 	resources::query_resource(
 		m_visual_name.c_str(),
 		resources::speedtree_instance_class,
 		boost::bind(&object_speedtree_visual::on_visual_ready, this, _1),
 		g_allocator
 		);	
}

void object_speedtree_visual::on_visual_ready( resources::queries_result& data )
{
 	R_ASSERT	( data.is_successful() );
 	
	m_speedtree_instance_ptr = static_cast_resource_ptr<xray::render::speedtree_instance_ptr>( data[0].get_unmanaged_resource() );
	m_game_world.get_game().renderer().scene().add_speedtree_instance( m_game_world.get_game().get_render_scene(), m_speedtree_instance_ptr, m_transform, !m_game_world.is_loading_or_unloading() );
}
} //namespace stalker2
