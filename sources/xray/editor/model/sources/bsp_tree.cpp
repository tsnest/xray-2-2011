////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "bsp_tree.h"
#include "bsp_tree_node.h"
#include "bsp_tree_utilities.h"
#include "triangles_mesh_utilities.h"
#include "generated_portal.h"
#pragma managed(push, off)
#include <xray/render/facade/debug_renderer.h>
#include <xray\ai_navigation\sources\graph_generator_adjacency_builder.h>
#include <xray/collision/geometry.h>
#pragma managed(pop)

namespace xray {
namespace model_editor {

bool bsp_tree::ms_draw_extracted_portals = false;

bsp_tree::bsp_tree(  ):
m_root( NULL ),
m_selected_node( NULL ),
m_portals_generated( false ),
m_initial_vertex_count( 0 ),
m_initial_index_count( 0 ),
m_bsp_tree_triangle_count( 0 ),
m_mesh_ready( false ),
m_bsp_ready( false )
{
}

bsp_tree::~bsp_tree(  )
{
	this->reset();
}

void bsp_tree::generate_bsp_tree( math::float3 const* vertices, u32 vertex_count, u32 const* indices, u32 index_count )
{
	m_initial_vertex_count	= vertex_count;
	initialize_triangles_mesh( vertices, vertex_count, indices, index_count, m_triangle_mesh );
	m_initial_index_count = m_triangle_mesh.indices.size();
	ai::navigation::graph_generator_adjacency_builder ( m_triangle_mesh.indices, m_triangle_mesh.data, m_triangle_mesh.data, 1.0f - math::epsilon_3 );
	
	math::aabb initial_aabb = math::create_aabb_min_max( m_triangle_mesh.vertices.front(), m_triangle_mesh.vertices.front() );
	triangles_mesh_type::vertices_type::const_iterator const vertices_end_it = m_triangle_mesh.vertices.end();
	for ( triangles_mesh_type::vertices_type::const_iterator it = m_triangle_mesh.vertices.begin(); it != vertices_end_it; ++it )
	{
		initial_aabb.modify( *it );
	}
	math::convex initial_convex( initial_aabb );

	triangle_ids_type triangle_ids;
	u32 const triangle_count = m_triangle_mesh.data.size();
	triangle_ids.reserve( triangle_count );
	for ( u32 i = 0; i < triangle_count; ++i )
	{
		triangle_ids.push_back( i );
		R_ASSERT( is_triangle_adjacency_correct( i, m_triangle_mesh ) );
	}
	

	m_root = NEW( bsp_tree_node )( m_triangle_mesh, triangle_ids, initial_convex );
	m_root->build_bsp();
	m_bsp_tree_triangle_count = m_triangle_mesh.data.size();
	remove_duplicates( m_triangle_mesh, false );
	m_bsp_ready = true;
	m_mesh_ready = true;
}

void bsp_tree::fill_tree_view( System::Windows::Forms::TreeView^ tree_view )
{
	System::Windows::Forms::TreeNode^ root_node = gcnew System::Windows::Forms::TreeNode("RootNode");
	tree_view->Nodes->Add( root_node );
	root_node->Tag = bsp_tree_node_wrapper( m_root );
	m_root->fill_tree_view_node( root_node );
	tree_view->ExpandAll();
}

void bsp_tree::set_selected_node( bsp_tree_node const* selected_node )
{
	m_selected_node = selected_node;
}

void bsp_tree::render( render::scene_ptr const& scene, render::debug::renderer& r ) const
{
	if ( !this->is_created() )
		return;
	if ( m_selected_node )
		m_selected_node->render( scene, r );
	else
		m_root->render( scene, r );
	if ( ms_draw_extracted_portals )
		this->draw_generated_portals( scene, r );
}

void bsp_tree::serialise_selected_node ( configs::lua_config_ptr config ) const
{
	if ( m_selected_node )
	{
		configs::lua_config_value root_cfg = (*config)["root"];
		m_selected_node->serialise( root_cfg );
	}
}

void bsp_tree::serialise_whole_tree( configs::lua_config_ptr config ) const
{
	if ( m_root )
	{
		configs::lua_config_value root_cfg = (*config)["root"];
		m_root->serialise( root_cfg );
	}
}

void bsp_tree::restore( configs::binary_config_ptr config )
{
	m_root = NEW( bsp_tree_node )( m_triangle_mesh );
	m_root->restore( ( *config ) [ "root" ] );
	m_bsp_ready = true;
}

void bsp_tree::reset()
{
	m_selected_node = NULL;
	m_bsp_ready = false;
	m_portals_generated = false;
	DELETE( m_root );
	m_root = NULL;
	m_triangle_mesh.data.clear();
	m_triangle_mesh.vertices.clear();
	m_triangle_mesh.indices.clear();
	if ( m_triangle_mesh.spatial_tree )
		m_triangle_mesh.destroy_spatial_tree();
	m_bsp_tree_triangle_count	= 0;
	m_initial_vertex_count	= 0;
	m_initial_index_count	= 0;
	m_portals.clear();
	m_mesh_ready = false;
	m_bsp_ready	= false;
}

math::aabb bsp_tree::get_aabb () const
{
	if ( m_selected_node )
		return m_selected_node->get_aabb();

	return math::create_identity_aabb();
}

void bsp_tree::generate_portals( )
{
	if ( m_root ) 
	{
		bsp_tree_node::portal_blanks_type dummy;
		triangle_ids_type portal_triangles;
		m_root->generate_portals( dummy, portal_triangles );
		remove_duplicates( m_triangle_mesh, false );
		remove_duplicate_triangles( m_triangle_mesh, portal_triangles );
		remove_empty_triangles( m_triangle_mesh, portal_triangles, 1e-4f );
		this->remove_portal_triangles_coincident_to_geomertry( portal_triangles );
		ai::navigation::graph_generator_adjacency_builder ( m_triangle_mesh.indices, portal_triangles, m_triangle_mesh.data, m_triangle_mesh.data, 1.0f - math::epsilon_3 );
		extract_portals_from_mesh( m_triangle_mesh, portal_triangles, m_portals );
		m_portals_generated = true;
	}
}
static inline bool true_predicate( xray::collision::ray_triangle_result const& ) 
{
	return true;
}

void bsp_tree::remove_portal_triangles_coincident_to_geomertry( triangle_ids_type& portal_triangles )
{
	struct triangle_is_coincident_to_geometry : public std::unary_function<u32, bool>
	{
		triangle_is_coincident_to_geometry( triangles_mesh_type const& mesh )
			:m_mesh( &mesh ){}
		bool operator()( u32 triangle_id ) const
		{
			float const c_max_distance = 2e-2f;
			math::float3 const& v0				= m_mesh->vertices[ m_mesh->indices[ triangle_id * 3 ] ];
			math::float3 const& v1				= m_mesh->vertices[ m_mesh->indices[ triangle_id * 3 + 1 ] ];
			math::float3 const& v2				= m_mesh->vertices[ m_mesh->indices[ triangle_id * 3 + 2 ] ];
			math::float3 const& center_point	= ( v0 + v1 + v2 ) / 3.0f;
			math::float3 const& ray_direction	= math::get_plane_normal( v0, v1, v2 );
			math::float3 const& origin_point	= center_point - ray_direction * 1e-2f;
			collision::ray_triangles_type ray_triangles( g_allocator );
			float distance = math::float_max;
			if ( m_mesh->spatial_tree->ray_query ( NULL, origin_point, ray_direction, c_max_distance, distance, ray_triangles, true_predicate ) )
			{
				collision::ray_triangles_type::const_iterator const ray_triangles_end = ray_triangles.end();
				for ( collision::ray_triangles_type::const_iterator it = ray_triangles.begin(); it != ray_triangles_end; ++it )
				{
					math::float3 const& u0 = m_mesh->vertices[ m_mesh->indices[ it->triangle_id * 3 ] ];
					math::float3 const& u1 = m_mesh->vertices[ m_mesh->indices[ it->triangle_id * 3 + 1 ] ];
					math::float3 const& u2 = m_mesh->vertices[ m_mesh->indices[ it->triangle_id * 3 + 2 ] ];
					math::plane const& triangle_plane =  math::create_plane( u0, u1, u2 );
					if ( math::is_zero( triangle_plane.classify( center_point ), 1e-2f ) )
						return true;
				}
			}
			return false;
		}
	private:
		triangles_mesh_type const* m_mesh;
	};
	portal_triangles.erase(
		std::remove_if( portal_triangles.begin(), portal_triangles.end(), triangle_is_coincident_to_geometry( m_triangle_mesh ) ),
		portal_triangles.end()
	);
}

void bsp_tree::serialise_triangles_mesh( configs::lua_config_ptr config ) const
{
	static u32 const sc_max_table_items_count = 65535;
	( *config )["initial_vertex_count"] = m_initial_vertex_count;
	( *config )["initial_index_count"] = m_initial_index_count;

	configs::lua_config_value vertices_cfg = (*config)["vertices"];
	u32 const vertices_count = m_triangle_mesh.vertices.size();
	for ( u32 i = 0; i < vertices_count; ++i )
	{
		vertices_cfg[ i / sc_max_table_items_count ][ i % sc_max_table_items_count ] = m_triangle_mesh.vertices[ i ];
	}
	configs::lua_config_value indices_cfg = (*config)["indices"];
	u32 const indices_count = m_triangle_mesh.indices.size();
	for ( u32 i = 0; i < indices_count; ++i )
	{
		indices_cfg[ i / sc_max_table_items_count ][ i % sc_max_table_items_count ] = m_triangle_mesh.indices[ i ];
	}
	configs::lua_config_value triangles_cfg = (*config)["triangles"];
	u32 const triangles_count = m_triangle_mesh.data.size();
	for ( u32 i = 0; i < triangles_count; ++i )
	{
		bsp_tree_triangle const& current			= m_triangle_mesh.data[ i ];
		configs::lua_config_value current_cfg		= triangles_cfg[ i / sc_max_table_items_count ][ i % sc_max_table_items_count ];
		current_cfg[ "plane" ]						= current.plane.vector;
		current_cfg[ "square" ]						= current.square;
		if ( current.has_neighbors() )
		{
			configs::lua_config_value neighbours_cfg	= current_cfg["neighbours"];
			neighbours_cfg[ 0 ]							= current.neighbours[ 0 ];
			neighbours_cfg[ 1 ]							= current.neighbours[ 1 ];
			neighbours_cfg[ 2 ]							= current.neighbours[ 2 ];
		}
		if ( current.parent != bsp_tree_triangle::msc_empty_id )
			current_cfg[ "parent" ]					= current.parent;
		if ( current.has_children() )
		{
			configs::lua_config_value children_cfg	= current_cfg[ "children" ];
			children_cfg[ 0 ]						= current.children[ 0 ];
			children_cfg[ 1 ]						= current.children[ 1 ];
			children_cfg[ 2 ]						= current.children[ 2 ];
		}
	}
}

void bsp_tree::restore_triangles_mesh( configs::binary_config_ptr const config )
{
	configs::binary_config_value root_cfg = config->get_root();
	R_ASSERT( root_cfg.value_exists( "initial_index_count" ) );
	m_initial_index_count = static_cast<u32>( root_cfg["initial_index_count"] );
	R_ASSERT( root_cfg.value_exists( "initial_vertex_count" ) );
	m_initial_vertex_count = static_cast<u32>( root_cfg["initial_vertex_count"] );
	
	R_ASSERT( root_cfg.value_exists( "vertices" ) );
	configs::binary_config_value vertices_cfg = root_cfg["vertices"];
	configs::binary_config_value::const_iterator vertices_end	= vertices_cfg.end();
	for ( configs::binary_config_value::const_iterator it = vertices_cfg.begin(); it != vertices_end; ++it )
	{
		configs::binary_config_value::const_iterator const sub_container_end	= ( *it ).end();
		for ( configs::binary_config_value::const_iterator sub_it = ( *it ).begin(); sub_it != sub_container_end; ++sub_it )
			m_triangle_mesh.vertices.push_back( static_cast<math::float3>( *sub_it ) );
	}

	R_ASSERT( root_cfg.value_exists( "indices" ) );
	configs::binary_config_value indices_cfg = root_cfg["indices"];
	configs::binary_config_value::const_iterator indices_end	= indices_cfg.end();
	for ( configs::binary_config_value::const_iterator it = indices_cfg.begin(); it != indices_end; ++it )
	{
		configs::binary_config_value::const_iterator const sub_container_end	= ( *it ).end();
		for ( configs::binary_config_value::const_iterator sub_it = ( *it ).begin(); sub_it != sub_container_end; ++sub_it )
			m_triangle_mesh.indices.push_back( static_cast<triangles_mesh_type::indices_type::value_type>( *sub_it ) );
	}

	R_ASSERT( root_cfg.value_exists( "triangles" ) );
	configs::binary_config_value triangles_cfg = root_cfg["triangles"];
	configs::binary_config_value::const_iterator triangles_end	= triangles_cfg.end();
	for ( configs::binary_config_value::const_iterator it = triangles_cfg.begin(); it != triangles_end; ++it )
	{
		configs::binary_config_value::const_iterator const sub_container_end	= ( *it ).end();
		for ( configs::binary_config_value::const_iterator sub_it = ( *it ).begin(); sub_it != sub_container_end; ++sub_it )
		{
			bsp_tree_triangle current;
			configs::binary_config_value current_cfg = *sub_it;
			current.plane.vector = static_cast<math::float4>( current_cfg[ "plane" ] );
			if ( current_cfg.value_exists( "neighbours" ) )
			{
				configs::binary_config_value neighbours_cfg	= current_cfg["neighbours"];
				current.neighbours[ 0 ] = static_cast<u32>( neighbours_cfg[ 0 ] );
				current.neighbours[ 1 ] = static_cast<u32>( neighbours_cfg[ 1 ] );
				current.neighbours[ 2 ] = static_cast<u32>( neighbours_cfg[ 2 ] );
			}
			current.square			= static_cast<float>( current_cfg["square"] );
			current.parent			= current_cfg.value_exists( "parent" ) ? static_cast<u32>( current_cfg["parent"] ) : bsp_tree_triangle::msc_empty_id;
			if ( current_cfg.value_exists( "children" ) )
			{
				configs::binary_config_value children_cfg	= current_cfg[ "children" ];
				current.children[ 0 ]					= children_cfg[ 0 ];
				current.children[ 1 ]					= children_cfg[ 1 ];
				current.children[ 2 ]					= children_cfg[ 2 ];
			}
			m_triangle_mesh.data.push_back( current );
		}
	}
	m_bsp_tree_triangle_count = m_triangle_mesh.data.size();
	m_triangle_mesh.build_spatial_tree( m_initial_index_count );
	m_mesh_ready = true;
}

void bsp_tree::draw_generated_portals  ( render::scene_ptr const& scene, render::debug::renderer& r ) const
{
	generated_portals_type::const_iterator const end_it = m_portals.end();
	for ( generated_portals_type::const_iterator it = m_portals.begin(); it != end_it; ++it )
	{
		it->render( scene, r );
	}
}

//void bsp_tree::filtrate_portals()
//{
//	if ( !m_portals_generated )
//		return;
//	pvoid edges_buffer = MALLOC( sizeof( edge ) * 3 * m_bsp_tree_triangle_count, "buffer for all edges of bsp-tree triangles" );
//	edges_buffer_type all_edges( edges_buffer, 3 * m_bsp_tree_triangle_count );
//	u32 const triangle_count = m_bsp_tree_triangle_count;
//	for ( u32 i = 0; i < triangle_count; ++i )
//	{
//		u32 const first		= m_triangle_mesh.indices[ i * 3 ];
//		u32 const second	= m_triangle_mesh.indices[ i * 3 + 1 ];
//		u32 const third		= m_triangle_mesh.indices[ i * 3 + 2 ];
//		all_edges.push_back( edge( std::min( first, second ), std::max( first, second ) ) );
//		all_edges.push_back( edge( std::min( second, third ), std::max( second, third ) ) );
//		all_edges.push_back( edge( std::min( third, first  ), std::max( third, first  ) ) );
//	}
//	std::sort( all_edges.begin(), all_edges.end(), edge_less() );
//	all_edges.erase( std::unique( all_edges.begin(), all_edges.end(), edge_equals() ), all_edges.end() );
//	
//	pvoid by_finish_buffer = MALLOC( sizeof( edge ) * all_edges.size(), "buffer for edges of bsp-tree triangles sorted_by finish" );
//	edges_buffer_type edges_by_finish( by_finish_buffer, all_edges.size() );
//	edges_by_finish.insert( edges_by_finish.end(), all_edges.begin(), all_edges.end() );
//	std::sort( edges_by_finish.begin(), edges_by_finish.end(), edge_less_by_index( 1 ) );
//
//	pvoid indices_buffer = MALLOC( sizeof( indices_buffer_type::value_type ) * all_edges.size() * 2, "buffer for indices of all triangles in bsp-tree" );
//	indices_buffer_type all_indices( indices_buffer, all_edges.size() * 2 );
//	edges_buffer_type::const_iterator const edge_end = all_edges.end();
//	for ( edges_buffer_type::const_iterator edge_it = all_edges.begin(); edge_it != edge_end; ++edge_it )
//	{
//		all_indices.push_back( edge_it->vertex_index0 );
//		all_indices.push_back( edge_it->vertex_index1 );
//	}
//	std::sort( all_indices.begin(), all_indices.end() );
//	all_indices.erase( std::unique( all_indices.begin(), all_indices.end() ), all_indices.end() );
//
//	generated_portals_type::iterator const portal_end = m_portals.end();
//	for ( generated_portals_type::iterator portal_it = m_portals.begin(); portal_it != portal_end; ++portal_it )
//		portal_it->check_hanged( all_edges, all_indices, edges_by_finish );
//	FREE( indices_buffer );
//	FREE( by_finish_buffer );
//	FREE( edges_buffer );
//}
//
} // namespace model_editor
} // namespace xray
