////////////////////////////////////////////////////////////////////////////
//	Created		: 04.06.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "cell.h"
#include "cell_manager.h"
#include "game_world.h"
#include "object.h"
#include "game.h"
#include <xray/render/facade/game_renderer.h>
#include <xray/render/world.h>
#include <xray/render/facade/scene_renderer.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/render/facade/terrain_base.h>
#include <xray/physics/rigid_body.h>
#include <xray/physics/world.h>
#include "collision_object_types.h"

namespace stalker2 {

#define cell_size (64.0f)

cell_key cell_key::pick( float3 const& point )
{
	cell_key			result;
	result.x			= (int)((point.x) / cell_size) -1;
	result.z			= (int)((point.z) / cell_size);
	if(point.z>0.0f)	++result.z;
	if(point.x>0.0f)	++result.x;

	return result;
}

cell_key cell_key::parse( pcstr key_str )
{
	cell_key			result;
	int offset			= 0;
	while( pcstr c = strchr(key_str+offset, '/') )
		offset = (int)(c - key_str) + 1;

	int count			= XRAY_SSCANF(key_str+offset, "%d_%d", &result.x, &result.z);
	R_ASSERT_U			(count == 2);
	return				result;
}

cell_key::fixed_string16 cell_key::to_string( ) const
{
	fixed_string16		result;
	result.assignf		( "%d_%d", x, z );
	return				result;
}

game_cell::game_cell		( /*xray::render::scene_ptr const& scene*/ ) :
//	m_scene					( scene ),
	m_game					( NULL ),
	m_quality				( false ),
	m_visibility_parameters	( 0.f )
{
}

void game_cell::load( )
{
	if( m_quality ) // already
		return;
	
	m_quality = true;

	if( contents_loaded() )
		load_impl	( );

	load_terrain	( );
}

void game_cell::unload( )
{
	if( !m_quality ) // already
		return;
	
	m_quality = false;

	if( contents_loaded() )
		unload_impl		( );

	unload_terrain		( );
}

void game_cell::load_impl( )
{
	R_ASSERT					( m_quality );
	object_list::iterator it	= m_objects.begin();
	object_list::iterator it_e	= m_objects.end();
	
	for(; it!=it_e; ++it)
	{
		game_object_ptr_& o		= *it;
		
		++o->m_loaded_cnt;
		if(o->m_loaded_cnt==1)
			o->load_contents		( );
	}
}

void game_cell::unload_impl( )
{
	object_list::iterator it	= m_objects.begin();
	object_list::iterator it_e	= m_objects.end();
	
	for(; it!=it_e; ++it)
	{
		game_object_ptr_& o		= *it;
		--o->m_loaded_cnt;
		if(o->m_loaded_cnt==0)
			o->unload_contents		( );
	}

	m_quality					= false;
}

void game_cell::load_contents( cell_manager* cm )
{
	m_cell_manager = cm;

	if( (*m_config).get_root().value_exists("objects"))
	{
		configs::binary_config_value t_objects	= (*m_config)["objects"];
		
		configs::binary_config_value::const_iterator it			= t_objects.begin();
		configs::binary_config_value::const_iterator it_e		= t_objects.end();

		u32 const requests_size					= t_objects.size();
		buffer_vector< resources::creation_request > objects_requests( ALLOCA( sizeof( resources::creation_request ) * requests_size ), requests_size );

		for ( u32 i=0; it!=it_e; ++it, ++i )
		{
			configs::binary_config_value* t_object	= NEW(configs::binary_config_value)(*it);
			pcstr object_name					= (*t_object)["full_name"];
			
			resources::class_id_enum clsid = resources::game_object_class;
			if ( it->value_exists("game_object_type") && strings::equal( (*it)["game_object_type"], "human_npc" ) )
				clsid	= resources::human_npc_class;

			objects_requests.push_back		( resources::creation_request( object_name, const_buffer( (void*)t_object, sizeof(configs::binary_config_value*) ), clsid ) );
		}

		resources::query_create_resources(
			&*objects_requests.begin(),
			requests_size,
			boost::bind( &game_cell::on_contents_loaded, this, _1 ),
			g_allocator
			);
	}
}

void game_cell::on_contents_loaded(  resources::queries_result& data )
{
	R_ASSERT		( data.is_successful() );
	u32 count		= data.size();

	for( u32 i=0; i<count; ++i)
	{
		resources::query_result_for_user& q = data[i];
		const_buffer user_data			= q.creation_data_from_user();

		configs::binary_config_value* ud	= (configs::binary_config_value*)user_data.c_ptr();

		game_object_ptr_ object_ptr		= static_cast_resource_ptr<game_object_ptr_>(q.get_unmanaged_resource());
		m_objects.push_back				( object_ptr );
		m_cell_manager->register_object	( object_ptr->reusable_request_name().c_str(), object_ptr );
		DELETE							( ud );
	}

	if( m_quality )
		load_impl();
}

bool game_cell::contents_loaded( ) const
{
	return !m_objects.empty();
}

void game_cell::unload_terrain( )
{
	if(m_terrain)
	{
		m_game->renderer().scene().terrain_remove_cell( m_game->get_render_scene(), m_terrain->m_render_model );
		m_game->get_game_world().get_collision_tree()->erase	( m_terrain_collision );
		m_terrain_collision->destroy( g_allocator );
		DELETE					( m_terrain_collision );
		m_terrain				= 0;
	
		m_game->get_game_world().get_physics_world()->remove_rigid_body( m_terrain_rigid_body );
		physics::destroy_rigid_body( m_terrain_rigid_body );
		m_terrain_rigid_body	= NULL;
	}
}

void game_cell::load_terrain( )
{
	if ( m_terrain )
		return;

	if( (*m_config).get_root().value_exists("terrain"))
	{
		configs::binary_config_value t_terrain	= (*m_config)["terrain"];
		math::int2 k							= t_terrain["key"];
		fs_new::virtual_path_string	query_path;
		
		pcstr terrain_prefix					= (*m_config)["guid"];

		query_path.assignf						("terrain%s/%d_%d.terr", terrain_prefix, k.x, k.y);

		render::terrain_cook_user_data			prj_name_terrain_pair; 
		prj_name_terrain_pair.current_project_resource_path	= m_game->get_game_world().project_resource_path();
		prj_name_terrain_pair.scene							= m_game->get_render_scene();

		resources::user_data_variant user_data;
		user_data.set							( prj_name_terrain_pair );

		resources::query_resource(
			query_path.c_str(),
			resources::terrain_model_class,
			boost::bind(&game_cell::on_terrain_visual_ready, this, _1),
			g_allocator,
			&user_data
		);
	}
}

void game_cell::on_terrain_visual_ready( resources::queries_result& data )
{
	if(m_quality)
	{
		R_ASSERT				( data.is_successful() );
		m_terrain				= static_cast_resource_ptr<render::terrain_model_ptr>( data[0].get_unmanaged_resource() );

		m_game->renderer().scene().terrain_add_cell( m_game->get_render_scene(), m_terrain->m_render_model );

		m_terrain_collision		= NEW(collision::collision_object)(g_allocator, collision_object_type_terrain, m_terrain->m_collision_geom, &m_visibility_parameters );
		

		configs::binary_config_value t_terrain	= (*m_config)["terrain"];
		
		float sz								= cell_size; //t_terrain["size"];
		math::int2 k							= t_terrain["key"];
		float4x4 terrain_transform				= math::create_translation( float3(sz*k.x, 0.0f, sz*k.y) );
//		m_terrain_collision->set_matrix			( terrain_transform );
//		float3 center				= m_terrain_collision->get_aabb_center();

		m_game->get_game_world().get_collision_tree()->insert	(m_terrain_collision, terrain_transform );
//		m_game->get_game_world().get_collision_tree()->insert	(m_terrain_collision, center, extents);

	physics::bt_rigid_body_construction_info	info;
	info.m_collisionShape			= m_terrain->m_bt_collision_shape;
	info.m_mass						= 0.0f;
	info.m_friction					= 100.0f;
	math::aabb bb = m_terrain_collision->get_aabb();
	info.m_render_model_offset		= float3( sz/2.0f, bb.min.y+(bb.max.y-bb.min.y)/2.0f, -sz/2.0f );

	m_terrain_rigid_body			= physics::create_rigid_body( info );

	m_terrain_rigid_body->set_transform( terrain_transform );
	
	m_game->get_game_world().get_physics_world()->add_rigid_body( m_terrain_rigid_body );

	}
}

} // namespace stalker2
