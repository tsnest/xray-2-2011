////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "loose_oct_tree.h"
#include <xray/render/facade/debug_renderer.h>
#include <xray/collision/object.h>
#include "colliders_ray_object.h"
#include "colliders_aabb_object.h"
#include "colliders_cuboid_object.h"

using xray::non_null;
using xray::collision::loose_oct_tree;
using xray::collision::object;
using xray::collision::ray_objects_type;
using xray::collision::objects_type;
using xray::collision::objects_predicate_type;
using xray::collision::ray_triangles_type;
using xray::collision::results_callback_type;
using xray::collision::triangles_type;
using xray::collision::triangles_predicate_type;
using xray::collision::oct_node;
//using xray::collision::query_mask;
using xray::collision::object_type;
using xray::collision::object;
using xray::math::intersection;

using namespace xray;

namespace xray {
namespace collision {

class vertex_allocator : private boost::noncopyable {
public:
			vertex_allocator	( memory::base_allocator* allocator, u32 const reserve_item_count ) :
		m_allocator			( allocator ),
		m_nodes				( 0 ),
		m_node_count		( 0 )
	{
		#pragma message ( XRAY_TODO( "use reserve_item_count here!" ) )
		XRAY_UNREFERENCED_PARAMETER(reserve_item_count);
	}

			~vertex_allocator	( )
	{
		while ( m_nodes ) {
			oct_node* node	= m_nodes;
			m_nodes			= m_nodes->parent;
			XRAY_DELETE_IMPL( m_allocator, node );
		}
	}

	oct_node* allocate			( )
	{
		++m_node_count;

		oct_node* result	= m_nodes;
		if ( result )
			m_nodes			= m_nodes->parent;
		else
			result			= XRAY_NEW_IMPL( m_allocator, oct_node );

		result->objects		= 0;
		result->parent		= 0;
		memory::zero		( result->octants, sizeof(result->octants) );
		return				( result );
	}

	void deallocate				( oct_node* node )
	{
		R_ASSERT			( m_node_count );
		--m_node_count;
		node->parent		= m_nodes;
		m_nodes				= node;
	}

	inline u32	node_count		( ) const
	{
		return				m_node_count;
	}

	inline memory::base_allocator& get_allocator( ) const
	{
		return				*m_allocator;
	}

private:
	mutable memory::base_allocator*	m_allocator;
	oct_node*						m_nodes;
	u32								m_node_count;
}; // class vertex_allocator

} // namespace collision
} // namespace xray

loose_oct_tree::loose_oct_tree		( memory::base_allocator* allocator, float const min_aabb_extents, u32 const reserve_item_count ) :
#ifdef DEBUG
	m_aabb_center			( math::SNaN, math::SNaN, math::SNaN ),
#endif // #ifdef DEBUG
	m_allocator				( XRAY_NEW_IMPL(allocator, vertex_allocator)( allocator, reserve_item_count) ),
	m_root					( 0 ),
#ifdef DEBUG
	m_aabb_extents			( math::SNaN ),
#endif // #ifdef DEBUG
	m_min_aabb_extents		( min_aabb_extents ),
	m_object_count			( 0 ),
	m_initialized			( false )
{
}

loose_oct_tree::~loose_oct_tree		( )
{
	if ( m_root )
		remove_nodes		( m_root );
	else
		R_ASSERT			( !m_initialized );

	memory::base_allocator* allocator = &m_allocator->get_allocator();
	XRAY_DELETE_IMPL		( allocator, m_allocator );
}

void loose_oct_tree::remove_nodes	( non_null<oct_node>::ptr node )
{
	oct_node ** const b		= node->octants;
	oct_node ** i			= b;
	oct_node ** const e		= b + 8;
	for ( ; i != e; ++i ) {
		if ( !*i )
			continue;

		remove_nodes		( *i );
	}

	m_allocator->deallocate	( &*node );
}

