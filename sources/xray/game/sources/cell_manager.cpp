////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "cell_manager.h"
#include "game_world.h"
#include "object.h"
#include "game.h"
#include <xray/render/world.h>
#include <xray/render/facade/game_renderer.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/console_command.h>

namespace stalker2 {

bool g_cells_debug_draw		= true;
u32 g_cells_flood_depth		= 7;

static xray::console_commands::cc_bool		cells_debug_draw( "cells_debug_draw", g_cells_debug_draw, true, xray::console_commands::command_type_user_specific );

static xray::console_commands::cc_u32		cells_flood_depth( "cells_flood_depth", g_cells_flood_depth, 3, 1000, true, xray::console_commands::command_type_engine_internal );


cell_manager::cell_manager( game& game ) :
	m_game	( game )
{
}

void cell_manager::set_inv_view_matrix( float4x4 const& inv_view_matrix )
{
	m_camera_position	= inv_view_matrix.c.xyz();
	m_camera_direction	= inv_view_matrix.k.xyz();
}

void cell_manager::tick( )
{
	if(!m_game_project)
		return;
	
	if(!m_game_project->m_cells.empty())
		update_active_keys		( g_cells_flood_depth );


	if ( g_cells_debug_draw )
		debug_draw				( );
}

bool cell_manager::has_cell( cell_key const& k ) const
{
	return m_game_project->m_cells.find( k ) != m_game_project->m_cells.end();
}

void cell_manager::load( game_project_ptr const& game_project )
{
	m_game_project								= game_project;
	configs::binary_config_value	t_cells		= (*m_game_project->m_config)["cells"];

	if( t_cells.empty() ) // no cells
		return;

	configs::binary_config_value::const_iterator it			= t_cells.begin();
	configs::binary_config_value::const_iterator it_e		= t_cells.end();

	typedef vector<resources::request>	requests_vec;
	requests_vec						cell_requests;
	cell_requests.resize				( t_cells.size() );
	
	for( u32 idx =0 ;it!=it_e; ++it, ++idx)
	{
		resources::request& q		= cell_requests[idx];

		pcstr cell_name				= *it;
		q.path						= cell_name;
		q.id						= resources::game_cell_class;
	}

	LOG_WARNING("===%d cells queried", cell_requests.size());
	resources::query_resources(
		&cell_requests[0],
		cell_requests.size(),
		boost::bind( &cell_manager::game_cell_loaded, this, _1 ),
		g_allocator
		);
}

void cell_manager::game_cell_loaded( resources::queries_result& data )
{
	R_ASSERT				( data.is_successful() );
	u32 count				= data.size();
	
	float s = 64.0f*64.0f*count; // sq.m
	s			/= 1000000;
	LOG_WARNING("===%d cells loaded (%.2f square km)", count, s);
	for(u32 i=0; i<count; ++i )
	{
		game_cell_ptr cell	= static_cast_resource_ptr<game_cell_ptr>(data[i].get_unmanaged_resource( ) );
		pcstr cell_name		= data[i].get_requested_path( );
		cell_key k			= cell_key::parse( cell_name );
		
		// maybe cached in convertation procedure
		if( m_game_project->m_cells.find( k ) == m_game_project->m_cells.end( ) )
			m_game_project->m_cells[k]			= cell;

		cell->m_game		= &m_game;
		cell->load_contents	( this );
	}

	load_scenes		( );
}

void cell_manager::unload( )
{
	if(m_game_project)
	{
		cell_container::iterator it		= m_game_project->m_cells.begin();
		cell_container::iterator it_e	= m_game_project->m_cells.end();
		for( ; it!=it_e; ++it)
		{
			game_cell_ptr c			= it->second;
			c->unload				( );
		}	
		m_game_project->m_cells.clear	( );

		m_game_project					= 0;
	}

	m_active_keys.clear			( );
	m_loaded_keys.clear			( );
	m_current_key.invalidate	( );
	m_scenes.clear				( );
	m_named_registry.clear		( );
}

bool cell_manager::empty( ) const
{
	return (!m_game_project) || m_game_project->m_cells.empty( );
}

void cell_manager::update_active_keys( int max_depth )
{
	cell_key cam			= cell_key::pick( m_camera_position );

	if(m_current_key == cam)
		return;

	m_current_key			= cam;

	m_active_keys.clear		( );

	for( int ix=-max_depth; ix<=max_depth; ++ix)
		for( int iz=-max_depth; iz<=max_depth; ++iz)
			m_active_keys.push_back(cell_key(m_current_key.x+ix, m_current_key.z+iz));

	keys_container::const_iterator it	= m_active_keys.begin();
	keys_container::const_iterator it_e = m_active_keys.end();

	for( ; it!=it_e; ++it)
	{
		cell_key const& k = *it;
		if(m_loaded_keys.end()==std::find(m_loaded_keys.begin(), m_loaded_keys.end(), k))
		{
			m_loaded_keys.push_back			( k );

			if( has_cell( k ) )
				load_cell(k);
		}
	}

	keys_container temp_list = m_loaded_keys;

	it	= temp_list.begin();
	it_e = temp_list.end();
	for( ; it!=it_e; ++it)
	{
		cell_key const& k	= *it;
		int dx				= m_current_key.x - k.x;
		int dz				= m_current_key.z - k.z;
		float d				= math::sqrt( float(dx*dx) + float(dz*dz) );
		
		if( d > max_depth+2 )
		{
			keys_container::iterator it_del		= std::find(m_loaded_keys.begin(), m_loaded_keys.end(), k );
			m_loaded_keys.erase					( it_del );

			if( has_cell(k) )
				unload_cell(k);
		}
	}
}

bool cell_manager::load_cell( cell_key const& k )
{
	game_cell_ptr& c		= m_game_project->m_cells[ k ];
	c->load					( );
	LOG_INFO("load_CELL %d %d", k.x, k.z );
	return true;
}

bool cell_manager::unload_cell( cell_key const& k )
{
	game_cell_ptr& c		= m_game_project->m_cells[ k ];
	c->unload				( );
	LOG_INFO("UNload_CELL %d %d", k.x, k.z );

	return true;
}

void cell_manager::debug_draw( )
{

	cell_container::iterator it		= m_game_project->m_cells.begin();
	cell_container::iterator it_e	= m_game_project->m_cells.end();
	math::color						active_clr(1.0f, 1.0f, 1.0f, 1.0f);
	math::color						inactive_clr(0.0f, 0.0f, 1.0f, 1.0f);
	for( ; it!=it_e; ++it)
	{
		cell_key k					= it->first;
		bool bactive				= m_active_keys.end()!=std::find( m_active_keys.begin(), m_active_keys.end(), k );

		debug_draw_key				( k, bactive?active_clr:inactive_clr );
	}
	debug_draw_key				( m_current_key, math::color(1.0f, 0.0f, 0.0f, 1.0f ) );

}

void cell_manager::debug_draw_key( cell_key& k, math::color const& clr )
{
	XRAY_UNREFERENCED_PARAMETERS( &k, clr );
//	xray::render::debug::renderer& renderer = m_game.renderer().debug();
	float4x4 m;
	m.identity();
	m.c.xyz() = float3( k.x*64.0f+32, 0.0f, k.z*64.0f-32 );
//	renderer.draw_rectangle	( scene, m, float3(32.0f, 1, 32.0f), clr );
	
}

game_object_ptr_ cell_manager::get_object_by_name( pcstr object_id ) const
{
	named_object_registry::const_iterator it = m_named_registry.find( object_id );
	
	if(it!=m_named_registry.end())
		return it->second;
	else
		return NULL;
}

void cell_manager::query_object_by_name( pcstr object_id, object_loaded_callback_type const& callback  )
{
	named_object_registry::const_iterator iter = m_named_registry.find( shared_string( object_id ) );
	if( iter != m_named_registry.end( ) )
	{
		callback( iter->second );
		return;
	}

	queried_object_data data = { object_id , callback };
	m_queried_objects.push_back( data );
}

void cell_manager::register_object( shared_string const& name, game_object_ptr_ const& o )
{
	m_named_registry[name] = o;
	if( m_queried_objects.empty( ) )
		return;

	queried_objects_list::iterator current	= m_queried_objects.begin( );
	queried_objects_list::iterator end		= m_queried_objects.end( );

	for( ; current != end; ++current )
	{
		if( current->name == name )
		{
			current->callback( o );
			current = m_queried_objects.erase( current );
		}
	}
}

void cell_manager::load_scenes( )
{

	configs::binary_config_value t_objects		= (*m_game_project->m_config)["scenes"];
	
	configs::binary_config_value::const_iterator it			= t_objects.begin();
	configs::binary_config_value::const_iterator it_e		= t_objects.end();

	u32 const requests_size					= t_objects.size();
	buffer_vector< resources::creation_request > objects_requests( ALLOCA( sizeof( resources::creation_request ) * requests_size ), requests_size );

	for ( u32 i=0; it!=it_e; ++it, ++i )
	{
		configs::binary_config_value* t_object	= NEW(configs::binary_config_value)(*it);
		pcstr object_name					= (*t_object)["full_name"];
		objects_requests.push_back			( resources::creation_request( object_name, const_buffer( (void*)t_object, sizeof(configs::binary_config_value*) ), resources::game_object_scene_class ) );
	}

		resources::query_create_resources(
			&*objects_requests.begin(),
			requests_size,
			boost::bind( &cell_manager::on_scenes_loaded, this, _1 ),
			g_allocator
			);
}

void cell_manager::on_scenes_loaded(  resources::queries_result& data )
{
	R_ASSERT		( data.is_successful() );
	u32 count		= data.size();

	for( u32 i=0; i<count; ++i)
	{
		resources::query_result_for_user& q = data[i];
		const_buffer user_data				= q.creation_data_from_user();

		configs::binary_config_value* ud	= (configs::binary_config_value*)user_data.c_ptr();
		game_object_ptr_ object_ptr			= static_cast_resource_ptr<game_object_ptr_>(q.get_unmanaged_resource());
		register_object						( object_ptr->reusable_request_name().c_str(), object_ptr );
		m_scenes.push_back					( object_ptr );
		DELETE								( ud );
	}
	
	m_game.get_game_world().start_game		( );
}

} // namespace stalker2