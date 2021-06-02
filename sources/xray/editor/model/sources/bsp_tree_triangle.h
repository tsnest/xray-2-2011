////////////////////////////////////////////////////////////////////////////
//	Created		: 23.08.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BSP_TREE_TRIANGLE_H_INCLUDED
#define BSP_TREE_TRIANGLE_H_INCLUDED
#include <xray\ai_navigation\sources\triangles_mesh.h>
#include "edge_utilities.h"
namespace xray {
namespace model_editor {

class bsp_tree_triangle {
public:
	static u32 const msc_empty_id = u32( -1 );
	static u32 const msc_neihbours_count = 3;
	static u32 const msc_children_count = 3;

	enum side { front, back, coplanar, spanning };
	math::plane	plane;
	u32		neighbours[3];
	u32		children[3];
	bool	divider;
	bool	is_marked;
	float	square;
	u32		line_id;
	union
	{
		u32 sector_id;
		u32 portal_id;
	};
	u32 parent;
public:
	bsp_tree_triangle():
	divider( false ),
	is_marked( false ),
	square( 0.0f ),
	sector_id( msc_empty_id ),
	parent( msc_empty_id ),
	line_id( u32( -1 ) )
	{
		for ( u32 i = 0; i < 3; ++i )
		{
			neighbours[i]	= msc_empty_id;
			children[i]		= msc_empty_id;
		}
	}
	
	explicit bsp_tree_triangle( math::plane const& p, u32 line ):
	plane( p ),
	divider( false ),
	is_marked( false ),
	square( 0.0f ),
	sector_id( msc_empty_id ),
	parent( msc_empty_id ),
	line_id( line )
	{
		for ( u32 i = 0; i < 3; ++i )
		{
			neighbours[i]	= msc_empty_id;
			children[i]		= msc_empty_id;
		}
	}

	bool is_coplanar( math::plane const& p, float normal_epsilon = math::epsilon_3 ) const
	{
		return math::is_similar( p.normal | plane.normal, 1.0f, normal_epsilon) && math::is_relatively_similar( p.d, plane.d, math::epsilon_3 );
	}
	bool has_children() const
	{
		return ( children[0] != msc_empty_id || children[1] != msc_empty_id || children[2] != msc_empty_id );
	}
	bool has_neighbors() const
	{
		return neighbours[ 0 ] != msc_empty_id || neighbours[ 1 ] != msc_empty_id || neighbours[ 2 ] != msc_empty_id;
	}
	bool is_neighbour_of( u32 neighbour_id ) const
	{
		R_ASSERT( neighbour_id != msc_empty_id );
		return neighbours[ 0 ] == neighbour_id || neighbours[ 1 ] == neighbour_id || neighbours[ 2 ] == neighbour_id;
	}
	//u32 empty_neighbours_count() const
	//{
	//	u32 result = 0;
	//	if ( neighbours[ 0 ] == msc_empty_id )
	//		++result;
	//	if ( neighbours[ 1 ] == msc_empty_id )
	//		++result;
	//	if ( neighbours[ 2 ] == msc_empty_id )
	//		++result;
	//	return result;
	//}
}; // class bsp_tree_triangle

struct adjacency_type
{
	adjacency_type()
	{
		neighbours[ 0 ] = neighbours[ 1 ] = neighbours[ 2 ] = bsp_tree_triangle::msc_empty_id;
	}
	u32 neighbours[ 3 ];
};


typedef ai::navigation::triangles_mesh< bsp_tree_triangle >	triangles_mesh_type;
typedef vector<u32> triangle_ids_type;   
bsp_tree_triangle::side		calculate_side				( math::plane const& p, u32 triangle_id, triangles_mesh_type const& mesh, float precision = math::epsilon_3 );
void						split_triangle				( math::plane const& p, u32 triangle_id, triangles_mesh_type& mesh, 
														  triangle_ids_type& positive_ids, triangle_ids_type& negative_ids, edge_to_u32_type* edges = NULL );
void						remove_duplicates			( triangles_mesh_type& triangles_mesh, bool remove_duplicate_triangles = true );
void						remove_duplicate_triangles	( triangles_mesh_type const& triangles_mesh, triangle_ids_type& triangles );

bool						is_convex_polygon_set		( triangle_ids_type const& triangle_ids, triangles_mesh_type const& mesh );
void						split_triangle_with_point	( u32 triangle_id, triangles_mesh_type& mesh, u32 old_neighbour,
														  u32 new_point_index, u32 new_neighbour0, u32 new_neighbour1 );
bool						is_triangle_adjacency_correct( u32 triangle_id, triangles_mesh_type const& mesh );
void						reset_triangle_adjacency	( u32 triangle_id, triangles_mesh_type& mesh );
void						clear_triangle_neighbours	( u32 triangle_id, triangles_mesh_type& mesh );
void						replace_neighbour_of_triangle( u32 triangle_id, triangles_mesh_type& mesh, u32 old_neighbour_id, u32 new_neighbour_id );

struct triangle_has_children : public std::unary_function<u32, bool>
{
	triangle_has_children( triangles_mesh_type const& mesh )
		:m_mesh( &mesh ){}
	bool operator()( u32 triangle_id ) const
	{
		return m_mesh->data[ triangle_id ].has_children();
	}
private:
	triangles_mesh_type const* m_mesh;
};


} // namespace model_editor
} // namespace xray

#endif // #ifndef BSP_TREE_TRIANGLE_H_INCLUDED