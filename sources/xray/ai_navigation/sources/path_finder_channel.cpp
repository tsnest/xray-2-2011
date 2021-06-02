////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "path_finder_channel.h"
#include "path_finder_modified_funnel.h"
#include "navigation_mesh_functions.h"
#include <xray/ai/search/vertex_allocator_fixed_count.h>
#include <xray/ai/search/priority_queue_binary_heap.h>
#include <xray/ai/search/path_constructor_vertex.h>
#include <xray/ai/search/a_star.h>
#include <xray/ai/search/search_restrictor_generic.h>


////////////////////////////////////////////////////////////////////////////
// this is because GCC does compile template classes functions without any
// instantiation :(
////////////////////////////////////////////////////////////////////////////
typedef u32	vertex_id_type;
typedef u32	edge_id_type;
typedef float distance_type;

namespace xray {
namespace ai {
namespace vertex_manager {

inline	u32	hash_value	( vertex_id_type const vertex_id )
{
	return					vertex_id;
}

} // namespace vertex_manager
} // namespace ai
} // namespace xray

#include <xray/ai/search/vertex_manager_fixed_count_hash.h>
////////////////////////////////////////////////////////////////////////////
// end of GCC specific
////////////////////////////////////////////////////////////////////////////

using xray::ai::navigation::path_finder_channel;
using xray::ai::navigation::path_finder_funnel;
using xray::ai::navigation::path_finder_modified_funnel;
using xray::ai::navigation::triangles_mesh_type;
using xray::ai::navigation::distance_to_segment;
using xray::math::float3;

class triangles_mesh_graph_wrapper : private boost::noncopyable {
public:
	struct vertex_impl {};
	struct look_up_cell_impl {};
	typedef edge_id_type const* const_edge_iterator;

public:
	inline			triangles_mesh_graph_wrapper( triangles_mesh_type const& graph, u32 const start_vertex_id  ) :
		m_graph				( graph ),
		m_start_vertex_id	( start_vertex_id )
	{
	}

	inline	void	on_before_search			( )
	{
	}

	inline	void	on_after_search				( bool const success )
	{
		XRAY_UNREFERENCED_PARAMETER	( success );
	}

	template < typename VertexType >
	inline	void	edge_iterators				( VertexType const& vertex, const_edge_iterator& begin, const_edge_iterator& end )
	{
		u32 const vertex_id	= ( vertex.id() == vertex_id_type(-1) ) ? m_start_vertex_id : vertex.id();
		m_current_vertex_enter_edge_id	= vertex.enter_edge_id;
		begin				= &m_graph.data[ vertex_id ].neighbours[0];
		end					= begin + 3;
	}

	inline	vertex_id_type	vertex_id			( vertex_id_type vertex_id, const_edge_iterator const iterator ) const
	{
		vertex_id			= (vertex_id == vertex_id_type(-1)) ? m_start_vertex_id : vertex_id;
		if ( *iterator == m_start_vertex_id )
			return			m_start_vertex_id;

		u32 const exit_edge_id	= edge_id(vertex_id, iterator);
		return				exit_edge_id == m_current_vertex_enter_edge_id ? u32( -1 ) : *iterator;
	}

	inline	edge_id_type	edge_id				( vertex_id_type vertex_id, const_edge_iterator const iterator ) const
	{
		vertex_id			= (vertex_id == vertex_id_type(-1)) ? m_start_vertex_id : vertex_id;
		return				iterator - m_graph.data[ vertex_id ].neighbours;
	}

private:
	triangles_mesh_type const&	m_graph;
	u32							m_current_vertex_enter_edge_id;
	u32	const					m_start_vertex_id;
}; // class triangles_mesh_graph_wrapper

static u32 get_closest_edge ( triangles_mesh_type const& graph, u32 const triangle_id, float3 const vertex )
{
	u32 const* indices = graph.indices.begin() + triangle_id * 3;
	u32 closest_id = 0;
	float closest_distance = distance_to_segment( vertex, graph.vertices[ indices[0] ], graph.vertices[ indices[1] ] );
	for ( u32 i = 1; i < 3; ++i ) {
		float distance = distance_to_segment( vertex, graph.vertices[ indices[i] ], graph.vertices[ indices[(i+1)%3] ]  );
		if ( distance < closest_distance ) {
			closest_id = i;
			closest_distance = distance;
		}

	}
	return closest_id;
}

