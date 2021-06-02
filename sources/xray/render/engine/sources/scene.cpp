////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/custom_config.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/backend.h>
#include "scene.h"
//#include "model_manager.h"
#include "lights_db.h"
#include "terrain.h"
#include "speedtree.h"
#include "speedtree_forest.h"
#include "speedtree_tree.h"
#include "speedtree_instance_impl.h"
#include "grass.h"
#include "render_particle_emitter_instance.h"
#include <xray/particle/world.h>
#include "light.h"
#include "statistics.h"
#include "system_renderer.h"
#include <xray/render/engine/world.h>
#include "decal_instance.h"
#include "material.h"
#include "material_manager.h"
#include <xray/collision/space_partitioning_tree.h>

#include <xray/render/core/options.h>

static const u32 s_max_vertex_count	= 64*1024;

static xray::command_line::key s_grass_key( "grass", "", "", "grass test" );
static bool is_grass_loaded	= false;


namespace xray {
namespace render {

particle::render_particle_emitter_instance* scene::particle_engine::create_render_emitter_instance	(
		particle::particle_emitter_instance& particle_emitter_instance,
		particle::particle_list_type const& particle_list,
		particle::billboard_parameters* billboard_parameters,
		particle::beamtrail_parameters* beamtrail_parameters,
		particle::enum_particle_locked_axis locked_axis,
		particle::enum_particle_screen_alignment screen_alignment,
		math::float4x4 const& transform,
		math::float4 const& instance_color
	)
{
	return
		MT_NEW( render_particle_emitter_instance ) (
			*m_particle_world,
			particle_emitter_instance,
			particle_list,
			billboard_parameters,
			beamtrail_parameters,
			locked_axis,
			screen_alignment,
			transform,
			instance_color
		);
}

void scene::particle_engine::destroy	( particle::render_particle_emitter_instance*& instance )
{
	render::render_particle_emitter_instance* render_instance = static_cast_checked< render::render_particle_emitter_instance*>(instance);
	MT_DELETE			( render_instance );
	instance			= 0;
}

// void scene::on_particle_world_created(resources::queries_result& result)
// {
// 	m_particle_world	= static_cast_checked<particle::world*>(result[0].get_unmanaged_resource().c_ptr());
// 	m_particle_engine.set_particle_world( *m_particle_world );
// }

static int test_grass = 0;

scene::scene( xray::render::editor_renderer_configuration const& renderer_configuration)
:
//.	m_models			( NEW(xray::render::model_manager) ),
	m_lights			( NEW(lights_db) ),
	m_speedtree_forest	( NEW(speedtree_forest)),
	m_terrain			( renderer_configuration.m_create_terrain ? NEW(render::terrain) : NULL ),
	m_particle_world	( NULL ),
	m_scene_slomo		( 1.0f ),
	m_grass				( NEW(grass_world) ),
	m_decals_tree		( &*collision::new_space_partitioning_tree( g_allocator, 1.f, 1024 ) ),
	m_models_tree		( &*collision::new_space_partitioning_tree( g_allocator, 1.f, 1024 ) )
{
	if (s_grass_key.is_set())
	{
		if (test_grass==1)
			m_grass->initialize_test_grass();
		
		test_grass++;
	}
}

scene::~scene()
{
	DELETE			( m_speedtree_forest );
	DELETE			( m_grass );
	
	collision::delete_space_partitioning_tree( m_decals_tree );
	collision::delete_space_partitioning_tree( m_models_tree );
}

xray::particle::world* scene::particle_world() 
{ 
	return m_particle_world.c_ptr(); 
}

void scene::add_speedtree_instance( speedtree_instance_ptr instance, math::float4x4 const& transform, bool populate_forest )
{
	m_speedtree_forest->add_instance(instance, transform);
	if(populate_forest)
		m_speedtree_forest->populate_forest();

}

void scene::remove_speedtree_instance( speedtree_instance_ptr instance, bool populate_forest  )
{
	m_speedtree_forest->remove_instance(instance);
	if(populate_forest)
		m_speedtree_forest->populate_forest();
}

void scene::set_speedtree_instance_transform ( speedtree_instance_ptr instance, math::float4x4 const& transform, bool populate_forest )
{
	m_speedtree_forest->set_transform(instance, transform);
	if(populate_forest)
		m_speedtree_forest->populate_forest();
}

void scene::populate_speedtree_forest( )
{
	m_speedtree_forest->populate_forest();
}

void scene::add_model( render_model_instance_impl_ptr v)
{
	R_ASSERT( std::find(m_render_model_instances.begin(), m_render_model_instances.end(), v) == m_render_model_instances.end());
	m_render_model_instances.push_back		( v );
	m_models_tree->insert( &v->m_collision_object, v->transform() );
}

void scene::modify_model( render_model_instance_impl_ptr v)
{
	R_ASSERT					( std::find(m_render_model_instances.begin(), m_render_model_instances.end(), v) != m_render_model_instances.end());
	m_models_tree->move			( &v->m_collision_object, v->transform() );
}

void scene::remove_model( render_model_instance_impl_ptr v)
{
	render_model_instances::iterator it	= std::find(m_render_model_instances.begin(), m_render_model_instances.end(), v);
	R_ASSERT					( it != m_render_model_instances.end() );
	m_models_tree->erase		( &v->m_collision_object);
	m_render_model_instances.erase	( it );
}

void scene::select_models( float4x4 const & mat_vp, render_surface_instances& selection )
{
	if (!options::ref().m_enabled_draw_models)
		return;
	
	BEGIN_TIMER(statistics::ref().visibility_stat_group.culling_time);

	if(options::ref().m_enabled_draw_models)
	{
		math::frustum view_frustum (mat_vp);	

		selection.clear();

		collision::objects_type query_result(render::g_allocator);
		m_models_tree->cuboid_query( u32(-1), view_frustum, query_result);

		selection.reserve( selection.size() + query_result.size());

		collision::objects_type::const_iterator end = query_result.end();
		for( collision::objects_type::iterator it = query_result.begin(); it != end; ++it)
		{
			collision_object<render_model_instance_impl> const* const object =
				static_cast_checked<collision_object<render_model_instance_impl> const*>(*it);

			object->owner()->get_surfaces( selection, true );
		}
	}
	END_TIMER;
}

void scene::update_models( )
{
//.	m_models->update_models( );
	BEGIN_TIMER(statistics::ref().visibility_stat_group.models_updating_time);
	
	render_model_instances::iterator		it	=	m_render_model_instances.begin();
	render_model_instances::const_iterator	end =	m_render_model_instances.end();

	for( ; it !=  end; ++it) 
		(*it)->update();

	END_TIMER;
}

void scene::select_terrain_cells	( float4x4 const & mat_vp, render::vector<terrain_render_model_instance_ptr> & cells)
{
	ASSERT( m_terrain);
	m_terrain->select_cells( mat_vp, cells);
}

void scene::add_light		( u32 id, render::light_props const& props)
{
	m_lights->add_light( id, props);
}

void scene::update_light		( u32 id, render::light_props const& props)
{
	m_lights->update_light( id, props);
}

void scene::remove_light		( u32 id)
{
	m_lights->remove_light( id);
}

void scene::add_decal(u32 id, render::decal_properties const& properties)
{
	m_decals.push_back						(NEW(decal_instance)(m_decals_tree, properties, id));
}

void scene::update_decal(u32 id, render::decal_properties const& properties)
{
	decal_instance*	instance				= m_decals.front();
	
	bool found								= false;
	while (instance)
	{
		if (instance->m_id == id)
		{
			instance->set_properties		(properties);
			found							= true;
		}
		instance							= m_decals.get_next_of_object(instance);
	}
	if (!found)
	{
		add_decal							(id, properties);
	}
}

void scene::remove_decal(u32 id)
{
	decal_instance*	instance				= m_decals.front();
	
	while (instance)
	{
		if (instance->m_id == id)
		{
			m_decals.erase					(instance);
			DELETE							(instance);
			break;
		}
		instance							= m_decals.get_next_of_object(instance);
	}
}

void scene::set_slomo			( float time_multiplier )
{
	m_scene_slomo = time_multiplier;
}

void scene::update_lines		( u32 const add_count )
{
	if ( m_line_indices.size( ) + add_count >= s_max_vertex_count )
		render_lines			( );

	ASSERT						( m_line_indices.size() + add_count < s_max_vertex_count );
}

void scene::render_lines		( bool covering_effect )
{
	if ( m_line_vertices.empty( ) ) {
		ASSERT					( m_line_indices.empty( ), "lines are empty, but not the pairs" );
		return;
	}

	ASSERT						( !m_line_indices.empty( ), "lines aren't empty, but not the pairs" );

	system_renderer::ref().draw_lines	(
		&*m_line_vertices.begin(),
		&*m_line_vertices.end(),
		&*m_line_indices.begin(),
		&*m_line_indices.end(),
		covering_effect
	);
	
	m_line_vertices.resize		( 0 );
	m_line_indices.resize		( 0 );
}

void scene::update_triangles	( u32 const add_count )
{
	R_ASSERT_CMP				( add_count, <=, s_max_vertex_count );
	R_ASSERT_CMP				( m_triangle_vertices.size(), <=, s_max_vertex_count );
	if ( m_triangle_vertices.size( ) + add_count >= s_max_vertex_count )
		render_triangles		( );

	ASSERT						( m_triangle_vertices.size( ) + add_count < s_max_vertex_count );
}

void scene::render_triangles	( bool covering_effect )
{
	if ( m_triangle_vertices.empty( ) )
		return;

	ASSERT						( ( m_triangle_indices.size( ) % 3 ) == 0, "triangle indices count isn't divisible by 3" );

	system_renderer::ref().draw_triangles	(
		&*m_triangle_vertices.begin(),
		&*m_triangle_vertices.end(),
		&*m_triangle_indices.begin(),
		&*m_triangle_indices.end(),
		covering_effect
	);
	m_triangle_vertices.resize	( 0 );
	m_triangle_indices.resize	( 0 );
}

void scene::draw_lines			( debug_vertices_type const& vertices, debug_indices_type const& indices )
{
	ASSERT						( indices.size( ) % 2 == 0 );
	update_lines				( indices.size( ) );

	u16	const n					= ( u16 ) m_line_vertices.size( );
	m_line_vertices.insert		( m_line_vertices.end(), vertices.begin( ), vertices.end( ) );

	debug_indices_type::const_iterator	i = indices.begin( ); 
	debug_indices_type::const_iterator	e = indices.end( );
	for ( ; i != e; ++i )
		m_line_indices.push_back( n + *i );
}

void scene::draw_triangles( debug_vertices_type const& vertices, debug_indices_type const& indices )
{
	update_triangles			( indices.size( ) );

	u16	const n					= ( u16 ) m_triangle_vertices.size( );
	m_triangle_vertices.insert	( m_triangle_vertices.end(), vertices.begin( ), vertices.end( ) );

	debug_indices_type::const_iterator	i = indices.begin( ); 
	debug_indices_type::const_iterator	e = indices.end( );
	for ( ; i != e; ++i )
		m_triangle_indices.push_back( n + *i );

	ASSERT						( ( m_triangle_indices.size( ) % 3 ) == 0, "triangle indices count isn't divisible by 3" );
}

void scene::flush( boost::function< void ( bool ) > const& on_draw_scene )
{
	backend::ref().reset_depth_stencil_target();

	on_draw_scene				( true );
	render_lines				( );
	render_triangles			( );

	backend::ref().reset_depth_stencil_target	( );
	backend::ref().clear_depth_stencil			( D3D_CLEAR_DEPTH|D3D_CLEAR_STENCIL, 1.0f, 0);

	// render selected models
	render_model_instances::const_iterator it			= m_selected_models.begin( );
	render_model_instances::const_iterator const it_e	= m_selected_models.end( );
	
	render_surface_instances list;
	for ( ;it != it_e; ++it ) 
	{
		render_model_instance_impl_ptr const model		= ( *it );
		model->get_surfaces								( list, true );
	}
	
	system_renderer::ref().draw_render_models_selection				( list );
	system_renderer::ref().draw_particle_system_instance_selections	( m_particle_system_instances );
	system_renderer::ref().draw_speedtree_instance_selections		( m_speedtree_instances );
	
	on_draw_scene				( false );
	render_lines				( true );
	render_triangles			( true );
}

void scene::select_particle_system_instance	( particle::particle_system_instance_ptr const& instance, bool const is_selected )
{
	particle_system_instances_type::iterator const found	= std::find( m_particle_system_instances.begin(), m_particle_system_instances.end(), instance );
	if ( found == m_particle_system_instances.end() ) {
		if ( is_selected )
			m_particle_system_instances.push_back ( instance );

		return;
	}
	
	if ( !is_selected )
		m_particle_system_instances.erase	( found );
}

void scene::select_speedtree_instance		( speedtree_instance_ptr const& instance, bool const is_selected )
{
	speedtree_instances_type::iterator const found	= std::find( m_speedtree_instances.begin(), m_speedtree_instances.end(), instance );
	if ( found == m_speedtree_instances.end() ) {
		if ( is_selected )
			m_speedtree_instances.push_back	( instance );

		return;
	}
	
	if ( !is_selected )
		m_speedtree_instances.erase			( found );
}

void scene::select_model( render_model_instance_impl_ptr const& instance, bool const is_selected )
{
	render_model_instances::iterator found		= std::find( m_selected_models.begin(), m_selected_models.end(), instance );
	if ( found == m_selected_models.end() ) 
	{
		if ( is_selected )
			m_selected_models.push_back		( instance );
		return;
	}
	
	if ( !is_selected )
		m_selected_models.erase				( found );
}

} // namespace render
} // namespace xray