void loose_oct_tree::initialize		( non_null<object>::ptr object, float3 const& aabb_center, float3 const& aabb_extents )
{
	ASSERT					( !m_initialized );
	ASSERT					( !m_root );
	ASSERT					( !m_object_count );

	m_initialized			= true;

	m_root					= m_allocator->allocate();
	m_root->objects			= &*object;

	object->m_node			= m_root;
	object->m_next			= 0;

	oct_node ** const b		= m_root->octants;
	oct_node ** i			= b;
	oct_node ** const e		= b + 8;
	for ( ; i != e; ++i )
		*i					= 0;

	m_object_count			= 1;

	m_aabb_center			= aabb_center;

	if ( aabb_extents < float3( m_min_aabb_extents, m_min_aabb_extents, m_min_aabb_extents) ) {
		m_aabb_extents		= m_min_aabb_extents;
		return;
	}

	float3 const relation	= aabb_extents/m_min_aabb_extents;
	float const max_relation = math::max( relation.x, math::max( relation.y, relation.z ) );
	int const power			= math::floor( math::log( max_relation )/math::log( 2.f ) );
	m_aabb_extents			= m_min_aabb_extents * math::pow( 2.f, (float)power );
	if ( !(float3( m_aabb_extents, m_aabb_extents, m_aabb_extents ) >= aabb_extents) )
		m_aabb_extents		= m_min_aabb_extents * math::pow( 2.f, (float)(power+1) );

	ASSERT					( float3( m_aabb_extents, m_aabb_extents, m_aabb_extents ) >= aabb_extents );
}

static float make_non_zero			( float const value )
{
	if ( !math::is_zero( value ) )
		return				value;

	return					1.f;
}

void loose_oct_tree::update_bounds	( float3 const& aabb_center, float3 const& aabb_extents )
{
	float3 const center2object = aabb_center - m_aabb_center;
	float3 const difference	= abs(center2object) + aabb_extents;

	float optimal_radius;
	if ( difference.x >= math::max(difference.y, difference.z) )
		optimal_radius		= (difference.x + m_aabb_extents)/2.f;
	else if ( difference.y >= math::max(difference.x, difference.z) )
		optimal_radius		= (difference.y + m_aabb_extents)/2.f;
	else {
		ASSERT				( difference.z > math::max(difference.x, difference.y) );
		optimal_radius		= (difference.z + m_aabb_extents)/2.f;
	}

	ASSERT_CMP				( optimal_radius, >=, m_aabb_extents );

	u32 power				= math::floor( math::log( optimal_radius/m_aabb_extents )/math::log( 2.f ) );
	float new_radius		= m_aabb_extents * math::pow( 2.f, float(power) );
	if ( new_radius < optimal_radius )
		new_radius			= m_aabb_extents * math::pow( 2.f, float(++power) );
	
	ASSERT					( optimal_radius <= new_radius );

	for ( float radius = m_aabb_extents*2.f; radius <= new_radius; radius *= 2.f ) {
		float3 const target2current	= m_aabb_center - aabb_center;
		float3 const signs	= 
			float3(
				make_non_zero( math::sign( target2current.x ) ),
				make_non_zero( math::sign( target2current.y ) ),
				make_non_zero( math::sign( target2current.z ) )
			);
		u32 const octant_index		= collision::octant_index( signs*1.f );
		oct_node* new_root	= m_allocator->allocate( );
		new_root->octants[ octant_index ]	= m_root;
		m_root->parent		= new_root;
		m_root				= new_root;
		m_aabb_center		-= m_aabb_extents*signs;
		m_aabb_extents		= radius;
	}

	R_ASSERT				( math::is_similar( m_aabb_extents, new_radius ) );
}

inline void loose_oct_tree::add		( non_null< oct_node >::ptr node, non_null<object>::ptr object )
{
	object->m_node			= &*node;
	object->m_next			= node->objects;
	node->objects			= &*object;
}

void loose_oct_tree::insert			( non_null<oct_node>::ptr node, non_null<object>::ptr object, float3 const& aabb_center, float const aabb_extents )
{
	if ( aabb_extents <= m_min_aabb_extents ) {
		add					( node, object );
		return;
	}

	float3 const center2object = object->m_aabb.center() - aabb_center;
	float3 const difference = abs(center2object);
	if ( !(difference >= object->m_aabb.extents() ) ) {
		add					( node, object );
		return;
	}

	float3 const signs		= 
		float3(
			math::sign( center2object.x ),
			math::sign( center2object.y ),
			math::sign( center2object.z )
		);
	u32 const octant_index	= collision::octant_index( signs );
	oct_node*& octant		= node->octants[octant_index];
	if ( !octant ) {
		octant				= m_allocator->allocate();
		octant->parent		= &*node;
	}
	
	float const new_radius	= aabb_extents/2.f;
	insert					( octant, object, aabb_center + signs*new_radius, new_radius );
}

inline bool loose_oct_tree::out_of_bounds( float3 const& aabb_center, float3 const& aabb_extents ) const
{
	float3 const difference	= abs(aabb_center - m_aabb_center) + aabb_extents;
	// here we check if some of the component of difference vector is greater
	// than the current radius, therefore it isn't the same as
	// difference > float3(m_aabb_extents, m_aabb_extents, m_aabb_extents)
	return					!(float3(m_aabb_extents, m_aabb_extents, m_aabb_extents) >= difference);
}

