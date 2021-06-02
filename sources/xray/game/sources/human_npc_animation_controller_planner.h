////////////////////////////////////////////////////////////////////////////
//	Created		: 09.11.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef HUMAN_NPC_ANIMATION_CONTROLLER_PLANNER_H_INCLUDED
#define HUMAN_NPC_ANIMATION_CONTROLLER_PLANNER_H_INCLUDED

#include <xray/ai/search/vertex_allocator_fixed_count.h>
#include <xray/ai/search/vertex_manager_fixed_count_hash.h>
#include <xray/ai/search/priority_queue_binary_heap.h>
#include <xray/ai/search/path_constructor_edge.h>
#include <xray/ai/search/a_star.h>
#include <xray/ai/search/search_restrictor_generic.h>
#include <xray/animation/cubic_spline_skeleton_animation.h>
#include <boost/crc.hpp>
#include <xray/animation/animation_player.h>
#include <xray/ai_navigation/world.h>

namespace stalker2 {

#ifndef MASTER_GOLD

struct animation_space_vertex_id {
	xray::math::quaternion	rotation;
	xray::math::float3		translation;
}; // struct animation_space_vertex_id

inline bool operator ==	( animation_space_vertex_id const& left, animation_space_vertex_id const& right )
{
	return		(left.rotation.vector == right.rotation.vector) && (left.translation.is_similar( right.translation, .3f ) );
}

inline bool operator !=	( animation_space_vertex_id const& left, animation_space_vertex_id const& right )
{
	return		!(left == right);
}

typedef animation_space_vertex_id	vertex_id_type;
typedef u32							edge_id_type;
typedef float						distance_type;

struct animation_space_edge {
	animation_space_vertex_id	movement;
	u32 						first_animation_index;
	u32 						second_animation_index;
	float						first_animation_weight;
	// float second_animation_weight = 1.f - first_animation_weight;

	inline	animation_space_edge	(
			u32 const first_animation_index,
			u32 const second_animation_index,
			float const first_animation_weight,
			animation_space_vertex_id const& movement
		) :
		movement				( movement ),
		first_animation_index	( first_animation_index ),
		second_animation_index	( second_animation_index ),
		first_animation_weight	( first_animation_weight )
	{
	}
}; // struct animation_space_edge

animation_space_vertex_id get_movement	(
		xray::animation::animation_player& player,
		xray::animation::skeleton_animation_ptr const& left,
		pcstr const left_identifier,
		xray::animation::skeleton_animation_ptr const& right,
		pcstr const right_identifier,
		float const left_weight
	);

class animation_space_graph : private boost::noncopyable {
public:
	typedef std::pair< xray::animation::skeleton_animation_ptr, pcstr >		animation_pair_type;
	typedef xray::debug::vector< animation_space_edge >						edges_type;
	typedef xray::debug::vector< animation_pair_type >						animations_type;

public:
	template < int AnimationsCount >
	inline			animation_space_graph	(
			xray::ai::navigation::world& navigation_world,
			float const agent_radius,
			xray::animation::skeleton_animation_ptr const& forward_animation,
			animation_pair_type const (&animations)[ AnimationsCount ],
			u32 const intervals_count
		) :
		m_navigation_world		( navigation_world ),
		m_agent_radius			( agent_radius )
	{
		m_animations.reserve	( AnimationsCount + 1 );
		m_animations.push_back	( std::make_pair( forward_animation, "forward" ) );
		for ( u32 i = 0; i < AnimationsCount; ++i )
			m_animations.push_back	( animations[i] );

		xray::animation::animation_player player;

		m_edges.reserve			( AnimationsCount*intervals_count + 1 );

		m_step_time				= xray::animation::cubic_spline_skeleton_animation_pinned( forward_animation )->length_in_frames() / xray::animation::default_fps;

		m_edges.push_back		( animation_space_edge( 0, 1, 1.f, get_movement( player, forward_animation, "forward", animations[1].first, animations[1].second, 1.f ) ) );
		float const interval_size	= 1.f / float(intervals_count);
		for ( u32 i = 1; i <= intervals_count; ++i ) {
			for ( animations_type::const_iterator b = m_animations.begin(), j = b + 1, e = m_animations.end(); j != e; ++j ) {
				m_edges.push_back(
					animation_space_edge(
						0,
						u32(j - b),
						i*interval_size,
						get_movement( player, forward_animation, "forward", (*j).first, (*j).second, i*interval_size )
					)
				);
			}
		}

		m_max_speed				= 0.f;
		edges_type::const_iterator i		= m_edges.begin( );
		edges_type::const_iterator const e	= m_edges.end( );
		for ( ; i != e; ++i )
			m_max_speed			= math::max( m_max_speed, (*i).movement.translation.length() );
	}

