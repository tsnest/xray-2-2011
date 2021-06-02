////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "surface.h"

namespace xray{
namespace maya{

int g_uniq_surface_counter	= 0;

surface::surface( )
:m_full_(false),
m_add_idx(0)
{
	m_texture_name = "notset";
}

MStatus	surface::save_indices( memory::writer& w ) const
{
	u32 indices_count		= m_indices.size( ); 
    w.write_u32				( indices_count );
    w.write					( &m_indices.front(), indices_count * sizeof(u16) );

	return MStatus::kSuccess;
}

void surface::set_full( )
{
	if(m_full_)
		return;

	m_full_		= true;
	m_full_idx	= g_uniq_surface_counter;
}

void surface::increment_add_idx( )
{
	m_add_idx = ++g_uniq_surface_counter;
}

MStatus surface::save_properties( configs::lua_config_value& value )  const
{
	value["source"]["mesh_name"]		= m_mesh_dag_path.fullPathName().asChar();
	value["source"]["vertices"]			= m_vertices.size();
	value["source"]["indices"]			= m_indices.size();
	value["source"]["primitives"]		= (u32)(m_indices.size()/3);
	
	value["source"]["texture_name"]		= m_texture_name.asChar();

	return MStatus::kSuccess;
}


MString surface::save_name( ) const
{
	MString result = m_sg_name;

	if(result.numChars()>60)
		result	= result.substring(0, 60);

	if(m_full_)
	{
		result	+= "_";
		result	+= m_full_idx;
	}
	if(m_add_idx)
	{
		result	+= "_";
		result	+= m_add_idx;
	}

	fixed_string512 str = result.asChar();
	str.replace(":", "_");
	str.replace("/", "_");
	str.replace("|", "_");
	str.replace("\\", "_");

	return MString(str.c_str());
}


void surface::fix_tangents_and_binormals( )
{
	m_lookup.clear();
	u32 count = m_vertices.size();
	m_lookup.resize(count);

	for(u32 i=0; i<count; ++i)
	{
		lookup_item& itm		= m_lookup[i];
		itm.vertex_id			= (u16)i;
		itm.square_magnitude	= m_vertices[i].squared_length();
	}
	std::sort(m_lookup.begin(), m_lookup.end() );
	
	float3vec tangents_new		= m_tangents;
	float3vec binormals_new		= m_binormals;
	tangents_new.resize			( count );
	binormals_new.resize		( count );
	
	u16vec nearest;

	for(u32 i=0; i<count; ++i)
	{
		nearest.clear				( );
		lookup_item const& itm		= m_lookup[i];
		u16 vertex_id				= itm.vertex_id;

		lookup_table::iterator it_lb = std::lower_bound(m_lookup.begin(), m_lookup.end(), itm.square_magnitude-math::epsilon_5 );
		while( it_lb<m_lookup.end() )
		{
			float3& v0 = m_vertices[vertex_id];
			float3& v1 = m_vertices[it_lb->vertex_id];

			float3& n0 = m_normals[vertex_id];
			float3& n1 = m_normals[it_lb->vertex_id];

			float2& uv0 = m_uvs[vertex_id];
			float2& uv1 = m_uvs[it_lb->vertex_id];

			if(	it_lb->vertex_id!=vertex_id && 
				n0.is_similar((n1), math::epsilon_3) &&
				v0.is_similar(v1) &&
				uv0.is_similar(uv1) 
				)
				nearest.push_back		(it_lb->vertex_id);

			++it_lb;
			if( (*it_lb).square_magnitude > itm.square_magnitude+math::epsilon_5)
				break;
		}

		u32 const num_near_vertices			= nearest.size();
		if (!num_near_vertices)
			continue;

		float const alpha = 1.0f / (num_near_vertices + 1.0f);
		
		float3& tangent		= tangents_new[vertex_id];
		float3& binormal	= binormals_new[vertex_id];
		for(u32 n=0; n<num_near_vertices; ++n )
		{
			u16 nearest_vertex_id = nearest[n];

			binormal	= binormal * (1.0f - alpha) + m_binormals[nearest_vertex_id] * alpha;
			tangent		= tangent * (1.0f - alpha) + m_tangents[nearest_vertex_id] * alpha;
		}
	}

	m_tangents.swap(tangents_new);
	m_binormals.swap(binormals_new);

}
} //namespace maya
} //namespace xray
