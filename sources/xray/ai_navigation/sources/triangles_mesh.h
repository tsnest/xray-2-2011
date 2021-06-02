////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TRIANGLES_MESH_H_INCLUDED
#define TRIANGLES_MESH_H_INCLUDED

#include <xray/collision/api.h>
#include <xray/ai_navigation/api.h>

#ifndef MASTER_GOLD

namespace xray {
namespace ai {
namespace navigation {

template < typename T >
class triangles_mesh {
private:
	class is_marked_predicate {
	public:
		inline			is_marked_predicate	( triangles_mesh& triangles_mesh ) :
			m_triangles_mesh	( &triangles_mesh )
		{
		}

		inline	bool	operator()			( u32 const& vertex_id ) const
		{
			size_t const vertex_id_index	= &vertex_id - &*m_triangles_mesh->indices.begin( );
			return							m_triangles_mesh->data[ vertex_id_index / 3 ].is_marked;
		}

		inline	bool	operator()			( T const& triangle ) const
		{
			return triangle.is_marked;
		}

	private:
		triangles_mesh* m_triangles_mesh;
	}; // class is_marked_predicate

public:
	inline			triangles_mesh		( ) :
		spatial_tree( 0 ),
		m_save_marked		( false )
	{
	}
	
	inline			~triangles_mesh		( )
	{
		if ( spatial_tree )
			destroy_spatial_tree( );
	}

	inline	void	build_spatial_tree	( )
	{
		if ( spatial_tree )
			destroy_spatial_tree( );

		u32 const index_count	= indices.size( );
		R_ASSERT_CMP			( index_count % 3, ==, 0 );
		spatial_tree			= 
			&*collision::new_triangle_mesh_geometry(
				&debug::g_mt_allocator,
				&*vertices.begin(),
				vertices.size(),
				&*indices.begin(),
				index_count
			);
	}

	inline	void	build_spatial_tree	( u32 index_count )
	{
		if ( spatial_tree )
			destroy_spatial_tree( );

		R_ASSERT_CMP			( index_count % 3, ==, 0 );
		spatial_tree			= 
			&*collision::new_triangle_mesh_geometry(
				&debug::g_mt_allocator,
				&*vertices.begin(),
				vertices.size(),
				&*indices.begin(),
				index_count
			);
	}


	inline	void	destroy_spatial_tree( )
	{
		if ( spatial_tree ) {
			collision::delete_geometry	( &debug::g_mt_allocator, spatial_tree );
			spatial_tree				= 0;
		}
	}

	inline void		remove_marked_triangles ( ) 
	{
		if ( m_save_marked )
			return;

		indices.erase		(
			std::remove_if(
				indices.begin( ),
				indices.end( ),
				is_marked_predicate( (*this) )
			),
			indices.end( )
		);

		data.erase			(
			std::remove_if(
				data.begin( ),
				data.end( ),
				is_marked_predicate( *this )
			),
			data.end( )
		);
	}

	inline void set_save_marked( bool flag ) { m_save_marked = flag; }
public:
	typedef xray::debug::vector< float3 >	vertices_type;
	typedef xray::debug::vector< u32 >		indices_type;
	typedef xray::debug::vector< T >		data_type;

public:
	vertices_type			vertices;
	indices_type			indices;
	data_type				data;
	collision::geometry*	spatial_tree;
	bool					m_save_marked;
}; // class triangles_mesh

} // namespace navigation
} // namespace ai
} // namespace xray

#endif //#ifndef MASTER_GOLD

#endif // #ifndef TRIANGLES_MESH_H_INCLUDED