static inline u32 get_third_edge ( u32 const id0, u32 const id1 ) 
{
	R_ASSERT_CMP( id0, !=, id1 );

	switch ( id0 + id1 ) {
		case 1: return 2;
		case 2: return 1;
		case 3: return 0;
		default: NODEFAULT( return u32(-1) );
	}
}

class triangles_mesh_search_restrictor :
	public xray::ai::search_restrictor::generic <
		vertex_id_type,
		triangles_mesh_graph_wrapper::const_edge_iterator,
		distance_type
	>
{
private:
	typedef xray::ai::search_restrictor::generic <
		vertex_id_type,
		triangles_mesh_graph_wrapper::const_edge_iterator,
		distance_type
	> super;

public:
	inline			triangles_mesh_search_restrictor	(
						triangles_mesh_type const& graph,
						float const agent_radius,
						float3 const& start_position,
						float3 const& target_position,
						vertex_id_type const& start_vertex_id,
						vertex_id_type const& real_start_vertex_id,
						vertex_id_type const& target_vertex_id,
						distance_type const& max_range = std::numeric_limits< distance_type >::max(),
						u32 const max_iteration_count = u32(-1),
						u32 const max_visited_vertex_count = u32(-1)
					) :
		super	(
			start_vertex_id,
			target_vertex_id,
			max_range,
			max_iteration_count,
			max_visited_vertex_count
		),
		m_graph					( graph ),
		m_start_position		( start_position ),
		m_target_position		( target_position ),
		m_start_vertex_id		( start_vertex_id ),
		m_real_start_vertex_id	( real_start_vertex_id ),
		m_target_vertex_id		( target_vertex_id ),
		m_agent_radius			( agent_radius )

	{
	}

	bool is_traversable ( u32 const triangle_id, u32 const edge_id0, u32 const edge_id1, u32 const neighbour_triangle_id ) const
	{
		if ( neighbour_triangle_id == m_target_vertex_id ) {
			u32 const closest_edge = get_closest_edge ( m_graph, neighbour_triangle_id, m_target_position );
			if ( m_graph.data[ neighbour_triangle_id ].neighbours[ closest_edge ] != triangle_id ) {
				u32 const similar_edge = get_similar_edge ( m_graph, neighbour_triangle_id, triangle_id );
				u32 const opposite_edge = get_third_edge( similar_edge, closest_edge );
				float3 const vertex = m_graph.vertices[ m_graph.indices[ neighbour_triangle_id * 3 + ((opposite_edge + 2) % 3) ] ];
				if ( m_agent_radius > (calculate_triangle_width( m_graph, vertex, neighbour_triangle_id, opposite_edge ) * 0.5) )
					return false;
			}
		}

		if ( triangle_id == m_real_start_vertex_id ) {
			u32 const closest_edge = get_closest_edge ( m_graph, triangle_id, m_start_position );
			if ( closest_edge == edge_id1 )
				return true;

			u32 const opposite_edge = get_third_edge( edge_id1, closest_edge );
			float3 const vertex = m_graph.vertices[ m_graph.indices[ triangle_id * 3 + ((opposite_edge + 2) % 3) ] ];
			return m_agent_radius <= (calculate_triangle_width( m_graph, vertex, triangle_id, opposite_edge ) * 0.5);
		}

		u32 opposite_edge = 0;
		switch ( edge_id0 + edge_id1 ) {
			case 1: opposite_edge = 2; break;
			case 2: opposite_edge = 1; break;
			case 3: opposite_edge = 0; break;
			default: NODEFAULT();
		}

		float3 const& vertex = m_graph.vertices[ m_graph.indices[ triangle_id * 3 + (opposite_edge + 2) % 3 ] ];
		float triangle_width = calculate_triangle_width( m_graph, vertex, triangle_id, opposite_edge ) * .5f;
		if ( triangle_width < m_agent_radius )
			return false;
		return true;
	}

	template < typename VertexType >
	inline	bool	accessible	( vertex_id_type neighbour_vertex_id, VertexType const& current_vertex, triangles_mesh_graph_wrapper::const_edge_iterator const& edge_iterator ) const
	{
		u32 const current_vertex_id = ( current_vertex.id() == vertex_id_type(-1) ) ? m_real_start_vertex_id : current_vertex.id();
		if ( neighbour_vertex_id == u32(-1) )
			return							false;

		xray::ai::navigation::navigation_triangle const& triangle = m_graph.data[ current_vertex_id ];
		u32 const exit_edge_id				= u32(edge_iterator - &triangle.neighbours[0]);
		if ( triangle.obstructions[ exit_edge_id ] != 0.f )
			return							false;

		if ( current_vertex.enter_edge_id == exit_edge_id ) {
			R_ASSERT_CMP					( neighbour_vertex_id, ==, m_real_start_vertex_id );
			return							false;
		}
	
		return								is_traversable( current_vertex_id, current_vertex.enter_edge_id, exit_edge_id, neighbour_vertex_id );
	}

	inline	u32		get_start_vertices_count( ) const
	{
		return								1;
	}

private:
	triangles_mesh_type const&	m_graph;
	float3	const&				m_start_position;
	float3	const&				m_target_position;
	u32		const				m_start_vertex_id;
	u32		const				m_real_start_vertex_id;
	u32		const				m_target_vertex_id;
	float	const				m_agent_radius;
}; // struct triangles_mesh_search_restrictor

