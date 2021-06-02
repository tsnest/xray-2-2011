////////////////////////////////////////////////////////////////////////////
//	Created		: 20.09.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "spatial_sector.h"
#pragma managed(push, off)
#include <xray/render/facade/debug_renderer.h>
#pragma managed(pop)
#include "triangles_mesh_utilities.h"

namespace xray {
namespace model_editor {

bool spatial_sector::ms_solid_sectors = true;

spatial_sector::spatial_sector():
m_color( 0, 0, 0 ),
m_line_color( 0, 0, 0 ),
m_mesh( NULL ),
m_square( 0.0f ),
m_id( static_cast<u32>( -1 ) )
{
}

spatial_sector::spatial_sector( triangles_mesh_type& mesh, math::color const& c, u32 sector_id ):
m_color( c ),
m_line_color( c.r, c.g, c.b ),
m_mesh( &mesh ),
m_square( 0.0f ),
m_id( sector_id )
{
}

void spatial_sector::add_triangle( u32 triangle_id )
{
	m_triangles.push_back( triangle_id );
	m_square += m_mesh->data[ triangle_id ].square;
}

void spatial_sector::add_portal( u32 portal_id )
{
	typedef std::pair<portal_ids_type::iterator, portal_ids_type::iterator> portal_ids_iterator_pair;
	portal_ids_iterator_pair it_pair = std::equal_range( m_portal_ids.begin(), m_portal_ids.end(), portal_id );
	if ( it_pair.first == it_pair.second )
		m_portal_ids.insert( it_pair.second, portal_id );
}

void spatial_sector::render( render::scene_ptr const& scene, render::debug::renderer& r ) const
{

	triangle_ids_type::const_iterator const end_it = m_triangles.end();
	for ( triangle_ids_type::const_iterator it = m_triangles.begin(); it != end_it; ++it )
	{
		const u32 triangle_id = *it;
		math::float3 const& first	= m_mesh->vertices[ m_mesh->indices[ triangle_id * 3 ] ];
		math::float3 const& second	= m_mesh->vertices[ m_mesh->indices[ triangle_id * 3 + 1 ] ];
		math::float3 const& third	= m_mesh->vertices[ m_mesh->indices[ triangle_id * 3 + 2 ] ];

		if ( ms_solid_sectors )
			r.draw_triangle( scene, first, second, third, m_color );
		r.draw_line( scene, first,	second, m_line_color );
		r.draw_line( scene, second, third,	m_line_color );
		r.draw_line( scene, third,	first,	m_line_color );
	}
}

math::float3 spatial_sector::get_mass_center() const
{
	return get_triangles_mass_center( m_triangles, *m_mesh );
}

void spatial_sector::merge( spatial_sector& other )
{
	R_ASSERT( other.m_portal_ids.empty() );
	triangle_ids_type::const_iterator const othe_triangles_end = other.m_triangles.end();
	for ( triangle_ids_type::const_iterator it = other.m_triangles.begin(); it != othe_triangles_end; ++it )
	{
		m_mesh->data[ *it ].sector_id = this->get_id();
	}
	m_triangles.reserve( m_triangles.size() + other.m_triangles.size() );
	m_triangles.insert( m_triangles.end(), other.m_triangles.begin(), other.m_triangles.end() );
	m_square += other.m_square;

	other.m_triangles.clear();
	other.m_portal_ids.clear();
}

} // namespace model_editor
} // namespace xray