void loose_oct_tree::insert_impl	( non_null<object>::ptr const object )
{
	float3 const& aabb_center	= object->m_aabb.center();
	float3 const& aabb_extents	= object->m_aabb.extents();

	if ( m_initialized )
		++m_object_count;
	else {
		initialize			( object, aabb_center, aabb_extents );
		validate			( m_root );
		return;
	}

	if ( out_of_bounds( aabb_center, aabb_extents ) )
		update_bounds		( aabb_center, aabb_extents );

	insert					( m_root, object, m_aabb_center, m_aabb_extents );
}

void loose_oct_tree::insert			( non_null<object>::ptr const object, float4x4 const& local_to_world )
{
	ASSERT					( !object->m_next );
	ASSERT					( !object->m_node );
	object->m_aabb			= object->update_aabb( local_to_world );
	insert_impl				( object );
}

bool loose_oct_tree::try_collapse_bottom_top( non_null< oct_node >::ptr node )
{
	oct_node ** const b		= node->octants;
	oct_node ** i			= b;
	oct_node ** const e		= b + 8;
	u32 octant_id			= u32(-1);
	for ( ; i != e; ++i ) {
		if ( !*i )
			continue;

		if ( octant_id == u32(-1) ) {
			octant_id		= u32(i - b);
			continue;
		}

		return				false;
	}

	ASSERT					( octant_id != u32(-1) );
	if ( node->parent && !try_collapse_bottom_top( node->parent ) )
		return				false;

	if ( node->objects )
		return				false;

	m_aabb_extents			/= 2.f;
	m_aabb_center			+= octant_vector( octant_id )*m_aabb_extents;
	m_allocator->deallocate	( &*node );
	return					true;
}

void loose_oct_tree::try_collapse_top_bottom	( )
{
	oct_node ** const b		= m_root->octants;
	oct_node ** i			= b;
	oct_node ** const e		= b + 8;
	u32 octant_id			= u32(-1);
	for ( ; i != e; ++i ) {
		if ( !*i )
			continue;

		if ( octant_id == u32(-1) ) {
			octant_id		= u32(i - b);
			continue;
		}

		return;
	}

	ASSERT					( octant_id != u32(-1) );

	oct_node* const new_root = m_root->octants[octant_id];
	m_allocator->deallocate	( &*m_root );
	m_root					= new_root;
	new_root->parent		= 0;

	m_aabb_extents			/= 2.f;
	m_aabb_center			+= octant_vector( octant_id )*m_aabb_extents;

	if ( m_root->objects )
		return;

	try_collapse_top_bottom	( );
}

void loose_oct_tree::uninitialize	( )
{
	ASSERT					( !m_object_count );
	m_allocator->deallocate	( m_root );
	m_root					= 0;
#ifdef DEBUG
	m_aabb_center			= float3( math::SNaN, math::SNaN, math::SNaN );
	m_aabb_extents			= math::SNaN;
#endif // #ifdef DEBUG
	m_initialized			= false;
}

void loose_oct_tree::remove_octant	( non_null< oct_node >::ptr node, oct_node* const child )
{
	if ( node )
		;
	else {
		ASSERT				( m_root == &*node );
		uninitialize		( );
		return;
	}

	oct_node** const b		= node->octants;
	oct_node** i			= b;
	oct_node** const e		= b + 8;
	u32 octant_id			= u32(-1);
	u32 count				= 0;
	for ( ; i != e; ++i ) {
		if ( !*i )
			continue;

		if ( *i != child ) {
			++count;
			octant_id		= u32(i - b);
			continue;
		}

		*i					= 0;
		m_allocator->deallocate	( &*child );

		if ( node->objects )
			return;
	}
	ASSERT					( !count || (octant_id != u32(-1)) );

	if ( node->objects )
		return;

	if ( !count ) {
		if ( node->parent )
			remove_octant	( node->parent, &*node );

		return;
	}

	if ( count > 1 )
		return;

	if ( node->parent ) {
		if ( !try_collapse_bottom_top( node->parent ) )
			return;
	}
	else {
		m_aabb_extents		/= 2.f;
		m_aabb_center		+= octant_vector( octant_id )*m_aabb_extents;
	}

	m_root					= node->octants[octant_id];
	m_root->parent			= 0;
	m_allocator->deallocate	( &*node );

	if ( m_root->objects )
		return;

	try_collapse_top_bottom	( );
}