class euclidian_distance_heuristics;

struct triangle_path_constructor : public xray::ai::path_constructor::vertex {
	typedef xray::ai::path_constructor::vertex	super;

	template < typename T >
	struct vertex_impl : public super::vertex_impl<T> {
		u8 enter_edge_id;
	}; // struct vertex

	template < typename VertexType, typename PathType >
	class impl :
		public super::impl< VertexType, PathType >,
		private boost::noncopyable
	{
	public:
		typedef xray::ai::path_constructor::vertex::impl<VertexType, PathType>	super;

		inline				impl				( triangles_mesh_type const& graph, vertex_id_type const start_vertex_id, PathType* path = 0 ) :
			super	( path ),
			m_graph	( graph ),
			m_start_vertex_id ( start_vertex_id )
		{
		}

		inline void			assign_parent		( VertexType& neighbour, VertexType* parent )
		{
			super::assign_parent( neighbour, parent );
			neighbour.enter_edge_id	= u8(-1);
		}

		inline u8			get_edge_id			( vertex_id_type const& neighbour_vertex_id, vertex_id_type const& predecessor_vertex_id ) const
		{
			u32 const* const b		= m_graph.data[ neighbour_vertex_id ].neighbours;
			u32 const* const e		= b + 3;
			u32 const* i			= b;
			for ( ; i != e; ++i ) {
				if ( *i == predecessor_vertex_id )
					break;
			}
			R_ASSERT				( i != e );
			return					u8(i - b);
		}

		template < typename EdgeIdType >
		inline void			assign_parent		( VertexType& neighbour, VertexType* parent, EdgeIdType const& parent_edge_id )
		{
			u32 const parent_id = parent->id() == vertex_id_type(-1) ? m_start_vertex_id : parent->id();
			super::assign_parent	( neighbour, parent, parent_edge_id );
			neighbour.enter_edge_id	= !parent ? u8(-1) : get_edge_id( neighbour.id(), parent_id );
		}

		triangles_mesh_type const&	m_graph;
		vertex_id_type const	m_start_vertex_id;
	};
}; // struct triangle_path_constructor

class search_service {
public:
	typedef distance_type									distance_type;
	typedef vertex_id_type									vertex_id_type;
	typedef edge_id_type									edge_id_type;
	typedef xray::ai::vertex_allocator::fixed_count			vertex_allocator_type;
	typedef xray::ai::vertex_manager::fixed_count_hash		vertex_manager_type;
	typedef xray::ai::priority_queue::binary_heap			priority_queue_type;
	typedef triangles_mesh_graph_wrapper					graph_wrapper_type;
	typedef triangle_path_constructor						path_constructor_helper_type;
	typedef xray::ai::a_star								algorithm_type;

	struct vertex_type : 
		public algorithm_type::vertex_helper< distance_type >::vertex_impl,
		public graph_wrapper_type::vertex_impl,
		public path_constructor_helper_type::vertex_impl< vertex_type >,
		public priority_queue_type::vertex_impl,
		public vertex_manager_type::vertex_helper< vertex_id_type >::vertex_impl,
		public vertex_allocator_type::vertex_impl
	{};

	struct look_up_cell_type :
		public algorithm_type::look_up_cell_impl,
		public graph_wrapper_type::look_up_cell_impl,
		public path_constructor_helper_type::look_up_cell_impl,
		public priority_queue_type::look_up_cell_impl,
		public vertex_manager_type::look_up_cell_helper< vertex_type >::look_up_cell_impl< look_up_cell_type >,
		public vertex_allocator_type::look_up_cell_impl
	{};

