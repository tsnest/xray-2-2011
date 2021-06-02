////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "static_surface.h"
#include <amd/tootle/tootlelib.h>

void			fill_mender_input				(	float3vec const& in_vertices, 
												   u16vec const& in_indices,
												   float3vec const& in_normals,
												   float2vec const& in_uvs,
												   vector< MeshMender::Vertex >& theVerts, u16vec& theIndices );

namespace xray{
namespace maya{

render_static_surface::render_static_surface( u32 max_verts_count )
:m_max_verts_count( max_verts_count ),
m_bbox( math::create_invalid_aabb() )
{
	R_ASSERT			( max_verts_count< u16(-1) );
}

int render_static_surface::find_vertex(	float3 const& position, 
										float3 const& normal, 
										float2 const& uvset ) const
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
		float3 const& n		= m_normals[rec.vertex_id];
		float2 const& uv	= m_uvs[rec.vertex_id];
		
		if( p.is_similar(position) && n.is_similar(normal) && uv.is_similar(uvset) )
			return rec.vertex_id;
	}
	return -1;
}

int render_static_surface::find_vertex_test( float3 const& position, 
										float3 const& normal, 
										float3 const& tangent, 
										float3 const& binormal, 
										float2 const& uvset ) const
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
		float3 const& n		= m_normals[rec.vertex_id];
		float2 const& uv	= m_uvs[rec.vertex_id];
		float3 const& tn	= m_tangents[rec.vertex_id];
		float3 const& bn	= m_binormals[rec.vertex_id];
		
		if( p.is_similar(position) && 
			n.is_similar(normal) && 
			uv.is_similar(uvset) )
		{
			bool b_tn = tn.is_similar(tangent);
			bool b_bn = bn.is_similar(binormal);

			if(b_tn && b_bn)
				return rec.vertex_id;
		}
	}
	return -1;
}

void render_static_surface::add_vertex(	float3 const& pos, 
										float3 const& norm, 
										float2 const& uv )
{
	int idx = find_vertex( pos, norm, uv );

	if(-1==idx)
	{
		idx						= (u16)m_vertices.size();
		m_vertices.push_back	( pos );
		m_normals.push_back		( norm );
		m_uvs.push_back			( uv );
		
		lookup_item				rec;
		rec.square_magnitude	= pos.squared_length();
		rec.vertex_id			= (u16)idx;

		lookup_table::iterator it = std::lower_bound(m_lookup.begin(), m_lookup.end(), rec);
		m_lookup.insert			( it, rec );
		m_bbox.modify			( pos );
	}

	m_indices.push_back		( (u16)idx );
	if(m_vertices.size()>m_max_verts_count)
		set_full( );
	
	R_ASSERT(m_vertices.size() < u16(-1));
}

void render_static_surface::check_integrity( pcstr message ) const
{
	u32 count	= m_indices.size();
	
	for(u32 i=0; i<count; ++i)
	{
		u16 idx = m_indices[i];
		if(idx > m_vertices.size()-1 )
		{
		
			LOG_ERROR("render_static_surface::check_integrity failed %d %d", idx, m_vertices.size() );
			R_ASSERT( 0, message );
		}
	}
}
MStatus render_static_surface::prepare( MArgDatabase const& arglist )
{
	MStatus				result;
	result				= calculate_tb( arglist );

	CHK_STAT_R			( result );

	CHK_STAT_R			( result );

	result				= optimize_uv( m_uvs, arglist );
	CHK_STAT_R			( result );
	result				= optimize_mesh( arglist );

	return				result;
}

void render_static_surface::retrive_data_from_mender_output( const vector< MeshMender::Vertex >&	theVerts,
								 				const u16vec& theIndices,
								 				const u16vec& mappingNewToOldVert )
{
	XRAY_UNREFERENCED_PARAMETERS	( mappingNewToOldVert );
	float3vec		old_vertices	= m_vertices;
	float3vec		old_normals		= m_normals;
	float2vec		old_uvs			= m_uvs;

	m_indices	= theIndices;


	const u32 vertex_count = theVerts.size();
	m_vertices.clear		( ); 
	m_normals.clear			( ); 
	m_uvs.clear				( ); 
	m_tangents.clear		( ); 
	m_binormals.clear		( ); 

	m_vertices.resize		( vertex_count );
	m_normals.resize		( vertex_count );
	m_uvs.resize			( vertex_count );
	m_tangents.resize		( vertex_count );
	m_binormals.resize		( vertex_count );

	for (u32 i=0; i < vertex_count; ++i)
	{
		const MeshMender::Vertex& in_vertex = theVerts[i];

		m_vertices[i]	= in_vertex.pos;
		R_ASSERT		(valid(m_vertices[i]));

		m_normals[i]	= in_vertex.normal;
		R_ASSERT		(valid(m_normals[i]));

		m_uvs[i]		= float2( in_vertex.s, in_vertex.t );

		m_tangents[i]	= in_vertex.tangent;
		R_ASSERT			(valid(m_tangents[i]));
		m_binormals[i]	= in_vertex.binormal;
		R_ASSERT			(valid(m_binormals[i]));
	}
}



MStatus render_static_surface::calculate_tb( MArgDatabase const& arglist )
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
		  0.5,//60 deg
		  0.5,//60 deg
		  0.0f,
		  MeshMender::DONT_CALCULATE_NORMALS,
		  MeshMender::RESPECT_SPLITS
		)
	)
	{
		display_warning( "solid_visual_exporter: NVMeshMender failed " );
		return MStatus::kFailure;
	}

	if(mender_in_out_verts.size()>u16(-1))
	{
		LOG_ERROR("mender_in_out_verts.size() index overflow");
		return MStatus::kInsufficientMemory;
	}

	retrive_data_from_mender_output		( mender_in_out_verts, mender_in_out_indices, mender_mapping_out_to_in_vert  );