template < typename T>
struct predicate_helper : private boost::noncopyable {
	T m_predicate;

	inline predicate_helper	( T predicate ) :
		m_predicate			( predicate )
	{
	}

	inline ~predicate_helper( )
	{
		m_predicate			( );
	}
}; // struct predicate_helper

template <typename T>
void loose_oct_tree::remove_impl	( non_null<object>::ptr object, T const& predicate )
{
	ASSERT					( object->m_node );

	R_ASSERT				( m_object_count );
	--m_object_count;

	oct_node* node			= object->m_node;
	collision::object* i	= node->objects;
	collision::object* prev	= 0;
	ASSERT					( object );
	for ( ; i != object; ) {
		prev				= i;
		R_ASSERT			( i->m_next );
		i					= i->m_next;
	}

	ASSERT					( i == &*object );
	collision::object* const next = i->m_next;
	object->m_node			= 0;
	object->m_next			= 0;

	{
		predicate_helper<T const& >	const temp( predicate );

		if ( prev ) {
			prev->m_next	= next;
			return;
		}

		node->objects		= next;
		if ( node->objects )
			return;
	}

	remove_octant			( node, 0 );

	if ( node_count( ) != 1 )
		return;

	if ( object_count( ) > 8 )
		return;

#ifndef MASTER_GOLD
	{
		oct_node** const b	= m_root->octants;
		oct_node** i		= b;
		oct_node** const e	= b + 8;
		for ( ; i != e; ++i )
			R_ASSERT		( !*i );
	}
#endif // #ifndef MASTER_GOLD

	i						= m_root->objects;
	m_object_count			= 0;
	uninitialize			( );

	while ( i ) {
		i->m_node			= 0;
		collision::object* const next	= i->m_next;
		i->m_next			= 0;
		insert_impl			( i );
		i					= next;
	}
}


namespace xray {
namespace collision {

struct remove_helper : private boost::noncopyable {
	inline void operator() ( ) const
	{
	}
}; // struct helper

struct move_helper : private boost::noncopyable {
	loose_oct_tree&			m_tree;
	object&					m_object;
	float4x4 const&			m_local_to_world;

	inline		move_helper		(
			loose_oct_tree&			tree,
			non_null<object>::ptr	object,
			float4x4 const&			local_to_world
		) :
		m_tree				( tree ),
		m_object			( *object ),
		m_local_to_world	( local_to_world )
	{
	}
	
	inline void operator ()	( ) const
	{
		m_tree.insert		( &m_object, m_local_to_world );
		ASSERT				( m_object.get_node( ) );
	}
}; // struct helper

} // namespace collision
} // namespace xray

void loose_oct_tree::erase	( non_null<object>::ptr object )
{
	validate				( m_root );
	remove_impl				( object, remove_helper() );
	validate				( m_root );
}

inline void loose_oct_tree::validate		( oct_node* node, u32* object_count ) const
{
#ifdef DEBUG
	if ( !m_initialized )
		return;

	u32 object_count_real	= 0;
	if ( !node->parent ) {
		R_ASSERT			( !object_count );
		object_count		= &object_count_real;
	}

	oct_node** const b		= node->octants;
	oct_node** i			= b;
	oct_node** const e		= b + 8;
	u32 count				= 0;
	for ( ; i != e; ++i ) {
		if ( !*i )
			continue;

		++count;
		validate			( *i, object_count );
	}

	for ( object* i = node->objects; i; i = i->m_next, ++*object_count );

	R_ASSERT				( count || node->objects );

	if ( node->parent )
		return;

	R_ASSERT				( m_object_count == object_count_real, " [%d] -> [%d]", m_object_count, object_count_real );

#else // #ifdef DEBUG
	XRAY_UNREFERENCED_PARAMETERS	( node, object_count );
#endif // #ifdef DEBUG
}

void loose_oct_tree::move	( non_null<collision::object>::ptr object, float4x4 const& new_local_to_world )
{
	R_ASSERT				( object->get_node(), "cannot move object which is not in the tree");
	math::aabb const& new_abb	= object->update_aabb( new_local_to_world );

	if ( !out_of_bounds( new_abb.center(), new_abb.extents() ) ) {
		validate			( m_root );
		remove_impl			( object, move_helper(*this, object, new_local_to_world) );
		validate			( m_root );
	}
	else {
		erase				( object );
		insert				( object, new_local_to_world );
		validate			( m_root );
	}

	ASSERT					( object->m_node );
}

u32 loose_oct_tree::object_count	( ) const
{
	return					m_object_count;
}

u32 loose_oct_tree::node_count		( ) const
{
	return					m_allocator->node_count( );
}

