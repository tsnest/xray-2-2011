////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "colliders_aabb_object.h"
#include "loose_oct_tree.h"

using xray::collision::colliders::aabb_object;
using xray::collision::query_type;
using xray::math::float3;
using xray::collision::triangles_type;
using xray::collision::objects_type;
using xray::math::intersection;
using xray::non_null;
using xray::collision::object;
using xray::collision::oct_node;

aabb_object::aabb_object					(
		query_type const query_type,
		xray::collision::loose_oct_tree const& tree,
		xray::math::aabb const& aabb,
		objects_type& objects
	) :
	super					( aabb ),
	m_objects				( &objects ),
	m_triangles				( 0 ),
	m_tree					( tree),
	m_query_type			( query_type )
{
	process					( );
}

aabb_object::aabb_object					(
		query_type const query_type,
		xray::collision::loose_oct_tree const& tree,
		xray::math::aabb const& aabb,
		triangles_type& triangles
	) :
	super					( aabb ),
	m_objects				( 0 ),
	m_triangles				( &triangles ),
	m_tree					( tree),
	m_query_type			( query_type )
{
	process					( );
}

void aabb_object::process					( )
{
	u32 const size_before	= m_objects ? m_objects->size( ) : m_triangles->size( );
	query					( m_tree.root( ), m_tree.aabb_center( ), m_tree.aabb_extents( ) );
	m_result				= ( m_objects ? m_objects->size() : m_triangles->size() ) > size_before;
}

void aabb_object::add_objects				( non_null<oct_node const>::ptr const node ) const
{
	oct_node const* const* const b	= node->octants;
	oct_node const* const* i		= b;
	oct_node const* const* const e	= b + 8;
	for ( ; i != e; ++i ) {
		if ( !*i )
			continue;

		add_objects			( *i );
	}

	for ( collision::object const* i = node->objects; i; i = i->get_next() )
		if ( i->is_type_suitable(m_query_type) )
			m_objects->push_back( &*i );
}

void aabb_object::add_triangles				( non_null<oct_node const>::ptr const node ) const
{
	oct_node const* const* const b	= node->octants;
	oct_node const* const* i		= b;
	oct_node const* const* const e	= b + 8;
	for ( ; i != e; ++i ) {
		if ( !*i )
			continue;

		add_triangles		( *i );
	}

	for ( collision::object const* i = node->objects; i; i = i->get_next() )
		(*i).add_triangles	( *m_triangles );
}

void aabb_object::query						( non_null<oct_node const>::ptr const node, float3 const& aabb_center, float const aabb_extents ) const
{
	float3 const aabb_radius_vector ( aabb_extents, aabb_extents, aabb_extents );
	if ( out_of_bounds( m_aabb, aabb_center, aabb_radius_vector) )
		return;

	if ( completely_inside( m_aabb, aabb_center, aabb_radius_vector ) ) {
		if ( m_objects )
			add_objects		( node );
		else
			add_triangles	( node );

		return;
	}

	float const octant_radius = aabb_extents/2.f;

	oct_node const* const* const b	= node->octants;
	oct_node const* const* i		= b;
	oct_node const* const* const e	= b + 8;
	for ( ; i != e; ++i ) {
		if ( !*i )
			continue;

		query				( *i, aabb_center + octant_vector(i - b)*octant_radius, octant_radius );
	}

	for ( collision::object* i = node->objects; i; i = i->get_next() ) {
		if ( !i->is_type_suitable(m_query_type) )
			continue;

		if ( out_of_bounds( m_aabb, i->get_aabb().center(), i->get_aabb().extents() ) )
			continue;

		if ( m_triangles ) {
			i->aabb_query	( m_aabb, *m_triangles );
			continue;
		}

		if ( !i->aabb_test(m_aabb) )
			continue;
		
		m_objects->push_back( &*i );
	}
}