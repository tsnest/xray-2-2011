////////////////////////////////////////////////////////////////////////////
//	Created		: 07.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "solid_visual_writer.h"
#include <tootle/tootlelib.h>
#include "NvMender2003/nvMeshMender.h"

using namespace xray;

solid_visual_writer::solid_visual_writer( MArgDatabase const& arglist, MStatus& status )
:m_arglist(arglist)
{
	status = MStatus::kSuccess;
}


solid_visual_writer::~solid_visual_writer()
{
	render_surfaces_it it		= m_render_surfaces.begin();
	render_surfaces_it it_e		= m_render_surfaces.end();
	for( ;it!=it_e; ++it)
	{
		render_surface* s	= *it;
		DELETE				( s );
	}
}

int render_surface::find_vertex(float3 const& position, 
								float3 const& normal, 
								float2 const& uvset )
{
	float3vec::iterator itv		= m_vertices.begin();
	float3vec::iterator itv_e	= m_vertices.end();

	float3vec::iterator itn		= m_normals.begin();
	float2vec::iterator ituv	= m_uvs.begin();

	for(;itv!=itv_e; ++itv, ++itn, ++ituv)
	{
		float3 const& p		= *itv;
		float3 const& n		= *itn;
		float2 const& uv	= *ituv;
		if(p.similar(position) && n.similar(normal) && uv.similar(uvset))
			return std::distance( m_vertices.begin(), itv );
	}
	return -1;
}

void render_surface::add_vertex(	float3 const& pos, 
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
	}

	m_indices.push_back		( (u16)idx );
}

MStatus render_surface::prepare( MArgDatabase const& arglist )
{
	MStatus				result;
	result				= calculate_tb( arglist );
	CHK_STAT_R			( result );
	result				= optimize_uv( arglist );
	CHK_STAT_R			( result );
	result				= optimize_mesh( arglist );

	return				result;
}

MStatus render_surface::optimize_uv( MArgDatabase const& arglist )
{
	XRAY_UNREFERENCED_PARAMETERS( arglist );
    // Optimize texture coordinates
    // 1. Calc bounds
    float2 		Tdelta;
    float2 	Tmin, Tmax;
	Tmin.set	(math::float_max, math::float_max);
    Tmax.set	(math::float_min, math::float_min);

	float2vec::iterator it = m_uvs.begin();
	float2vec::iterator it_e = m_uvs.end();

	for(; it!=it_e; ++it)
	{
	  float2& uv = *it;
	  Tmin.min				(uv);
	  Tmax.max				(uv);
	}

    Tdelta.x 	= floorf((Tmax.x-Tmin.x)/2+Tmin.x);
    Tdelta.y 	= floorf((Tmax.y-Tmin.y)/2+Tmin.y);

	// 2. Recalc UV mapping
	it = m_uvs.begin();
	for(; it!=it_e; ++it)
	{
	  float2& uv = *it;
	  uv		-= Tdelta;
	}

	return MStatus::kSuccess;
}

void render_surface::fill_mender_input(vector< MeshMender::Vertex >& theVerts, u16vec& theIndices )
{
	theVerts.clear();
	theIndices.clear();
	// fill inputs ( verts )
	const u32 vertices_number	=  m_vertices.size();
	theVerts.resize				( vertices_number );

	float3vec::iterator itv		= m_vertices.begin();
	float3vec::iterator itv_e	= m_vertices.end();

	float3vec::iterator itn		= m_normals.begin();
	float2vec::iterator ituv	= m_uvs.begin();

	vector< MeshMender::Vertex >::iterator out_verts_it = theVerts.begin();

	for(;itv!=itv_e; ++itv, ++itn, ++ituv, ++out_verts_it)
	{
		MeshMender::Vertex& out_vertex	= *out_verts_it;

		cv_vector		( out_vertex.pos,		*itv );
		cv_vector		( out_vertex.normal,	*itn );
		out_vertex.s	= (*ituv).x;
		out_vertex.t	= (*ituv).y;
	}

	theIndices			= m_indices;
}

void render_surface::retrive_data_from_mender_output(	const vector< MeshMender::Vertex >&	theVerts,
								 				const u16vec& theIndices,
								 				const u16vec& mappingNewToOldVert )
{
	float3vec		old_vertices	= m_vertices;
	float3vec		old_normals		= m_normals;
	float2vec		old_uvs			= m_uvs;

	m_indices	= theIndices;

    {
      const u32 vertex_count = theVerts.size();
      m_vertices.clear	( ); 
      m_normals.clear	( ); 
      m_uvs.clear		( ); 
      m_tangents.clear	( ); 
      m_binormals.clear	( ); 

	  m_vertices.resize	( vertex_count );
      m_normals.resize	( vertex_count );
      m_uvs.resize		( vertex_count );
      m_tangents.resize	( vertex_count );
      m_binormals.resize( vertex_count );

      for (u32 i=0; i < vertex_count; ++i)
	  {
		  const MeshMender::Vertex& in_vertex = theVerts[i];
		
		  u32 old_idx		= mappingNewToOldVert[i];

		  m_vertices[i]	= old_vertices[old_idx];
		  m_normals[i]	= old_normals[old_idx];
		  m_uvs[i]		= old_uvs[old_idx];

		  cv_vector		( m_vertices[i], in_vertex.pos );
		  cv_vector		( m_normals[i], in_vertex.normal );
		  m_uvs[i]		= float2( in_vertex.s, in_vertex.t );
		  cv_vector	( m_tangents[i], in_vertex.tangent );
		  cv_vector	( m_binormals[i], in_vertex.binormal );
	  }
    }
}