	struct vertex_allocator_impl_type : public vertex_allocator_type::impl< vertex_type >
	{
		typedef vertex_allocator_type::impl< vertex_type >	super;
		inline vertex_allocator_impl_type	( u32 const max_vertex_count ) :
			super							( max_vertex_count )
		{
		}
	};

	struct vertex_manager_impl_type : public 
		vertex_manager_type::impl<
			vertex_allocator_impl_type,
			look_up_cell_type
		>
	{
		typedef vertex_manager_type::impl< vertex_allocator_impl_type, look_up_cell_type >	super;
		inline vertex_manager_impl_type		(
			vertex_allocator_impl_type& allocator,
			u32 const hash_size,
			u32 const fix_size
		) : super							( allocator, hash_size, fix_size )
		{
		}
	};

	struct priority_queue_impl_type : public priority_queue_type::impl< vertex_manager_impl_type >
	{
		typedef priority_queue_type::impl< vertex_manager_impl_type >	super;
		inline priority_queue_impl_type		( vertex_manager_type& vertex_manager, u32 const vertex_count ) :
			super							( vertex_manager, vertex_count )
		{
		}
	};

	typedef xray::debug::vector< vertex_id_type >				path_type;
	typedef euclidian_distance_heuristics						path_heuristics_type;
	typedef path_constructor_helper_type::impl<
				vertex_type,
				path_type
			> path_constructor_type;
	typedef triangles_mesh_search_restrictor					search_restrictor_type;

private:
	enum {
		max_branching_factor	= 3,
		max_vertex_count		= 4*1024,
		hash_size				= 256,
	};

public:
	inline	search_service		( ) :
		m_vertex_allocator	( max_vertex_count ),
		m_vertex_manager	( m_vertex_allocator, hash_size, max_vertex_count ),
		m_priority_queue	( m_vertex_manager, max_vertex_count )
	{
	}

	bool	search				(
			triangles_mesh_type const& graph,
			path_type* path,
			vertex_id_type const start_vertex_id,
			vertex_id_type const target_vertex_id,
			float3 const& start_position,
			float3 const& target_position,
			float const agent_radius
		);

public:
	vertex_allocator_impl_type	m_vertex_allocator;
	vertex_manager_impl_type	m_vertex_manager;
	priority_queue_impl_type	m_priority_queue;
}; // class search_service

// according to this paper
// http://skatgame.net/mburo/ps/thesis_demyen_2006.pdf
// page 67
class euclidian_distance_heuristics : private boost::noncopyable {
public:
	inline	euclidian_distance_heuristics		(
			search_service::path_constructor_type& path_constructor,
			triangles_mesh_type const& graph,
			vertex_id_type const start_vertex_id,
			vertex_id_type const target_vertex_id,
			float3 const& start_position,
			float3 const& target_position,
			float const agent_radius
		) :
		m_path_constructor	( path_constructor ),
		m_graph				( graph ),
		m_start_vertex_id	( start_vertex_id ),
		m_target_vertex_id	( target_vertex_id ),
		m_start_position	( start_position ),
		m_target_position	( target_position ),
		m_agent_radius		( agent_radius )
	{
		XRAY_UNREFERENCED_PARAMETER	( target_vertex_id );
	}

	inline	void	on_before_search			( )
	{
	}

	inline	void	on_after_search				( bool const success )
	{
		XRAY_UNREFERENCED_PARAMETER	( success );
	}

	inline	void	on_start_iteration			( vertex_id_type const vertex_id )
	{
		XRAY_UNREFERENCED_PARAMETER	( vertex_id );
	}

	inline	float	estimate					( vertex_id_type const* const current_vertex_id_ptr, vertex_id_type neighbour_vertex_id ) const
	{
		if ( !current_vertex_id_ptr )
			return						length( m_start_position - m_target_position );

		if ( *current_vertex_id_ptr == m_target_vertex_id )
			return						0.f;

		u32 const current_vertex_id		= ( *current_vertex_id_ptr == vertex_id_type(-1) ) ? m_start_vertex_id : *current_vertex_id_ptr;
		neighbour_vertex_id				= ( neighbour_vertex_id == vertex_id_type(-1) ) ? m_start_vertex_id : neighbour_vertex_id;

		u32 const* const current_indices = m_graph.indices.begin() + current_vertex_id * 3;

		u32 const edge_id				= get_similar_edge ( m_graph, current_vertex_id, neighbour_vertex_id );

		float3 const& v0				= m_graph.vertices[ current_indices[ edge_id ] ];
		float3 const& v1				= m_graph.vertices[ current_indices[ (edge_id + 1) % 3 ] ];
		return							distance_to_segment( m_target_position, v0, v1 );
	}