void loose_oct_tree::render_iterate	( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer, non_null< oct_node const>::ptr const node, float3 const& aabb_center, float const aabb_extents )
{
	ASSERT					( m_initialized );

	float const render_aabb_extents = aabb_extents - .01f;
	renderer.draw_aabb		( scene, aabb_center, float3( render_aabb_extents, render_aabb_extents, render_aabb_extents ), math::color( 0u, 255u, 0u, 255u ) );

	float const octant_radius = aabb_extents/2.f;

	oct_node const* const* const b	= node->octants;
	oct_node const* const* i		= b;
	oct_node const* const* const e	= b + 8;
	for ( ; i != e; ++i ) {
		if ( !*i )
			continue;

		render_iterate		( scene, renderer, *i, aabb_center + octant_vector(i - b)*octant_radius, octant_radius );
	}

	object* object			= node->objects;
	while ( object ) {
//		renderer.draw_ellipsoid( math::create_translation( object->m_aabb_center ), object->m_aabb_extents, math::color_xrgb( 255, 0, 0 ) );
		object->render		( scene, renderer );
		object				= object->m_next;
	}
}

void loose_oct_tree::render			( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer )
{
	if ( !m_initialized )
		return;

	render_iterate			( scene, renderer, m_root, m_aabb_center, m_aabb_extents );
}

bool loose_oct_tree::aabb_query		( object_type query_mask, math::aabb const& query_aabb, objects_type& objects )
{
	if ( !m_initialized )
		return				false;

	return					colliders::aabb_object( query_mask, *this, query_aabb, objects ).result( );
}

bool loose_oct_tree::aabb_query		( object_type query_mask, math::aabb const& query_aabb, triangles_type& triangles )
{
	if ( !m_initialized )
		return				false;

	return					colliders::aabb_object( query_mask, *this, query_aabb, triangles ).result( );
}

bool loose_oct_tree::cuboid_query	( object_type query_mask, math::cuboid const& cuboid, objects_type& objects )
{	
	if ( !m_initialized )
		return				false;

	return					colliders::cuboid_object( query_mask, *this, cuboid, objects ).result( );
}

bool loose_oct_tree::cuboid_query	( object_type query_mask, math::cuboid const& cuboid, triangles_type& triangles )
{	
	if ( !m_initialized )
		return				false;

	return					colliders::cuboid_object( query_mask, *this, cuboid, triangles ).result( );
}

bool loose_oct_tree::cuboid_query	( object_type query_mask, math::cuboid const& cuboid, results_callback_type& callback )
{
	if ( !m_initialized )
		return				false;

	return					colliders::cuboid_object( query_mask, *this, cuboid, callback ).result();
}

bool loose_oct_tree::ray_query		(
		object_type const query_mask,
		float3 const& origin,
		float3 const& direction,
		float max_distance,
		ray_objects_type& objects,
		objects_predicate_type const& predicate
	)
{
	if ( !m_initialized )
		return				false;

	return					colliders::ray_object( query_mask, *this, origin, direction, max_distance, objects, predicate ).result( );
}

bool loose_oct_tree::ray_query		(
		object_type const query_mask,
		float3 const& origin,
		float3 const& direction,
		float max_distance,
		ray_triangles_type& triangles,
		triangles_predicate_type const& predicate
	)
{
	if ( !m_initialized )
		return				false;

	return					colliders::ray_object( query_mask, *this, origin, direction, max_distance, triangles, predicate ).result( );
}

bool loose_oct_tree::aabb_test		( math::aabb const& aabb, triangles_predicate_type const& predicate ) 
{
	XRAY_UNREFERENCED_PARAMETERS	(&aabb, &predicate);
	return true; 
}

bool loose_oct_tree::cuboid_test	( math::cuboid const& cuboid, triangles_predicate_type const& predicate ) 
{
	XRAY_UNREFERENCED_PARAMETERS	(&cuboid, &predicate);
	return true; 
}

bool loose_oct_tree::ray_test		( math::float3 const& origin, math::float3 const& direction, float max_distance, triangles_predicate_type const& predicate ) 
{
	XRAY_UNREFERENCED_PARAMETERS	(&origin, &direction, max_distance, &predicate);
	return true; 
}

memory::base_allocator& loose_oct_tree::get_allocator( ) const
{
	return							m_allocator->get_allocator();
}

void loose_oct_tree::for_each		( predicate_type const& predicate ) const
{
	if ( !m_root )
		return;

	for_each_iterate				( predicate, root(), m_aabb_center, m_aabb_extents );
}