	inline bool					is_passable	( animation_space_vertex_id const& from, animation_space_vertex_id const& to ) const
	{
		//XRAY_UNREFERENCED_PARAMETER	( from );
		//u32 const triangle_id	= m_navigation_world.get_node_id_at( to.translation );
		//if ( triangle_id == u32(-1) )
		//	return				false;
		//return					m_navigation_world.can_stand( to.translation, triangle_id, m_agent_radius );
		XRAY_UNREFERENCED_PARAMETERS( &from, &to );
		return					true;
	}

	inline edges_type const&	edges		( ) const
	{
		return					m_edges;
	}

	inline animations_type const& animations( ) const
	{
		return					m_animations;
	}

	inline float				step_time	( ) const
	{
		return					m_step_time;
	}

	inline float				max_speed	( ) const
	{
		return					m_max_speed;
	}

private:
#ifndef MASTER_GOLD
	animations_type				m_animations;
	edges_type					m_edges;
#endif // #ifndef MASTER_GOLD
	xray::ai::navigation::world& m_navigation_world;
	float const					m_agent_radius;
	float						m_step_time;
	float						m_max_speed;
}; // struct animation_space_graph

struct animation_space_graph_wrapper : private boost::noncopyable {
private:
	typedef animation_space_graph::edges_type	edges_type;

public:
	struct vertex_impl {};
	struct look_up_cell_impl {};
	typedef edges_type::const_iterator	const_edge_iterator;

public:
	inline			animation_space_graph_wrapper(
			animation_space_graph const& graph,
			vertex_id_type const& start_vertex_id
		) :
		m_graph					( graph ),
		m_start_vertex_id		( start_vertex_id )
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
		XRAY_UNREFERENCED_PARAMETER	( vertex );
		begin				= m_graph.edges().begin();
		end					= m_graph.edges().end();
	}

	inline	vertex_id_type	vertex_id			( vertex_id_type const& vertex_id, const_edge_iterator const iterator ) const
	{
		math::float4x4 temp	= math::create_rotation( vertex_id.rotation );
		temp.c.xyz()		= vertex_id.translation;

		vertex_id_type		result;
		result.translation	= temp.transform_position( (*iterator).movement.translation );
		result.rotation		= (*iterator).movement.rotation * vertex_id.rotation;
		return				result;
	}

	inline	edge_id_type	edge_id				( vertex_id_type const& vertex_id, const_edge_iterator const iterator ) const
	{
		XRAY_UNREFERENCED_PARAMETER	( vertex_id );
		return				edge_id_type( iterator - m_graph.edges().begin() );
	}

private:
	animation_space_graph const&	m_graph;
	vertex_id_type const&			m_start_vertex_id;
}; // struct animation_space_graph_wrapper