	inline	float	get_angle					(
			vertex_id_type const vertex_id,
			vertex_id_type const enter_edge_id,
			edge_id_type const exit_edge_id
		) const
	{
		R_ASSERT					( enter_edge_id != exit_edge_id );

		float3 const* const vertices = m_graph.vertices.begin();
		u32 const* const indices	= m_graph.indices.begin() + vertex_id * 3;
		float3 const& u0			= vertices[ indices[ enter_edge_id ] ];
		float3 const& u1			= vertices[ indices[ (enter_edge_id + 1) % 3 ] ];
		float3 const& v0			= vertices[ indices[ exit_edge_id ] ];
		float3 const& v1			= vertices[ indices[ (exit_edge_id + 1) % 3 ] ];

		R_ASSERT					( (&v1 == &u0) || (&v0 == &u1) );
		float const cosine			=
			&v1 == &u0 ?
				(normalize( v0 - v1 ) | normalize( u1 - v1 )) :
				(normalize( v1 - v0 ) | normalize( u0 - v0 ));

		return						acos( xray::math::clamp_r( cosine, -1.0f, 1.0f ) );
	}
	
	inline	float	evaluate					(
			search_service::vertex_type& current_vertex,
			search_service::vertex_type& neighbour_vertex,
			triangles_mesh_graph_wrapper::const_edge_iterator const iterator
		) const
	{
		XRAY_UNREFERENCED_PARAMETER		( iterator );

#define CHECK_HEURISTICS	0
#if !CHECK_HEURISTICS
		if ( neighbour_vertex.id() == m_target_vertex_id ) {
			search_service::vertex_type* neighbour_vertex_back =  neighbour_vertex.back( );
			neighbour_vertex.back( )	= &current_vertex;
			m_path_constructor.construct_path( neighbour_vertex );
			neighbour_vertex.back( )	= neighbour_vertex_back;
				
			typedef path_finder_modified_funnel::path_type path_type;
			typedef path_finder_modified_funnel::channel_type channel_type;
			channel_type& channel = m_path_constructor.path();
			channel[0] = m_start_vertex_id;

			path_type path;
			path_finder_modified_funnel			( m_graph, channel, m_start_position, m_target_position, m_agent_radius, path );
			R_ASSERT					( !path.empty() );
			float distance				= 0;
			path_type::iterator i		= path.begin();
			path_type::iterator const e = path.end() - 1;
			for ( ; i != e; ++i )
				distance				+= length ( (*(i+1)) - (*i) );

			return						distance;
		}
#endif // #if !CHECK_HEURISTICS

		u32 const current_vertex_id = current_vertex.id() == vertex_id_type(-1) ? m_start_vertex_id : current_vertex.id();

		u32 const similar_edge			= u32(iterator - m_graph.data[current_vertex_id].neighbours);
		R_ASSERT_CMP					( similar_edge, <, 3 );
		u32 const* const indices		= m_graph.indices.begin() + current_vertex_id * 3;
		float3 const& v0				= m_graph.vertices[ indices[ similar_edge ] ];
		float3 const& v1				= m_graph.vertices[ indices[ (similar_edge+1) % 3 ] ];
		float const heuristics0			= distance_to_segment( m_start_position, v0, v1 );
		float const heuristics1			= 
			(m_start_vertex_id == current_vertex_id) ? 0.f :
			(m_target_vertex_id == neighbour_vertex.id()) ? distance_to_segment( m_target_position, v0, v1 ) :
			(current_vertex.g() + m_agent_radius * get_angle( current_vertex.id(), current_vertex.enter_edge_id, iterator - m_graph.data[ current_vertex_id ].neighbours ));
		float const heuristics2			= current_vertex.g() + neighbour_vertex.h() - current_vertex.h();
		float const result				=
			xray::math::max(
				xray::math::max(
					heuristics0,
					heuristics1
				),
				heuristics2
			);

#if CHECK_HEURISTICS
		if ( neighbour_vertex.id() == m_target_vertex_id ) {
			search_service::vertex_type* neighbour_vertex_back =  neighbour_vertex.back( );
			neighbour_vertex.back( )	= &current_vertex;
			m_path_constructor.construct_path( neighbour_vertex );
			neighbour_vertex.back( )	= neighbour_vertex_back;
				
			typedef path_finder_modified_funnel::path_type path_type;
			typedef path_finder_modified_funnel::channel_type channel_type;
			channel_type& channel = m_path_constructor.path();
			channel[0] = m_start_vertex_id;

			path_type path;
			path_finder_modified_funnel			( m_graph, channel, m_start_position, m_target_position, m_agent_radius, path );
			R_ASSERT					( !path.empty() );
			float distance				= 0;
			path_type::iterator i		= path.begin();
			path_type::iterator const e = path.end() - 1;
			for ( ; i != e; ++i )
				distance				+= length ( (*(i+1)) - (*i) );
			R_ASSERT_CMP				( distance, >=, result );
			return						distance;
		}
#endif // #if CHECK_HEURISTICS
#undef CHECK_HEURISTICS
		return							result;
	}