MStatus render_surface::calculate_tb( MArgDatabase const& arglist )
{
	XRAY_UNREFERENCED_PARAMETERS( arglist );

  	vector<MeshMender::Vertex>	mender_in_out_verts;
	u16vec						mender_in_out_indices;
	u16vec						mender_mapping_out_to_in_vert;

	fill_mender_input( mender_in_out_verts, mender_in_out_indices );

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
		  MeshMender::RESPECT_SPLITS,
		  MeshMender::DONT_FIX_CYLINDRICAL
		)
	)
	{
		MGlobal::displayError( "solid_visual_exporter: NVMeshMender failed " );
		return MStatus::kFailure;
	}
	
	retrive_data_from_mender_output		( mender_in_out_verts, mender_in_out_indices, mender_mapping_out_to_in_vert  );
//	t_remove_isolated_verts				( m_vertices, m_indices );
//	check_isolated_verts				( );

	mender_in_out_verts.clear			( );
	mender_in_out_indices.clear			( );
	mender_mapping_out_to_in_vert.clear	( );
	return MStatus::kSuccess;
}

MStatus render_surface::optimize_mesh( MArgDatabase const& arglist )
{
	if(!arglist.isFlagSet("-t"))
		return MStatus::kSuccess;

	MString status_str;

	TootleOverdrawOptimizer overdraw_mode = arglist.isFlagSet("-hq") ? TOOTLE_OVERDRAW_AUTO : TOOTLE_OVERDRAW_FAST;

	status_str				= "tootle optimization started in "; 
	status_str				+= (overdraw_mode==TOOTLE_OVERDRAW_AUTO)? "High Quality Mode" : "Fast Mode";
	MGlobal::displayInfo	( status_str );
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
	if(res!=TOOTLE_OK)		MGlobal::displayError( "TootleMeasureCacheEfficiency failed" );

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
	if(res!=TOOTLE_OK)		MGlobal::displayError( "TootleMeasureOverdraw failed" );

	status_str = "Efficiency before :";
	status_str += "VCache-";
	status_str += cache_efficiency;
	status_str += " OverdrawAVG-";
	status_str += overdraw_avg;
	status_str += " OverdrawMAX-";
	status_str += overdraw_max;
	MGlobal::displayInfo( status_str );

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

	if(res!=TOOTLE_OK)		MGlobal::displayError( "TootleOptimize failed" );

	for(u32 i=0; i<m_indices.size(); ++i)
		m_indices[i] = (u16)u32indices[i];

	res = TootleMeasureCacheEfficiency(
		pib,
		nfaces,
		TOOTLE_DEFAULT_VCACHE_SIZE,
		&cache_efficiency
		);
	if(res!=TOOTLE_OK)		MGlobal::displayError( "TootleMeasureCacheEfficiency failed" );

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
	if(res!=TOOTLE_OK)		MGlobal::displayError( "TootleMeasureOverdraw failed" );

	status_str = "Efficiency after  :";
	status_str += "VCache-";
	status_str += cache_efficiency;
	status_str += " OverdrawAVG-";
	status_str += overdraw_avg;
	status_str += " OverdrawMAX-";
	status_str += overdraw_max;
	MGlobal::displayInfo( status_str );

	TootleCleanup	( );

	return (res==TOOTLE_OK) ? MStatus::kSuccess : MStatus::kFailure;
}

MStatus collision_surface::prepare( MArgDatabase const& arglist )
{
	XRAY_UNREFERENCED_PARAMETER( arglist );
	return MStatus::kSuccess;
}

void collision_surface::clear( )
{
	m_vertices.clear	( );
	m_indices.clear		( );
}

void collision_surface::add_vertex( float3 const& pos )
{
	float3vec::iterator itv		= m_vertices.begin();
	float3vec::iterator itv_e	= m_vertices.end();
	
	int idx = -1;

	for(; itv!=itv_e; ++itv)
	{
		float3 const& p		= *itv;
		if(p.similar(pos))
			idx = std::distance( m_vertices.begin(), itv );
	}
	if(idx==-1)
	{
		idx						= (u16)m_vertices.size();
		m_vertices.push_back	( pos );
	}
	m_indices.push_back		( (u16)idx );
}

