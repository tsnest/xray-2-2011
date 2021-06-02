////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game.h"
#include "game_world.h"
#include <xray/render/world.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/facade/game_renderer.h>
#include <xray/input/api.h>
#include <xray/collision/geometry_instance.h>

#include <xray/collision/object.h>
#include <xray/physics/world.h>
#include <xray/console_command_processor.h>

// temporary for debug purposes
//static xray::collision::geometry*	s_collision			= NULL;
//static xray::collision::object*		s_collision_object	= NULL;
//static xray::physics::shell*		s_statics_shell		= NULL;

namespace stalker2 {

void game::test					( )
{
	return;
#if 0
	u32 const						line_count = 200;
	float const						step = line_count;
	u32 const line_color			= math::color_xrgb(64, 64, 64);
	for ( u32 i=0; i<=line_count; ++i)
		m_render_world.game().debug().draw_line	( float3( -step*.5f + i, 0.f, -step*.5f ), float3( -step*.5f + i, 0.f, step*.5f ), line_color );

	for ( u32 i=0; i<=line_count; ++i)
		m_render_world.game().debug().draw_line	( float3( -step*.5f, 0.f, -step*.5f + i ), float3( step*.5f, 0.f, -step*.5f + i ), line_color );

	//m_physics.render				( m_render.game().debug() );

	//if ( s_collision )
		//s_collision->render			( m_render, dynamic_cast<xray::collision::geometry_instance const*>( s_collision_object )->get_matrix() );

#if 0
	// this is memory leak, just for test purposes
	static non_null<collision::space_partitioning_tree>::ptr tree = m_collision.create_tree( g_allocator, 1.f, 1024 );
	static collision_object_geometry objects[4];
	static bool initialized			= false;
	if ( !initialized ) {
		initialized	= true;
		tree->insert( &objects[0], float3( 0.f, 0.f, 0.f),			float3( 2.1f, 4.1f, 1.1f ) );
		tree->insert( &objects[1], float3( 1.f, 1.f, 1.f),			float3( 2.1f, 4.1f, 1.1f ) );
		tree->insert( &objects[2], float3( 4.f, 5.f, 10.f),			float3( 2.1f, 4.1f, 1.1f ) );
		tree->insert( &objects[3], float3( -3.f, -10.f, 10.f),		float3( 2.1f, 4.1f, 1.1f ) );
	}

	static float3	offset( 0.f, 0.f, 0.f );
	static int counter = 0;	
	offset			+= float3( 0.f, 0.f, 0.01f);

	tree->move		( &objects[0], float3( 0.f, 0.f, 0.f) + offset,	float3( 2.1f, 4.1f, 1.1f ) );
	tree->render	( m_render );
	++counter;

	collision::Results				temporary( g_allocator );

	if (
			tree->ray_query(
				collision::query_type_object,
				m_inverted_view.c.xyz( ),
				m_inverted_view.k.xyz( ),
				1000.f,
				temporary,
//				collision::Predicate( this, &game_world::save_triangle )
				collision::Predicate( this, &game_world::save_object )
			) && m_collision_object
		)
		m_collision_object->render		( m_render );
//		m_render.debug().draw_triangle	( m_triangle[0], m_triangle[1], m_triangle[2], math::color_xrgb( 255, 0, 0 ) );
#endif // #if 0
#endif // #if 0

#if 0
#if 1
	collision::Results				temporary( g_allocator );
	float distance;
	if (
			s_collision_object
		) {
		s_collision_object->ray_query(
			collision::query_type( object_type_geometry + object_type_object ),
			m_inverted_view.c.xyz( ),
			m_inverted_view.k.xyz( ),
			1000.f,
			distance,
			temporary,
			collision::Predicate( this, &game_world::save_triangle )
		);
		if ( !temporary.empty() ) {
			struct distance {
				static inline bool predicate	( collision::result const& left, collision::result const& right )
				{
					return left.distance < right.distance;
				}
			}; // struct distance
			std::sort		( temporary.begin(), temporary.end(), &distance::predicate );
			save_triangle	( temporary.front() );
			m_render.debug().draw_triangle	( m_triangle[0], m_triangle[1], m_triangle[2], math::color_xrgb( 255, 0, 0 ) );
		}
	}
#else // #if 0
	using collision::Results;
	Results	triangles				( g_allocator );
	if (
			m_collision.root() &&
			m_collision.ray_query(
				m_inverted_view.c.xyz( ),
				m_inverted_view.k.xyz( ),
				1000.f,
				triangles,
				collision::Predicate( this, &game_world::save_triangle )
			)	
		) {
#	if 1
		float3 const center			= ( m_triangle[0] + m_triangle[1] + m_triangle[2] )*.333f;
		float3 const half_size		= float3( .5f, .5f, .5f );
		if ( m_collision.aabb_query( math::aabb( center - half_size, center + half_size ), triangles ) ) {
			Results::const_iterator	i = triangles.begin( );
			Results::const_iterator	e = triangles.end( );
			for ( ; i != e; ++i ) {
				float3 const* const vertices	= (*i).cell->vertices( );
				u32 const* const indices		= (*i).cell->indices( (*i).triangle_id );
				m_render.debug().draw_triangle	(
					vertices[ indices[0] ],
					vertices[ indices[1] ],
					vertices[ indices[2] ],
					math::color_xrgb( 255, 0, 0 )
				);
			}
		}
#	else // #if 0
		float3 const center			= ( m_triangle[0] + m_triangle[1] + m_triangle[2] )*.333f;
		float const aspect_ratio	= 4.f/3.f;
		float4x4 const projection	= math::create_perspective_projection( math::pi_d4/aspect_ratio, aspect_ratio, .2f, 500.0f); //( m_inverted_view.c.xyz() - center ).length( ) );
		float4x4 const full			= math::mul4x4( invert( m_inverted_view ), projection );
		math::cuboid cuboid		( full );
		if ( m_collision.cuboid_query( cuboid, triangles ) ) {
			Results::const_iterator	i = triangles.begin( );
			Results::const_iterator	e = triangles.end( );
			for ( ; i != e; ++i ) {
				float3 const* const vertices	= (*i).cell->vertices( );
				u32 const* const indices		= (*i).cell->indices( (*i).triangle_id );
				m_render.debug().draw_triangle	(
					vertices[ indices[0] ],
					vertices[ indices[1] ],
					vertices[ indices[2] ],
					math::color_xrgb( 255, 0, 0 )
				);
			}
		}
#	endif // #if 0

	}

#endif // #if 0
#endif // #if 0

//	m_render.debug().draw_triangle	(
//		float3( 1.f, 0.f, 0.f),
//		float3( 0.f, 1.f, 0.f),
//		float3( 1.f, 1.f, 0.f),
//		math::color_xrgb( 255, 0, 0 )
//	);
}

} // namespace stalker2