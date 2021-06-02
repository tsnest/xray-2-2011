////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "bsp_tree_node.h"
#include "bsp_portal_blank.h"
#include "bsp_tree_utilities.h"
#include "triangles_mesh_utilities.h"
#pragma managed(push, off)
#include <xray/render/facade/debug_renderer.h>
#include <xray\collision\geometry.h>
#include <xray\ai_navigation\sources\graph_generator_adjacency_builder.h>
#include <xray\ai_navigation\sources\constrained_delaunay_triangulator.h>
#pragma managed(pop)
namespace xray {
namespace model_editor {

struct separation_result
{
	separation_result( u32 polygon_id, float  rel, u32 split, u32 coincident, float square )
		:id( polygon_id ), relation( rel ), split_count( split ), coincident_count(coincident), coincident_square( square ){}
	float get_rating() const
	{
		return coincident_square * relation / ( split_count != 0 ? split_count * 10.0f : 1.0f );
	}
	u32					id;
	float				relation;
	u32					split_count;
	u32					coincident_count;
	float				coincident_square;
};

struct separation_result_better : public std::binary_function<separation_result const&, separation_result const&, bool>
{
	bool operator() ( separation_result const& first, separation_result const& second ) const
	{
		return /*first.split_count <= second.split_count &&*/ first.get_rating() > second.get_rating();
	}
};

static float get_triangles_accumulated_square( triangles_mesh_type const& triangle_mesh, triangle_ids_type const& ids )
{
	float result = 0.0f;
	triangle_ids_type::const_iterator const end_it = ids.end();
	for ( triangle_ids_type::const_iterator it = ids.begin(); it != end_it; ++it )
	{
		result += triangle_mesh.data[ *it ].square;
	}
	return result;
}

static bool is_point_on_any_edge( math::float3 const& normal, math::float3 const& point, edges_type const& edges, triangles_mesh_type const& mesh )
{
	u32 coordinate_indices[2];
	xray::ai::navigation::fill_coordinate_indices( normal, coordinate_indices );
	edges_type::const_iterator const edges_end = edges.end();
	for ( edges_type::const_iterator it = edges.begin(); it != edges_end; ++it )
	{
		math::float3 const& first	= mesh.vertices[ it->vertex_index0 ];
		math::float3 const& second	= mesh.vertices[ it->vertex_index1 ];
		if ( is_similar( coordinate_indices, point, first, math::epsilon_3  ) || 
			 is_similar( coordinate_indices, point, second, math::epsilon_3 ) || 
			 is_on_segment( coordinate_indices, point, first, second, math::epsilon_6 ) )
			return true;
	}
	return false;
}

//---------------------------------------------------------------------------------------------------
float bsp_tree_node::ms_min_portal_square = 0.0f;
float bsp_tree_node::ms_slimness_threshold = 0.0f;
bool bsp_tree_node::ms_draw_solid = false;
bool bsp_tree_node::ms_draw_aabb = true;
bool bsp_tree_node::ms_draw_double_sided = false;
bool bsp_tree_node::ms_draw_divider = false;
bool bsp_tree_node::ms_draw_geometry = false;
bool bsp_tree_node::ms_draw_portal_blanks = false;
bool bsp_tree_node::ms_draw_portal = false;
bool bsp_tree_node::ms_draw_outer_edges = false;

bsp_tree_node::bsp_tree_node ( triangles_mesh_type& triangle_mesh ):
m_positive(NULL),
m_negative(NULL),
m_mesh( triangle_mesh ),
m_broken( false ),
m_divider_id( bsp_tree_triangle::msc_empty_id ),
m_predicted_spanning( 0 ),
m_real_spanning( 0 )
{
}

bsp_tree_node::bsp_tree_node( triangles_mesh_type& triangle_mesh, triangle_ids_type const& triangle_ids, math::convex const& bounding_convex  ):
m_positive(NULL),
m_negative(NULL),
m_mesh( triangle_mesh ),
m_triangle_ids( triangle_ids ),
m_broken( false ),
m_convex( bounding_convex ),
m_divider_id( bsp_tree_triangle::msc_empty_id ),
m_predicted_spanning( 0 ),
m_real_spanning( 0 )
{
	m_convex.fill_cache( m_convex_cache );
}

bsp_tree_node::~bsp_tree_node( )
{
	DELETE ( m_negative );
	DELETE ( m_positive );
}

u32 bsp_tree_node::choose_dividing_polygon()
{
	u32 const total_triangles = m_mesh.spatial_tree->get_triangles_count( m_convex );
	math::convex positive_convex, negative_convex;
	typedef std::vector<separation_result> separation_results_type;
	separation_results_type results;
	triangle_ids_type processed; 
	triangle_ids_type::const_iterator const id_end_it = m_triangle_ids.end();
	for ( triangle_ids_type::const_iterator id_it = m_triangle_ids.begin(); id_it != id_end_it; ++id_it )
	{
		R_ASSERT( !m_mesh.data[ *id_it ].has_children() );
		if ( m_mesh.data[ *id_it ].divider || std::binary_search( processed.begin(), processed.end(), *id_it ) )
			continue;
		u32 const triangle_id = *id_it;
		triangle_ids_type coplanar_triangles; 
		adjacent_coplanar_triangles( triangle_id, m_mesh, coplanar_triangles );
		triangle_ids_type temp;
		temp.reserve( coplanar_triangles.size() + processed.size() );
		std::merge( coplanar_triangles.begin(), coplanar_triangles.end(), processed.begin(), processed.end(), std::back_inserter( temp ) );
		std::swap( temp, processed );
		float coincident_square = 0.0f;
		triangle_ids_type::const_iterator const end_it = coplanar_triangles.end();
		for ( triangle_ids_type::const_iterator it = coplanar_triangles.begin(); it != end_it; ++it )
		{
			math::float3 const& first	= m_mesh.vertices[ m_mesh.indices[ ( *it ) * 3 ] ];
			math::float3 const& second	= m_mesh.vertices[ m_mesh.indices[ ( *it ) * 3 + 1 ] ];
			math::float3 const& third	= m_mesh.vertices[ m_mesh.indices[ ( *it ) * 3 + 2 ] ];
			coincident_square += ( ( second - first ) ^ ( third - first ) ).length() * 0.5f;
		}
		math::plane const& divider = m_mesh.data[ triangle_id ].plane;
		m_convex.split( divider, m_convex_cache, positive_convex, negative_convex );
		
		u32 const coincident_count = coplanar_triangles.size();
		u32 positive_count = 0, negative_count = 0, split_count = 0;
		if ( positive_convex.empty() )
		{
			R_ASSERT( !negative_convex.empty() );
			positive_count	= coincident_count;
			negative_count	= total_triangles - coincident_count;
			split_count		= 0;
		}
		else
		{
			//TODO: split_count sometimes is calculating incorrect, this causes suboptimal partitioning
			//R_ASSERT( m_triangle_mesh.spatial_tree->get_triangles_count( positive_convex ) <= total_triangles );
			//R_ASSERT( m_triangle_mesh.spatial_tree->get_triangles_count( negative_convex ) <= total_triangles );

			u32 const inside_positive_convex = std::max( m_mesh.spatial_tree->get_triangles_count( positive_convex ), coincident_count );
			u32 const inside_negative_convex = std::max( m_mesh.spatial_tree->get_triangles_count( negative_convex ), coincident_count );
			//R_ASSERT( inside_positive_convex + inside_negative_convex >= total_triangles + coincident_count );
			split_count		= ( inside_positive_convex + inside_negative_convex - total_triangles - coincident_count ) / 2;
			positive_count	= inside_positive_convex - split_count;
			negative_count	= inside_negative_convex - coincident_count - split_count;
			//R_ASSERT( positive_count );
		}
		float const relation = std::min( positive_count, negative_count ) / static_cast<float>( std::max ( positive_count, negative_count ) );
		results.push_back( separation_result( triangle_id, relation, split_count, coincident_count, coincident_square ) );
	}
	separation_results_type::const_iterator result_it = std::min_element( results.begin(), results.end(), separation_result_better() );
	if ( result_it != results.end() )
	{
		m_predicted_spanning = result_it->split_count;
		return result_it->id;
	}
	return bsp_tree_triangle::msc_empty_id;
}

void bsp_tree_node::build_bsp()
{
	if ( is_convex_polygon_set( m_triangle_ids, m_mesh ) || m_convex.empty() )
		return;
	static u32 const c_empty_divider = bsp_tree_triangle::msc_empty_id;
	triangle_ids_type positive_ids, negative_ids;
	for( ;; )//this loop prevents nodes with only one child
	{
		m_divider_id = this->choose_dividing_polygon();
		if ( m_divider_id == c_empty_divider )
		{
			m_broken = true;
			return;
		}
		R_ASSERT( !m_convex.empty() );
		m_mesh.data[ m_divider_id ].divider = true;
		triangle_ids_type coplanar_triangles;
		//simplified_adjacent_coincident_triangles( m_divider_id, m_mesh, coplanar_triangles );//FIX ME
		adjacent_coplanar_triangles( m_divider_id, m_mesh, coplanar_triangles );
		math::plane const divider = m_mesh.data[ m_divider_id ].plane;
		R_ASSERT( divider.valid() );
		triangle_ids_type::const_iterator const end_it = m_triangle_ids.end();
		for ( triangle_ids_type::const_iterator it = m_triangle_ids.begin(); it != end_it; ++it )
		{
			this->process_triangle( *it, divider, coplanar_triangles, positive_ids, negative_ids );
		}
		if ( !negative_ids.empty() )
			break;
		m_triangle_ids = positive_ids;
		positive_ids.clear();
		math::convex pos_part, neg_part;
		m_convex.split( divider, m_convex_cache, pos_part, neg_part );
		m_convex = pos_part;
		m_convex.fill_cache( m_convex_cache );
	}//this loop prevents nodes with only one child
	R_ASSERT( !positive_ids.empty() );
	triangle_ids_type dummy;
	std::swap( m_triangle_ids, dummy );
	
	math::convex positive_convex, negative_convex;
	m_convex.split( m_mesh.data[ m_divider_id ].plane, m_convex_cache, positive_convex, negative_convex );
	replace_triangles_with_their_children( m_mesh, negative_ids );
	m_negative = NEW ( bsp_tree_node ) ( m_mesh, negative_ids, negative_convex );
	m_negative->build_bsp();
	
	replace_triangles_with_their_children( m_mesh, positive_ids );
	m_positive = NEW ( bsp_tree_node ) ( m_mesh, positive_ids, positive_convex );
	m_positive->build_bsp();
}

void bsp_tree_node::process_triangle( u32 triangle_id, math::plane const& divider, triangle_ids_type& coplanar_triangles,
									 triangle_ids_type& positive_ids, triangle_ids_type& negative_ids )
{
	if ( m_mesh.data[ triangle_id ].has_children() )
	{
		if ( m_mesh.data[ triangle_id ].children[ 0 ] != bsp_tree_triangle::msc_empty_id )
			this->process_triangle( m_mesh.data[ triangle_id ].children[ 0 ], divider, coplanar_triangles, positive_ids, negative_ids );
		if ( m_mesh.data[ triangle_id ].children[ 1 ] != bsp_tree_triangle::msc_empty_id )
			this->process_triangle( m_mesh.data[ triangle_id ].children[ 1 ], divider, coplanar_triangles, positive_ids, negative_ids );
		if ( m_mesh.data[ triangle_id ].children[ 2 ] != bsp_tree_triangle::msc_empty_id )
			this->process_triangle( m_mesh.data[ triangle_id ].children[ 2 ], divider, coplanar_triangles, positive_ids, negative_ids );
	}
	else
	{
		R_ASSERT( is_triangle_adjacency_correct( triangle_id, m_mesh ) );
		if ( std::binary_search( coplanar_triangles.begin(), coplanar_triangles.end(), triangle_id ) )
		{
			positive_ids.push_back( triangle_id );
			m_mesh.data[ triangle_id ].divider = true;
			m_mesh.data[ triangle_id ].plane	= divider;
			return;
		}
		bsp_tree_triangle::side side = calculate_side( divider, triangle_id, m_mesh );
		switch ( side )
		{
		case bsp_tree_triangle::front:
			remove_adjacency_in_coplanar_edges( triangle_id, divider, m_mesh );
			positive_ids.push_back( triangle_id );
			break;
		case bsp_tree_triangle::back:
			remove_adjacency_in_coplanar_edges( triangle_id, divider, m_mesh );
			negative_ids.push_back( triangle_id );
			break;
		case bsp_tree_triangle::coplanar:
			this->process_coplanar_triangle( triangle_id, divider, coplanar_triangles, positive_ids, negative_ids );
			break;
		case bsp_tree_triangle::spanning:
			//R_ASSERT( std::find_if( positive_ids.begin(), positive_ids.end(), triangle_has_children( m_mesh ) ) ==  positive_ids.end() );
			//R_ASSERT( std::find_if( negative_ids.begin(), negative_ids.end(), triangle_has_children( m_mesh ) ) ==  negative_ids.end() );
			split_triangle( divider, triangle_id, m_mesh, positive_ids, negative_ids );
			//R_ASSERT( std::find_if( positive_ids.begin(), positive_ids.end(), triangle_has_children( m_mesh ) ) ==  positive_ids.end() );
			//R_ASSERT( std::find_if( negative_ids.begin(), negative_ids.end(), triangle_has_children( m_mesh ) ) ==  negative_ids.end() );
			++m_real_spanning;
			break;
		default:
			NODEFAULT();
		}
	}
}

void bsp_tree_node::process_coplanar_triangle( u32 triangle_id, math::plane const& divider, triangle_ids_type& coplanar_triangles, triangle_ids_type& positive_ids, triangle_ids_type& negative_ids )
{
	triangle_ids_type coplanar_to_this;
	adjacent_coplanar_triangles( triangle_id, m_mesh, coplanar_to_this );
	std::sort( negative_ids.begin(), negative_ids.end() );
	triangle_ids_type temp;
	temp.reserve( negative_ids.size() );
	std::set_intersection( negative_ids.begin(), negative_ids.end(), coplanar_to_this.begin(), coplanar_to_this.end(), std::back_inserter( temp ) );
	triangle_ids_type::const_iterator const intersection_end = temp.end();
	for ( triangle_ids_type::const_iterator it = temp.begin(); it != intersection_end; ++it )
	{
		m_mesh.data[ *it ].plane	= divider;
		m_mesh.data[ *it ].divider = true;
	}
	positive_ids.insert( positive_ids.end(), temp.begin(), temp.end() );
	temp.clear();
	std::set_difference( negative_ids.begin(), negative_ids.end(), coplanar_to_this.begin(), coplanar_to_this.end(), std::back_inserter( temp ) );
	std::swap( negative_ids, temp );
	temp.clear();
	temp.reserve( coplanar_triangles.size() + coplanar_to_this.size() );
	std::merge( 
		coplanar_triangles.begin(), coplanar_triangles.end(), 
		coplanar_to_this.begin(), coplanar_to_this.end(),
		std::back_inserter( temp )
		);
	std::swap( coplanar_triangles, temp );
	positive_ids.push_back( triangle_id );
	m_mesh.data[ triangle_id ].plane	= divider;
	m_mesh.data[ triangle_id ].divider = true;
}


bool bsp_tree_node::is_leaf( ) const
{
	return m_positive == NULL && m_negative == NULL;
}

void bsp_tree_node::fill_tree_view_node( System::Windows::Forms::TreeNode^ node )
{
	node->Tag = bsp_tree_node_wrapper( this );
	if ( m_positive )
	{
		System::String^ label = "pos:";
		if ( m_positive->is_leaf() )
			label += System::String::Format( "Triangles - {0}", m_positive->m_triangle_ids.size() );
		else
		{
			label += System::String::Format( "Spanning: {0} - {1}", m_positive->m_predicted_spanning, m_positive->m_real_spanning );
		}
		System::Windows::Forms::TreeNode^ positive_node = gcnew System::Windows::Forms::TreeNode( label );
		node->Nodes->Add( positive_node );
		m_positive->fill_tree_view_node( positive_node );
	}
	if ( m_negative )
	{
		System::String^ label = "neg: ";
		if ( m_negative->is_leaf() )
			label += System::String::Format( "Triangles - {0}", m_negative->m_triangle_ids.size() );
		else
		{
			label += System::String::Format( "Spanning: {0} - {1}", m_negative->m_predicted_spanning, m_negative->m_real_spanning );
		}
		System::Windows::Forms::TreeNode^ negative_node = gcnew System::Windows::Forms::TreeNode( label );
		node->Nodes->Add( negative_node );
		m_negative->fill_tree_view_node( negative_node );
	}
	if ( m_broken )
		node->BackColor = System::Drawing::Color::Gray;
}


void bsp_tree_node::render( render::scene_ptr const& scene, render::debug::renderer& r ) const
{
	if ( ms_draw_aabb )
		this->draw_bounding_convex( scene, r );

	if ( this->is_leaf() )
	{
		if ( ms_draw_geometry )
			this->draw_polygons( scene, r, math::color( 0, 255, 0 ), ms_draw_solid, ms_draw_double_sided );
		if ( ms_draw_outer_edges )
			this->draw_outer_edges( scene, r );
		if ( ms_draw_portal )
			this->draw_portals( scene, r );
	}
	else
	{
		if ( ms_draw_geometry && ms_draw_divider )
			this->draw_divider_triangle( scene, r );
		if ( ms_draw_divider ) 
			draw_portal_blank( scene, r, m_poratal_blank, math::color( 128, 128, 200 ) );
		if ( m_positive )
			m_positive->render( scene, r, math::color( 255, 0, 0 ) );
		if ( m_negative )
			m_negative->render( scene, r, math::color( 0, 0, 255 ) );
	}
	if ( ms_draw_portal_blanks )
		this->draw_portal_blanks( scene, r );
}

void bsp_tree_node::render( render::scene_ptr const& scene, render::debug::renderer& r, math::color const& c ) const
{
	if ( this->is_leaf() )
	{
		if ( ms_draw_geometry )
		{
			this->draw_polygons( scene, r, c, ms_draw_solid, ms_draw_double_sided );
		}
		if ( ms_draw_outer_edges )
			this->draw_outer_edges( scene, r );
		if ( ms_draw_portal )
			this->draw_portals( scene, r );
	}
	else
	{
		if ( m_positive )
			m_positive->render( scene, r, c );
		if ( m_negative )
			m_negative->render( scene, r, c );
	}
}

void bsp_tree_node::draw_polygons( render::scene_ptr const& scene, render::debug::renderer& r, math::color const& c, bool solid, bool double_sided ) const
{
	math::color const triangle_color ( c.r, c.g, c.b, 128 );
	triangle_ids_type::const_iterator const end_it = m_triangle_ids.end();
	for ( triangle_ids_type::const_iterator it = m_triangle_ids.begin(); it != end_it; ++it )
	{
		math::float3 const& first	= m_mesh.vertices[ m_mesh.indices[ ( *it ) * 3 ] ];
		math::float3 const& second	= m_mesh.vertices[ m_mesh.indices[ ( *it ) * 3 + 1 ] ];
		math::float3 const& third	= m_mesh.vertices[ m_mesh.indices[ ( *it ) * 3 + 2 ] ];
		r.draw_line( scene, first,	second,	c );
		r.draw_line( scene, second, third,	c );
		r.draw_line( scene, third,	first,	c );

		if ( solid )
		{
			r.draw_triangle( scene, first, second, third, triangle_color );
			if ( double_sided )
				r.draw_triangle( scene, first, third, second, triangle_color );
		}
	}
}

void bsp_tree_node::draw_portal_blanks( render::scene_ptr const& scene, render::debug::renderer& r  ) const
{
	portal_blanks_type::const_iterator const end_it = m_portal_blanks.end();
	for ( portal_blanks_type::const_iterator it = m_portal_blanks.begin(); it != end_it; ++it )
	{
		draw_portal_blank( scene, r, *it, math::color( 164, 128, 164 ) );
	}
}


void bsp_tree_node::draw_bounding_convex( render::scene_ptr const& scene, render::debug::renderer& r ) const
{
	math::color const convex_color( 255, 255, 255 );
	math::convex::cache::edges_type::const_iterator const end_it = m_convex_cache.edges.end();
	for ( math::convex::cache::edges_type::const_iterator it = m_convex_cache.edges.begin(); it != end_it; ++it )
	{
		math::float3 const& first	= m_convex_cache.vertices[ it->first ];
		math::float3 const& second	= m_convex_cache.vertices[ it->second ];
		r.draw_line( scene, first, second, convex_color );
	}
}

void bsp_tree_node::draw_divider_triangle( render::scene_ptr const& scene, render::debug::renderer& r  ) const
{
	if ( m_divider_id != bsp_tree_triangle::msc_empty_id )
	{
		math::color const divider_color( 255, 255, 255 );
		math::float3 const& first	= m_mesh.vertices[ m_mesh.indices[ m_divider_id * 3 ] ];
		math::float3 const& second	= m_mesh.vertices[ m_mesh.indices[ m_divider_id * 3 + 1 ] ];
		math::float3 const& third	= m_mesh.vertices[ m_mesh.indices[ m_divider_id * 3 + 2 ] ];
		r.draw_line( scene, first,	second,	divider_color);
		r.draw_line( scene, second, third,	divider_color );
		r.draw_line( scene, third,	first,	divider_color );
		r.draw_triangle( scene, first, second, third, divider_color );
		r.draw_triangle( scene, third, first, second, divider_color );
	}
}

void bsp_tree_node::draw_portals( render::scene_ptr const& scene, render::debug::renderer& r ) const
{
	math::color const triangle_color ( 128, 128, 255, 128 );
	math::color const line_color ( 128, 128, 255 );
	triangle_ids_type::const_iterator const id_end = m_portal_triangles.end();
	for ( triangle_ids_type::const_iterator id_it = m_portal_triangles.begin(); id_it != id_end; ++id_it )
	{
		u32 const triangle_id = *id_it;
		math::float3 const& first	= m_mesh.vertices[ m_mesh.indices[ triangle_id * 3 ] ];
		math::float3 const& second	= m_mesh.vertices[ m_mesh.indices[ triangle_id * 3 + 1 ] ];
		math::float3 const& third	= m_mesh.vertices[ m_mesh.indices[ triangle_id * 3 + 2 ] ];
		float const perimeter	= ( second - first ).length() + ( third - second ).length() + ( first - third ).length();
		float const square		= triangle_square( first, second, third );	
		if ( math::is_zero( perimeter ) || square / perimeter < ms_slimness_threshold )
			continue;
		r.draw_line( scene, first,	second,	line_color );
		r.draw_line( scene, second, third,	line_color );
		r.draw_line( scene, third,	first,	line_color );
		r.draw_triangle( scene, first, second, third, triangle_color );
	}
}

void bsp_tree_node::serialise( configs::lua_config_value& node_config ) const
{
	node_config[ "divider_id" ] = m_divider_id;
	math::convex::planes_type const& planes	= m_convex.get_planes();
	if ( u32 const planes_count = planes.size() )
	{
		configs::lua_config_value convex_cfg = node_config[ "convex" ];
		configs::lua_config_value planes_cfg = convex_cfg[ "planes" ];
		for ( u32 i = 0; i < planes_count; ++i )
		{
			planes_cfg[ i ] = planes[ i ].plane.vector;
		}
		configs::lua_config_value adjacencies_cfg = convex_cfg[ "adjacencies" ];
		math::convex::adjacencies_type const& adjacencies	= m_convex.get_adjacencies();
		u32 const adjacencies_count = adjacencies.size();
		for ( u32 i = 0; i < adjacencies_count; ++i )
		{
			adjacencies_cfg[ i ] = adjacencies[ i ];
		}
	}

	if ( this->is_leaf() )
	{
		configs::lua_config_value triangle_ids_cfg = node_config[ "triangle_ids" ];
		u32 const triangles_count = m_triangle_ids.size();
		for ( u32 i = 0; i < triangles_count; ++i )
		{
			triangle_ids_cfg[ i ] = m_triangle_ids[ i ];
		}
	}
	else
	{
		if ( m_positive )
		{
			configs::lua_config_value positive_config = node_config[ "positive" ];
			m_positive->serialise( positive_config );
		}
		if ( m_negative )
		{
			configs::lua_config_value negative_config = node_config[ "negative" ];
			m_negative->serialise( negative_config );
		}
	}
}

void bsp_tree_node::restore( configs::binary_config_value const& node_config )
{
	m_divider_id = static_cast<triangle_ids_type::value_type>( node_config[ "divider_id" ] );

	if ( node_config.value_exists( "convex" ) )
	{
		configs::binary_config_value convex_cfg = node_config[ "convex" ];
		R_ASSERT( convex_cfg.value_exists( "planes" ) );
		configs::binary_config_value planes_cfg = convex_cfg[ "planes" ];
		math::convex::planes_type planes;
		configs::binary_config_value::const_iterator plane_end_it = planes_cfg.end();
		for ( configs::binary_config_value::const_iterator it = planes_cfg.begin(); it != plane_end_it; ++it )
		{
			math::aabb_plane p;
			p.plane.vector = static_cast<math::float4>( *it );
			p.normalize();
			planes.push_back( p );
		}
		R_ASSERT( convex_cfg.value_exists( "adjacencies" ) );
		configs::binary_config_value adjacencies_cfg = convex_cfg[ "adjacencies" ];
		math::convex::adjacencies_type adjacencies;
		configs::binary_config_value::const_iterator adjacencies_end_it = adjacencies_cfg.end();
		for ( configs::binary_config_value::const_iterator it = adjacencies_cfg.begin(); it !=  adjacencies_end_it; ++it )
		{
			adjacencies.push_back( static_cast<math::convex::adjacencies_type::value_type>( *it ) );
		}
		m_convex = math::convex( planes, adjacencies );
		m_convex.fill_cache( m_convex_cache );
		
	}

	if ( node_config.value_exists( "triangle_ids" ) )
	{
		configs::binary_config_value triangle_ids_cfg = node_config[ "triangle_ids" ];
		configs::binary_config_value::const_iterator end_it	= triangle_ids_cfg.end( );
		for ( configs::binary_config_value::const_iterator it = triangle_ids_cfg.begin(); it != end_it; ++it )
		{
			m_triangle_ids.push_back( *it );
		}
	}
	else
	{
		if ( node_config.value_exists( "positive" ) )
		{
			m_positive = NEW (bsp_tree_node)( m_mesh );
			m_positive->restore( node_config["positive"] );
		}
		if ( node_config.value_exists( "negative" ) )
		{
			m_negative = NEW (bsp_tree_node)(  m_mesh );
			m_negative->restore( node_config["negative"] );
		}
	}
}

math::aabb bsp_tree_node::get_aabb() const
{
	if ( m_convex_cache.vertices.empty() )
		return math::create_identity_aabb();

	math::aabb result = math::create_aabb_min_max( m_convex_cache.vertices.front(), m_convex_cache.vertices.back() );
	for ( math::convex::cache::vertices_type::const_iterator it = m_convex_cache.vertices.begin(); it != m_convex_cache.vertices.end(); ++it )
		result.modify( *it );
	return result;
}

void bsp_tree_node::generate_portals( portal_blanks_type& blanks, triangle_ids_type& portal_triangles  )
{
	m_portal_blanks = blanks;
	if ( !this->is_leaf() )
	{
		math::plane const& divider = m_mesh.data[ m_divider_id ].plane;
		portal_blanks_type positive_blanks, negative_blanks;
		const portal_blanks_type::const_iterator end_it = blanks.end();
		for ( portal_blanks_type::const_iterator it = blanks.begin(); it != end_it; ++it )
		{
			bsp_portal_blank positive_blank, negative_blank;
			it->split( divider, positive_blank, negative_blank );
			if ( positive_blank.get_vertices().size() >= 3 )
				positive_blanks.push_back( positive_blank );
			if ( negative_blank.get_vertices().size() >= 3 )
				negative_blanks.push_back( negative_blank );
		}
		m_poratal_blank.initialize( m_convex, m_convex_cache, m_mesh.data[ m_divider_id ].plane );
		positive_blanks.push_back( m_poratal_blank );
		if ( m_positive )
			m_positive->generate_portals( positive_blanks, portal_triangles );
		if ( m_negative )
			m_negative->generate_portals( negative_blanks, portal_triangles );
	}
	else
	{
		portal_blanks_type::iterator const blank_end_it = m_portal_blanks.end();
		for ( portal_blanks_type::iterator blank_it = m_portal_blanks.begin(); blank_it != blank_end_it; ++blank_it )
		{
			triangle_ids_type coplanar_triangles;
			triangle_ids_type::const_iterator const end_id = m_triangle_ids.end();
			for ( triangle_ids_type::const_iterator id_it = m_triangle_ids.begin(); id_it != end_id; ++id_it )
			{
				if ( math::is_zero( m_mesh.data[ *id_it ].square ) )
					continue;
				if ( m_mesh.data[ *id_it ].is_coplanar( blank_it->get_plane() ) )
					coplanar_triangles.push_back( *id_it );
				else
					blank_it->limit_by_positive_part( m_mesh.data[ *id_it ].plane );
			}
			remove_empty_triangles( m_mesh, coplanar_triangles );
			if ( coplanar_triangles.empty() )
				continue;
			float const triangles_square = get_triangles_accumulated_square( m_mesh, coplanar_triangles );
			if ( math::is_zero( blank_it->get_square() ) || math::is_zero( triangles_square ) || triangles_square / blank_it->get_square() < 0.25f )
				continue;
			
			edges_type edges;
			get_outer_edges( m_mesh, coplanar_triangles, edges );
			R_ASSERT( !edges.empty() );
			this->preprocess_triangulation_input( blank_it->get_plane().normal, edges );
			m_outer_edges.reserve( m_outer_edges.size() + edges.size() );

			edges_type::const_iterator const edges_end = edges.end();
			for ( edges_type::const_iterator edge_it = edges.begin(); edge_it != edges_end; ++edge_it )
			{
				m_outer_edges.push_back( std::make_pair( m_mesh.vertices[ edge_it->vertex_index0 ], m_mesh.vertices[ edge_it->vertex_index1 ] ) );
			}
			
			input_indices_type indices_from_edges;
			get_vertex_indices_of_edges( edges, indices_from_edges );
			input_indices_type input_indices( indices_from_edges );

			bsp_portal_blank::vertices_type blank_vertices = blank_it->get_vertices();
			input_indices_type indices_from_blank;
			indices_from_blank.reserve( blank_vertices.size() );
			bsp_portal_blank::vertices_type::const_iterator blank_vertices_end = blank_vertices.end();
			for ( bsp_portal_blank::vertices_type::const_iterator vert_it = blank_vertices.begin(); vert_it != blank_vertices_end; ++vert_it )
			{
				if ( !is_point_on_any_edge( blank_it->get_plane().normal, *vert_it, edges, m_mesh )	)
				{
					input_indices.push_back( m_mesh.vertices.size() );
					indices_from_blank.push_back( m_mesh.vertices.size() );
					m_mesh.vertices.push_back( *vert_it );
				}
			}

			if ( input_indices.size() < 3 )
				continue;
			if ( !is_correct_triangulation_input_data( blank_it->get_plane().normal, m_mesh.vertices, input_indices, edges ) )
			{
				save_to_lua( m_mesh.vertices, input_indices, edges, blank_it->get_plane().normal );
				R_ASSERT( false );
			}
			typedef	xray::ai::navigation::constrained_delaunay_triangulator::indices_type triangulator_indices_type;
			typedef xray::ai::navigation::delaunay_triangulator delaunay_triangulator;
			u32 const max_triangle_count = delaunay_triangulator::get_maximum_triangle_count_in_triangulation( input_indices.size() );
			triangulator_indices_type output_indices ( ALLOCA( max_triangle_count * 3 * sizeof( triangulator_indices_type::value_type ) ), max_triangle_count, 0 );
			
			xray::ai::navigation::constrained_delaunay_triangulator( 
				m_mesh.vertices, 
				input_indices, 
				output_indices, 
				blank_it->get_plane().normal,
				edges, 0, u32( -1 )
			);
			this->extract_portal_triangles( output_indices, edges, indices_from_blank, blank_it->get_plane() );
		}
		portal_triangles.insert( portal_triangles.end(), m_portal_triangles.begin(), m_portal_triangles.end() );
	}
}

typedef	ai::navigation::delaunay_triangulator::indices_type triangulator_indices_type;
static inline bool triangle_contains_any_edge( u32 triangle_id, triangulator_indices_type const& indices, edges_type const& sorted_edges )
{
	u32 const first_index		= indices[ triangle_id * 3 ];
	u32 const second_index		= indices[ triangle_id * 3 + 1 ];
	u32 const third_index		= indices[ triangle_id * 3 + 2 ];

	return 	
		std::binary_search( sorted_edges.begin(), sorted_edges.end(), edge( first_index, second_index ), edge_strictly_less() ) ||
		std::binary_search( sorted_edges.begin(), sorted_edges.end(), edge( second_index, third_index ), edge_strictly_less() ) ||
		std::binary_search( sorted_edges.begin(), sorted_edges.end(), edge( third_index, first_index  ), edge_strictly_less() );
}

static inline bool triangle_contains_any_index( u32 triangle_id, triangulator_indices_type const& indices, input_indices_type const& sorted_indices )
{
	u32 const first_index		= indices[ triangle_id * 3 ];
	u32 const second_index		= indices[ triangle_id * 3 + 1 ];
	u32 const third_index		= indices[ triangle_id * 3 + 2 ];

	return 	
		std::binary_search( sorted_indices.begin(), sorted_indices.end(), first_index ) ||
		std::binary_search( sorted_indices.begin(), sorted_indices.end(), second_index ) ||
		std::binary_search( sorted_indices.begin(), sorted_indices.end(), third_index );
}

static inline bool is_portal_triangle( u32 triangle_id, triangulator_indices_type const& indices, edges_type const& inverted_edges, input_indices_type const& indices_from_blank )
{
	return triangle_contains_any_edge( triangle_id, indices, inverted_edges ) ||
		   triangle_contains_any_index( triangle_id, indices, indices_from_blank );
}

static inline bool is_neighbour_of_geometry( adjacency_type const& adjacency, triangulator_indices_type const& indices, edges_type const& sorted_edges )
{
	return 
		( adjacency.neighbours[ 0 ] != bsp_tree_triangle::msc_empty_id && triangle_contains_any_edge( adjacency.neighbours[ 0 ], indices, sorted_edges ) ) &&
		( adjacency.neighbours[ 1 ] != bsp_tree_triangle::msc_empty_id && triangle_contains_any_edge( adjacency.neighbours[ 1 ], indices, sorted_edges ) ) &&
		( adjacency.neighbours[ 2 ] != bsp_tree_triangle::msc_empty_id && triangle_contains_any_edge( adjacency.neighbours[ 2 ], indices, sorted_edges ) );

}

void bsp_tree_node::extract_portal_triangles ( triangulator_indices_type const& indices, edges_type const& sorted_edges, input_indices_type const& indices_from_blank, math::plane const& portal_blank_plane )
{
	u32 const triangle_count = indices.size() / 3;
	R_ASSERT( indices.size() % 3 == 0 );
	edges_type inverted_edges;
	inverted_edges.reserve( sorted_edges.size() );
	edges_type::const_iterator const edges_end = sorted_edges.end();
	for ( edges_type::const_iterator it = sorted_edges.begin(); it != edges_end; ++it )
	{
		inverted_edges.push_back( edge( it->vertex_index1, it->vertex_index0 ) );
	}
	std::sort( inverted_edges.begin(), inverted_edges.end(), edge_strictly_less() );

	xray::buffer_vector<adjacency_type> adjacencies ( ALLOCA( sizeof( adjacency_type ) * triangle_count ), triangle_count, triangle_count );
	ai::navigation::graph_generator_adjacency_builder ( indices, adjacencies );

	for ( u32 i = 0; i < triangle_count; ++i )
	{
		if ( is_portal_triangle( i, indices, inverted_edges, indices_from_blank ) || 
			 !is_neighbour_of_geometry( adjacencies[ i ], indices, sorted_edges ) )
		{
			u32 const first_index		= indices[ i * 3 ];
			u32 const second_index		= indices[ i * 3 + 1 ];
			u32 const third_index		= indices[ i * 3 + 2 ];

			 m_mesh.indices.push_back( first_index );
			 m_mesh.indices.push_back( second_index );
			 m_mesh.indices.push_back( third_index );
			 bsp_tree_triangle portal_triangle;
			 portal_triangle.plane = portal_blank_plane;
			 portal_triangle.square = triangle_square( m_mesh.vertices[ first_index ],  m_mesh.vertices[ second_index ], m_mesh.vertices[ third_index ] );
			 m_portal_triangles.push_back( m_mesh.data.size() );
			 m_mesh.data.push_back( portal_triangle );
		}
	}
}


void bsp_tree_node::preprocess_triangulation_input( math::float3 const& normal, edges_type& edges )
{
	coord_indices_pair coordinate_indices;
	xray::ai::navigation::fill_coordinate_indices( normal, coordinate_indices );
	remove_zero_edges					( coordinate_indices, m_mesh.vertices, edges, math::epsilon_5 );
	make_vertices_of_edges_unique_in_2d	( coordinate_indices, m_mesh.vertices, edges, math::epsilon_3 );
	merge_consecutive_edges				( coordinate_indices, m_mesh.vertices, edges );
	remove_edges_inside_edges			( coordinate_indices, m_mesh.vertices, edges );
	remove_close_contrdirectional_edges	( coordinate_indices, m_mesh.vertices, edges );
	fix_reciprocal_intersections		( coordinate_indices, m_mesh.vertices, edges );
	merge_consecutive_edges				( coordinate_indices, m_mesh.vertices, edges );
	remove_zero_edges					( coordinate_indices, m_mesh.vertices, edges, math::epsilon_3 );
	make_vertices_of_edges_unique_in_2d	( coordinate_indices, m_mesh.vertices, edges, math::epsilon_3 );
}

void bsp_tree_node::draw_outer_edges( render::scene_ptr const& scene, render::debug::renderer& r ) const
{
	math::color const edges_color( 255, 255, 0 );
	outer_edges_type::const_iterator const end_it = m_outer_edges.end();
	for ( outer_edges_type::const_iterator it = m_outer_edges.begin(); it != end_it; ++it )
	{
		//r.draw_line( scene, m_triangle_mesh.vertices[ it->vertex_index0 ], m_triangle_mesh.vertices[ it->vertex_index1 ], edges_color );
		//r.draw_sphere_solid( scene, m_triangle_mesh.vertices[ it->vertex_index0 ], 1e-2f, edges_color );
		r.draw_arrow(  scene, it->first, it->second, edges_color, edges_color );
	}
}

} // namespace model_editor
} // namespace xray