//	t_remove_isolated_verts				( m_vertices, m_indices );
//	check_isolated_verts				( );

	mender_in_out_verts.clear			( );
	mender_in_out_indices.clear			( );
	mender_mapping_out_to_in_vert.clear	( );

	fix_tangents_and_binormals			( );
	return								MStatus::kSuccess;
}

MStatus render_static_surface::optimize_mesh( MArgDatabase const& arglist )
{
	if(!arglist.isFlagSet("-t"))
		return MStatus::kSuccess;

	MString status_str;

	TootleOverdrawOptimizer overdraw_mode = arglist.isFlagSet("-hq") ? TOOTLE_OVERDRAW_AUTO : TOOTLE_OVERDRAW_FAST;

	status_str				= "tootle optimization started in "; 
	status_str				+= (overdraw_mode==TOOTLE_OVERDRAW_AUTO)? "High Quality Mode" : "Fast Mode";
	display_info	( status_str );
	TootleInit				( );
	void* pvb				= &m_vertices[0];
	u32vec					u32indices;
	u32indices.resize		(m_indices.size());

	for(u32 i=0; i<m_indices.size(); ++i)
		u32indices[i] = m_indices[i];

	u32* pib		= &u32indices[0];
	u32 nvertices	= m_vertices.size();
	u32 nfaces		= m_indices.size()/3;
	u32 num_cluster_out = 0;
	u32 vstride		= sizeof(float3);
	float			cache_efficiency;
	float			overdraw_avg;
	float			overdraw_max;
	TootleResult	res;

	res = TootleMeasureCacheEfficiency(
		pib,
		nfaces,
		TOOTLE_DEFAULT_VCACHE_SIZE,
		&cache_efficiency
		);
	if(res!=TOOTLE_OK)		display_warning( "TootleMeasureCacheEfficiency failed" );

	res = TootleMeasureOverdraw(
		pvb,
		pib,
		nvertices,
		nfaces,
		vstride,
		NULL,
		0,
		TOOTLE_CW,
		&overdraw_avg,
		&overdraw_max,
		TOOTLE_OVERDRAW_DIRECT3D
		);
	if(res!=TOOTLE_OK)		display_warning( "TootleMeasureOverdraw failed" );

	status_str = "Efficiency before :";
	status_str += "VCache-";
	status_str += cache_efficiency;
	status_str += " OverdrawAVG-";
	status_str += overdraw_avg;
	status_str += " OverdrawMAX-";
	status_str += overdraw_max;
	display_info( status_str );

	res = TootleOptimize	(
		pvb,
		pib,
		nvertices,
		nfaces,
		vstride,
		TOOTLE_DEFAULT_VCACHE_SIZE,
		NULL,
		0,
		TOOTLE_CW,
		pib,
		&num_cluster_out,
		TOOTLE_VCACHE_AUTO,
		overdraw_mode
		);

	if(res!=TOOTLE_OK)		display_warning( "TootleOptimize failed" );

	for(u32 i=0; i<m_indices.size(); ++i)
		m_indices[i] = (u16)u32indices[i];

	res = TootleMeasureCacheEfficiency(
		pib,
		nfaces,
		TOOTLE_DEFAULT_VCACHE_SIZE,
		&cache_efficiency
		);
	if(res!=TOOTLE_OK)		display_warning( "TootleMeasureCacheEfficiency failed" );

	res = TootleMeasureOverdraw(
		pvb,
		pib,
		nvertices,
		nfaces,
		vstride,
		NULL,
		0,
		TOOTLE_CW,
		&overdraw_avg,
		&overdraw_max,
		TOOTLE_OVERDRAW_DIRECT3D
		);
	if(res!=TOOTLE_OK)		display_warning( "TootleMeasureOverdraw failed" );

	status_str = "Efficiency after  :";
	status_str += "VCache-";
	status_str += cache_efficiency;
	status_str += " OverdrawAVG-";
	status_str += overdraw_avg;
	status_str += " OverdrawMAX-";
	status_str += overdraw_max;
	display_info( status_str );

	TootleCleanup	( );

	return (res==TOOTLE_OK) ? MStatus::kSuccess : MStatus::kFailure;
}


MStatus render_static_surface::save_properties( configs::lua_config_value& value ) const
{
	super::save_properties				( value );
	math::sphere sphere					= m_bbox.sphere( );

	value["type"]						= xray::render::mt_static_submesh;
	value["version"]					= 0;
	value["bounding_box"]["min"]		= m_bbox.min;
	value["bounding_box"]["max"]		= m_bbox.max;
	value["bounding_sphere"]["pos"]		= sphere.center;
	value["bounding_sphere"]["radius"]	= sphere.radius;
	
	return MStatus::kSuccess;
}

MStatus render_static_surface::save_vertices( memory::writer& F ) const
{
	u32 vertices_count	= m_vertices.size();

	F.write_u32			( vertices_count );

	xray::render::vert_static			v;
	for (u32 idx = 0; idx<vertices_count; ++idx)
	{
		v.P					= m_vertices[idx];
		v.N					= m_normals[idx];
		v.T					= m_tangents[idx];
		v.B					= m_binormals[idx];
		v.uv				= m_uvs[idx];
		F.write				( &v, sizeof(v) );
	}

	return MStatus::kSuccess;
}

} //namespace maya
} //namespace xray
