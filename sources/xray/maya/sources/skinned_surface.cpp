////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "skin_visual_writer.h"

void			fill_mender_input				(	float3vec const& in_vertices, 
												   u16vec const& in_indices,
												   float3vec const& in_normals,
												   float2vec const& in_uvs,
												   vector< MeshMender::Vertex >& theVerts, u16vec& theIndices );

namespace xray{
namespace maya{

MStatus sk_bone::save( memory::writer& w ) const
{
	MStatus				result;
	w.write_string		( m_name.asChar() );
	w.write_float3		( m_bind_translation );
	w.write_float3		( m_bind_rotation );
	w.write_float3		( m_bind_scale );

	result				= MStatus::kSuccess;
	return				result;
}

void bone_influences::normalize( u16 weighted_bone_count )
{
	float sum		= 0.0f;
	for(u16 idx=0; idx<max_influence; ++idx)
	{
		if(idx<weighted_bone_count)
		{
			sum			+= influence_arr[idx].weight;
		}else
		{
			influence_arr[idx].bone_index	= u16(-1); //influence_arr[0].bone_index
			influence_arr[idx].weight		= 0.0f;
		}
	}
	
	float d = 1.0f/sum;

	for(u16 idx=0; idx<weighted_bone_count; ++idx)
		influence_arr[idx].weight	*= d;
}

int render_skinned_surface::find_vertex(float3 const& position, 
										float3 const& normal, 
										float2 const& uvset,
										u16 infl_id) const
{
	float const sqm	= position.squared_length();

	lookup_table::const_iterator it		= std::lower_bound(m_lookup.begin(), m_lookup.end(), sqm);
	lookup_table::const_iterator it_e	= m_lookup.end();
	for( ; it!=it_e; ++it)
	{
		lookup_item const& rec	= *it;
		if(rec.square_magnitude > sqm+0.1f)
			break;

		float3 const& p			= m_vertices[rec.vertex_id];
		float3 const& n			= m_normals[rec.vertex_id];
		float2 const& uv		= m_uvs[rec.vertex_id];
		u16 const influence_id	= m_influences_idx[rec.vertex_id];

		if(p.is_similar(position) && n.is_similar(normal) && uv.is_similar(uvset))
		{
			R_ASSERT			( influence_id==infl_id );
			return				rec.vertex_id;
		}
	}
	return -1;
}

void render_skinned_surface::add_vertex(	float3 const& position, 
											float3 const& normal, 
											float2 const& uvset,
											u16 infl_id )
{
	int idx = find_vertex( position, normal, uvset, infl_id );
	if(-1==idx)
	{
		idx								= (u16)m_vertices.size();
		m_vertices.push_back			( position );
		m_influences_idx.push_back		( infl_id );
		m_normals.push_back				( normal );
		m_uvs.push_back					( uvset );

		lookup_item					rec;
		rec.square_magnitude		= position.squared_length();
		rec.vertex_id				= (u16)idx;

		lookup_table::iterator it	= std::lower_bound(m_lookup.begin(), m_lookup.end(), rec);
		m_lookup.insert				( it, rec );
	}

	m_indices.push_back		( (u16)idx );
}

MStatus render_skinned_surface::prepare( MArgDatabase const& arglist )
{
	MStatus				result;
	if(m_vertices.size() > u16(-1))
	{
		display_warning("Surface vertex count > 65535, cant fit it into 16Bit index");
		result				= MStatus::kFailure;
		CHK_STAT_R			( result );
	}
	result				= calculate_tb	( arglist );
	CHK_STAT_R			( result );
	result				= optimize_uv	( m_uvs, arglist );
//	CHK_STAT_R			( result );
//	result				= optimize_mesh( arglist );

	return				result;
}

MStatus render_skinned_surface::calculate_tb( MArgDatabase const& arglist )
{
	XRAY_UNREFERENCED_PARAMETERS( arglist );

  	vector<MeshMender::Vertex>	mender_in_out_verts;
	u16vec						mender_in_out_indices;
	u16vec						mender_mapping_out_to_in_vert;

	fill_mender_input(	m_vertices, 
						m_indices, 
						m_normals, 
						m_uvs, 
						mender_in_out_verts, 
						mender_in_out_indices );

	MeshMender	mender	;
	if
	( 
		!mender.Mend		
		(
		  mender_in_out_verts,
		  mender_in_out_indices,
		  mender_mapping_out_to_in_vert,
		  1,
		  0.5,
		  0.5,
		  0.0f,
		  MeshMender::DONT_CALCULATE_NORMALS,
		  MeshMender::RESPECT_SPLITS
		)
	)
	{
		display_warning( "render_skinned_surface: NVMeshMender failed " );
		return MStatus::kFailure;
	}
	
	retrive_data_from_mender_output		( mender_in_out_verts, mender_in_out_indices, mender_mapping_out_to_in_vert  );

	mender_in_out_verts.clear			( );
	mender_in_out_indices.clear			( );
	mender_mapping_out_to_in_vert.clear	( );

	fix_tangents_and_binormals			( );

	return MStatus::kSuccess;
}

void render_skinned_surface::retrive_data_from_mender_output(	const vector< MeshMender::Vertex >&	theVerts,
								 				const u16vec& theIndices,
								 				const u16vec& mappingNewToOldVert )
{
	float3vec		old_vertices	= m_vertices;
	float3vec		old_normals		= m_normals;
	float2vec		old_uvs			= m_uvs;
	u16vec			old_infls		= m_influences_idx;

	m_indices	= theIndices;

    {
      const u32 vertex_count	= theVerts.size();
      m_vertices.clear			( ); 
      m_normals.clear			( ); 
      m_uvs.clear				( ); 
      m_tangents.clear			( ); 
      m_binormals.clear			( ); 
	  m_influences_idx.clear	( );

	  m_vertices.resize			( vertex_count );
      m_normals.resize			( vertex_count );
      m_uvs.resize				( vertex_count );
      m_tangents.resize			( vertex_count );
      m_binormals.resize		( vertex_count );
	  m_influences_idx.resize	( vertex_count );

      for (u32 i=0; i < vertex_count; ++i)
	  {
		  const MeshMender::Vertex& in_vertex = theVerts[i];
		
		  u32 old_idx		= mappingNewToOldVert[i];

		  m_vertices[i]			= old_vertices	[ old_idx ];
		  m_normals[i]			= old_normals	[ old_idx ];
		  m_uvs[i]				= old_uvs		[ old_idx ];
		  m_influences_idx[i]	= old_infls		[ old_idx ];

			m_vertices[i]		= in_vertex.pos;
			R_ASSERT			(valid(m_vertices[i]));
			m_normals[i]		= in_vertex.normal;
			R_ASSERT			(valid(m_normals[i]));
		  m_uvs[i]		= float2( in_vertex.s, in_vertex.t );

		  m_tangents[i]			= in_vertex.tangent;
		R_ASSERT(valid(m_tangents[i]));

		  m_binormals[i]		= in_vertex.binormal;
			R_ASSERT(valid(m_binormals[i]));
	  }
    }
}

MStatus render_skinned_surface::save_properties( configs::lua_config_value& value )  const
{
	super::save_properties	( value );

	math::aabb tmp_bbox		=  math::create_identity_aabb() ;

	math::sphere sphere		= tmp_bbox.sphere( );

	switch (max_influence)
	{
		case 1: 	
			value["type"]		= xray::render::mt_skinned_submesh_1w; break;
		case 2:
			value["type"]		= xray::render::mt_skinned_submesh_2w; break;
		case 3:
			value["type"]		= xray::render::mt_skinned_submesh_3w; break;
		case 4:
			value["type"]		= xray::render::mt_skinned_submesh_4w; break;
		default:
			NODEFAULT();
	}

	value["version"]							= 0;
	value["bounding_box"]["min"]				= tmp_bbox.min;
	value["bounding_box"]["max"]				= tmp_bbox.max;
	value["bounding_sphere"]["pos"]				= sphere.center;
	value["bounding_sphere"]["radius"]			= sphere.radius;
	
	return MStatus::kSuccess;
}

MStatus	render_skinned_surface::save_vertices( memory::writer& w ) const
{
	w.write_u32			( m_vertices.size() );

	switch (max_influence)
	{
		case 1: 	
			write_1w_vertices( w );
		break;
		case 2:
			write_2w_vertices( w );
		break;
		case 3:
			write_3w_vertices( w );
		break;
		case 4:
			write_4w_vertices( w );
		break;
	}
	return MStatus::kSuccess;
}

MStatus render_skinned_surface::write_4w_vertices( memory::writer& w ) const
{
	xray::render::vert_boned_4w		vertex;
	u32 vcount			= m_vertices.size();
	for(u32 vidx=0; vidx<vcount; ++vidx)
	{
		vertex.invalidate			();
		vertex.P					= m_vertices[vidx];
		vertex.N					= m_normals[vidx];
		vertex.T					= m_tangents[vidx];
		vertex.B					= m_binormals[vidx];
		vertex.uv					= m_uvs[vidx];
		
		u16 infl_idx				= m_influences_idx[vidx];
		bone_influences const& bi	= m_influences_->m_all_influences[infl_idx];
		vertex.m[0]					= bi.influence_arr[0].bone_index;
		vertex.m[1]					= bi.influence_arr[1].bone_index;
		vertex.m[2]					= bi.influence_arr[2].bone_index;
		vertex.m[3]					= bi.influence_arr[3].bone_index;
		
		vertex.w[0]					= bi.influence_arr[0].weight;
		vertex.w[1]					= bi.influence_arr[1].weight;
		vertex.w[2]					= bi.influence_arr[2].weight;

		w.write						( &vertex, sizeof(vertex) );
	}
	return MStatus::kSuccess;
}

MStatus render_skinned_surface::write_3w_vertices( memory::writer& w ) const
{
	xray::render::vert_boned_3w		vertex;
	u32 vcount			= m_vertices.size();
	for(u32 vidx=0; vidx<vcount; ++vidx)
	{
		vertex.invalidate			();
		vertex.P					= m_vertices[vidx];
		vertex.N					= m_normals[vidx];
		vertex.T					= m_tangents[vidx];
		vertex.B					= m_binormals[vidx];
		vertex.uv					= m_uvs[vidx];
		
		u16 infl_idx				= m_influences_idx[vidx];
		bone_influences const& bi	= m_influences_->m_all_influences[infl_idx];
		vertex.m[0]					= bi.influence_arr[0].bone_index;
		vertex.m[1]					= bi.influence_arr[1].bone_index;
		vertex.m[2]					= bi.influence_arr[2].bone_index;
		
		vertex.w[0]					= bi.influence_arr[0].weight;
		vertex.w[1]					= bi.influence_arr[1].weight;

		w.write						( &vertex, sizeof(vertex) );
	}
	return MStatus::kSuccess;
}

MStatus render_skinned_surface::write_2w_vertices( memory::writer& w ) const
{
	xray::render::vert_boned_2w		vertex;
	u32 vcount			= m_vertices.size();
	for(u32 vidx=0; vidx<vcount; ++vidx)
	{
		vertex.invalidate			();
		vertex.P					= m_vertices[vidx];
		vertex.N					= m_normals[vidx];
		vertex.T					= m_tangents[vidx];
		vertex.B					= m_binormals[vidx];
		vertex.uv					= m_uvs[vidx];
		
		u16 infl_idx				= m_influences_idx[vidx];
		bone_influences const& bi	= m_influences_->m_all_influences[infl_idx];
		vertex.m[0]					= bi.influence_arr[0].bone_index;
		vertex.m[1]					= bi.influence_arr[1].bone_index;
		
		vertex.w					= bi.influence_arr[0].weight;

		w.write						( &vertex, sizeof(vertex) );
	}
	return MStatus::kSuccess;
}

MStatus render_skinned_surface::write_1w_vertices( memory::writer& w ) const
{
	xray::render::vert_boned_1w		vertex;
	u32 vcount			= m_vertices.size();
	for(u32 vidx=0; vidx<vcount; ++vidx)
	{
		vertex.invalidate			();
		vertex.P					= m_vertices[vidx];
		vertex.N					= m_normals[vidx];
		vertex.T					= m_tangents[vidx];
		vertex.B					= m_binormals[vidx];
		vertex.uv					= m_uvs[vidx];
		
		u16 infl_idx				= m_influences_idx[vidx];
		bone_influences const& bi	= m_influences_->m_all_influences[infl_idx];
		vertex.m					= bi.influence_arr[0].bone_index;

		w.write						( &vertex, sizeof(vertex) );
	}
	return MStatus::kSuccess;
}

} //namespace maya
} //namespace xray