class animation_space_heuristics : private boost::noncopyable {
public:
	inline			animation_space_heuristics	(
			animation_space_graph const& graph,
			vertex_id_type const& target_vertex_id,
			float const step_time,
			float const max_speed
		) :
		m_graph						( graph ),
		m_target_vertex_id			( target_vertex_id ),
		m_step_time					( step_time ),
		m_max_speed					( max_speed ),
		m_min_heuristics_value		( math::infinity )
	{
		m_best_vertex_id.rotation	= math::quaternion( math::float3(0.f, 0.f, 1.f), 0.f );
		m_best_vertex_id.translation= math::float3(0.f, 0.f, 0.f);
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

	inline	float	estimate					( vertex_id_type const* const current_vertex_id_ptr, vertex_id_type const& neighbour_vertex_id ) const
	{
		XRAY_UNREFERENCED_PARAMETER	( current_vertex_id_ptr );
		float const result			= length( neighbour_vertex_id.translation - m_target_vertex_id.translation ) / m_max_speed;
		if ( result < m_min_heuristics_value ) {
			m_min_heuristics_value	= result;
			m_best_vertex_id		= neighbour_vertex_id;
		}
		return						result;
	}

	template < typename VertexType >
	inline	float	evaluate					(
			VertexType const& current_vertex,
			VertexType const& neighbour_vertex,
			animation_space_graph_wrapper::const_edge_iterator const& iterator
		) const
	{
		XRAY_UNREFERENCED_PARAMETERS( &current_vertex, &neighbour_vertex, &iterator );
		return						m_step_time;
	}

	static inline bool metric_euclidian			( )
	{
		return						true;
	}

	inline	vertex_id_type const&	best_vertex_id	( ) const
	{
		return						m_best_vertex_id;
	}

private:
	animation_space_graph const&	m_graph;
	vertex_id_type const&			m_target_vertex_id;
	mutable vertex_id_type			m_best_vertex_id;
	mutable float					m_min_heuristics_value;
	float const						m_step_time;
	float const						m_max_speed;
}; // struct animation_space_heuristics


class animation_space_search_restrictor :
	public xray::ai::search_restrictor::generic <
		vertex_id_type,
		animation_space_graph_wrapper::const_edge_iterator,
		distance_type
	>
{
private:
	typedef xray::ai::search_restrictor::generic <
		vertex_id_type,
		animation_space_graph_wrapper::const_edge_iterator,
		distance_type
	> super;

public:
	inline	animation_space_search_restrictor	(
				animation_space_graph const& graph,
				vertex_id_type const& start_vertex_id,
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
		m_graph					( graph )
	{
	}

	template < typename VertexType >
	inline	bool	accessible	( vertex_id_type const& neighbour_vertex_id, VertexType const& current_vertex, animation_space_graph_wrapper::const_edge_iterator const& edge_iterator ) const
	{
		XRAY_UNREFERENCED_PARAMETER	( edge_iterator );
		return					m_graph.is_passable( current_vertex.id(), neighbour_vertex_id );
	}

	inline	u32		get_start_vertices_count( ) const
	{
		return					1;
	}

private:
	animation_space_graph const&	m_graph;
}; // class animation_space_search_restrictor 

class search_service {
public:
	typedef distance_type									distance_type;
	typedef vertex_id_type									vertex_id_type;
	typedef edge_id_type									edge_id_type;
	typedef xray::ai::vertex_allocator::fixed_count			vertex_allocator_type;
	typedef xray::ai::vertex_manager::fixed_count_hash		vertex_manager_type;
	typedef xray::ai::priority_queue::binary_heap			priority_queue_type;
	typedef animation_space_graph_wrapper					graph_wrapper_type;
	typedef xray::ai::path_constructor::edge				path_constructor_helper_type;
	typedef xray::ai::a_star								algorithm_type;

	struct vertex_type : 
		public algorithm_type::vertex_helper< distance_type >::vertex_impl,
		public graph_wrapper_type::vertex_impl,
		public path_constructor_helper_type::helper< edge_id_type>::vertex_impl< vertex_type >,
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

	typedef xray::debug::vector< edge_id_type >					path_type;
	typedef animation_space_heuristics							path_heuristics_type;
	typedef xray::ai::path_constructor::edge::impl<
				vertex_type,
				path_type
			>													path_constructor_type;
	typedef animation_space_search_restrictor					search_restrictor_type;

private:
	enum {
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
			animation_space_graph const& graph,
			path_type* path,
			vertex_id_type const start_vertex_id,
			vertex_id_type const target_vertex_id
		);

public:
	vertex_allocator_impl_type	m_vertex_allocator;
	vertex_manager_impl_type	m_vertex_manager;
	priority_queue_impl_type	m_priority_queue;
}; // class search_service

} // namespace stalker2

namespace xray {
namespace ai {
namespace vertex_manager {

inline	u32	hash_value	( stalker2::vertex_id_type const& vertex_id )
{
	boost::crc_32_type		processor;
	processor.process_block	( xray::pointer_cast<pcbyte>( &vertex_id ), xray::pointer_cast<pcbyte>( &vertex_id ) + sizeof(vertex_id.rotation) + sizeof(vertex_id.translation) );
	return					processor.checksum();
}

} // namespace vertex_manager
} // namespace ai

#endif // #ifndef MASTER_GOLD

} // namespace xray

#endif // #ifndef HUMAN_NPC_ANIMATION_CONTROLLER_PLANNER_H_INCLUDED