	static inline bool metric_euclidian			( )
	{
		return							true;
	}

private:
	search_service::path_constructor_type&	m_path_constructor;
	triangles_mesh_type const&				m_graph;
	float3 const&							m_start_position;
	float3 const&							m_target_position;
	u32 const								m_start_vertex_id;
	u32 const								m_target_vertex_id;
	float const								m_agent_radius;
}; // class euclidian_distance_heuristics

bool search_service::search		(
		triangles_mesh_type const& graph,
		path_type* path,
		vertex_id_type const start_vertex_id,
		vertex_id_type const target_vertex_id,
		float3 const& start_position,
		float3 const& target_position,
		float const agent_radius
	)
{
	u32 fake_start_vertex_id = start_vertex_id;

	if ( start_vertex_id == target_vertex_id ) {
		u32 const closest_edge0 = get_closest_edge( graph, start_vertex_id, start_position );
		u32 const closest_edge1 = get_closest_edge( graph, target_vertex_id, target_position );
		if ( closest_edge0 == closest_edge1 ) {
			if ( path ) {
				path->resize( 1 );
				(*path)[0] = start_vertex_id;
				return true;
			}
		}

		u32 const* indices = graph.indices.begin() + start_vertex_id * 3;
		u32 const opposite_edge = get_third_edge ( closest_edge0, closest_edge1 );
		float3 const& vertex = graph.vertices[ indices[ (opposite_edge + 2) % 3 ] ];
		if ( calculate_triangle_width( graph, vertex, start_vertex_id, opposite_edge ) * 0.5f < agent_radius )
			fake_start_vertex_id = u32(-1);
		else {
			if ( path ) {
				path->resize( 1 );
				(*path)[0] = start_vertex_id;
				return true;
			}
		}
	}

	graph_wrapper_type graph_wrapper			= graph_wrapper_type( graph, start_vertex_id );
	path_constructor_type path_constructor		= path_constructor_type( graph, start_vertex_id, path );
	path_heuristics_type path_heuristics		= path_heuristics_type( path_constructor, graph, start_vertex_id, target_vertex_id, start_position, target_position, agent_radius );
	search_restrictor_type search_restrictor	= search_restrictor_type(
		graph,
		agent_radius,
		start_position,
		target_position,
		fake_start_vertex_id,
		start_vertex_id,
		target_vertex_id,
		std::numeric_limits< distance_type >::max( ),
		u32(-1),
		max_vertex_count - max_branching_factor
	);

	return
		algorithm_type::find		(
			m_priority_queue,
			graph_wrapper,
			path_constructor,
			path_heuristics,
			search_restrictor
		);
}

path_finder_channel::path_finder_channel (
		triangles_mesh_type const& triangles_mesh,
		path_finder_funnel::channel_type& channel,
		u32 const start_vertex_id,
		u32 const target_vertex_id,
		float3 const& start_position,
		float3 const& target_position,
		float const agent_radius
	) :
	m_triangles_mesh	( triangles_mesh )	
{
	search_service service;
	search_service::path_type path;
	service.search		(
		triangles_mesh,
		&channel,
		start_vertex_id,
		target_vertex_id,
		start_position,
		target_position,
		agent_radius
	);
	if ( !channel.empty() )
		channel[0] = start_vertex_id;
}