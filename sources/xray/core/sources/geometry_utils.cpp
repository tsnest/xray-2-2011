////////////////////////////////////////////////////////////////////////////
//	Created		: 09.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/geometry_utils.h>
#include <xray/geometry_primitives.h>
#include <xray/linkage_helper.h>

XRAY_DECLARE_LINKAGE_ID(core_geometry_utils)

using xray::math::float4x4;
using xray::math::float3;

namespace xray {
namespace geometry_utils{

void create_primitive(	geom_vertices_type& avertices, 
						 geom_indices_type& aindices, 
						 float4x4 transform, 
						 float const* vertices,  
						 u32 vertex_count, 
						 u16 const* faces, 
						 u32 index_count )
{
	float3 tmp_vertex;
	avertices.resize( vertex_count );
	for( u32 i = 0, j = 0 ; i < vertex_count; ++i, j+=3 )
	{
		tmp_vertex.x = vertices[j];
		tmp_vertex.y = vertices[j+1];
		tmp_vertex.z = vertices[j+2];

		avertices[i] = transform.transform_position( tmp_vertex );
	}

	aindices.resize( index_count );
	for( u32 i = 0; i < index_count; ++i)
		aindices[i] = faces[i];
}

bool create_torus( geom_vertices_type& vertices, 
					geom_indices_type& indices, 
					float4x4 transform,
					float outer_raduius,  
					float inner_raduius, 
					u16 outer_segments, 
					u16 inner_segments )
{
	return generate_torus( vertices, indices, transform, outer_raduius, inner_raduius, outer_segments, inner_segments );
}

bool create_cylinder( geom_vertices_type& vertices, geom_indices_type& indices, float4x4 transform, float3 size )
{
	 create_primitive	( vertices, 
							indices, 
							create_scale(size)*transform, 
							cylinder_solid::vertices,  
							cylinder_solid::vertex_count, 
							cylinder_solid::faces, 
							cylinder_solid::index_count );
	 return true;
}

bool create_cone( geom_vertices_type& vertices, xray::vectora< u16 >& indices, float4x4 transform, float3 size )
{
	create_primitive	( vertices, 
		indices, 
		create_scale(size)*transform, 
		cone_solid::vertices,  
		cone_solid::vertex_count, 
		cone_solid::faces, 
		cone_solid::index_count );
	return true;
}


bool create_cube( geom_vertices_type& vertices, geom_indices_type& indices, float4x4 transform, float3 size )
{
	create_primitive	( vertices, 
		indices, 
		create_scale(size)*transform, 
		cube_solid::vertices,  
		cube_solid::vertex_count, 
		cube_solid::faces, 
		cube_solid::index_count );
	return true;
}

bool create_ellipsoid( geom_vertices_type& vertices, geom_indices_type& indices, float4x4 transform, float3 size )
{
	create_primitive	( vertices, 
		indices, 
		create_scale(size)*transform, 
		ellipsoid_solid::vertices,  
		ellipsoid_solid::vertex_count, 
		ellipsoid_solid::faces, 
		ellipsoid_solid::index_count );

	return true;
}

bool create_octahedron( geom_vertices_type& vertices, geom_indices_type& indices, float4x4 transform, float3 size )
{
	create_primitive( vertices, 
		indices, 
		create_scale(size)*transform, 
		octahedron_solid::vertices,  
		octahedron_solid::vertex_count, 
		octahedron_solid::faces, 
		octahedron_solid::index_count );

	return true;
}

bool create_ring( geom_vertices_type& vertices, 
					geom_indices_type& indices, 
					float inner_radius, 
					float width,
					u16 segments_count)
{
	ASSERT					(segments_count>3);
	
	vertices.resize			(segments_count*2);
	indices.resize			(segments_count*6);

	float segment_ang		= math::pi_x2/segments_count;
	u16 i					= 0;
	u32 vidx				= 0;
	float outer_radius		= inner_radius+width;
	for(i=0; i<segments_count;++i)
	{
		math::sine_cosine	sincos(segment_ang*i);
		vertices[vidx++].set(inner_radius*sincos.cosine, 0.0f, -inner_radius*sincos.sine);
		vertices[vidx++].set(outer_radius*sincos.cosine, 0.0f, -outer_radius*sincos.sine);
	}

	u32 iidx				= 0;
	for(i=0; i<segments_count-1; ++i)
	{
		indices[iidx++]		= (i*2);
		indices[iidx++]		= (i*2+1);
		indices[iidx++]		= (i*2+3);

		indices[iidx++]		= (i*2);
		indices[iidx++]		= (i*2+3);
		indices[iidx++]		= (i*2+2);
	}
	{ // last segment
		indices[iidx++]		= (i*2);
		indices[iidx++]		= (i*2+1);
		indices[iidx++]		= (1);

		indices[iidx++]		= (i*2);
		indices[iidx++]		= (1);
		indices[iidx++]		= (0);
	}
	return true;
}

geometry_collector::geometry_collector( memory::base_allocator& a)
:m_vertices(a),
m_indices(a),
m_lookup(a)
{}

int geometry_collector::find_vertex( float3 const& position ) const
{
	float const sqm	= position.squared_length();

	lookup_table::const_iterator it		= std::lower_bound(m_lookup.begin(), m_lookup.end(), sqm);
	lookup_table::const_iterator it_e	= m_lookup.end();
	for( ; it!=it_e; ++it)
	{
		lookup_item const& rec	= *it;
		if(rec.square_magnitude > sqm+0.1f)
			break;

		float3 const& p		= m_vertices[rec.vertex_id];
		
		if(p.is_similar(position))
			return rec.vertex_id;
	}
	return -1;
}


void geometry_collector::add_vertex(	float3 const& pos ) 
{
	int idx = find_vertex( pos );

	if(-1==idx)
	{
		idx						= m_vertices.size();
		m_vertices.push_back	( pos );
		
		lookup_item				rec;
		rec.square_magnitude	= pos.squared_length();
		rec.vertex_id			= idx;

		lookup_table::iterator it = std::lower_bound(m_lookup.begin(), m_lookup.end(), rec);
		m_lookup.insert			( it, rec );
	}

	m_indices.push_back			( idx );
}

void geometry_collector::add_triangle( float3 const& p0, float3 const& p1, float3 const& p2 )
{
	add_vertex(	p0 ) ;
	add_vertex(	p1 ) ;
	add_vertex(	p2 ) ;
}

bool geometry_collector::write_obj_file( pcstr fn, float const scale )
{
	using namespace fs_new;
	synchronous_device_interface const & device	=	resources::get_synchronous_device();
	
	file_type*	f;
	
	if(!device->open(&f, fn, file_mode::create_always, file_access::write, assert_on_fail_false))
	{
		LOG_INFO					("unable to open file [%s]", fn);
		return false;
	}

	vertices_type::iterator it		= m_vertices.begin();
	vertices_type::iterator it_e	= m_vertices.end();
	string1024						buff;
	for(; it!=it_e; ++it)
	{
		float3&	v	= *it;
		sprintf_s	( buff, "v %f %f %f\n", v.x*scale, v.y*scale, v.z*scale );
		device->write	( f, buff, strings::length(buff) );
	}

	for(u32 idx=0; idx<m_indices.size();)
	{
		sprintf_s		( buff, "f %d %d %d\n", 1+ (m_indices[idx]), 1+ (m_indices[idx+1]), 1+ (m_indices[idx+2])  );
		device->write	( f, buff, strings::length(buff) );

		idx		+=3;
	}

	device->close(f);

	return true;
}

} //namespace geometry_utils
} //namespace xray
