////////////////////////////////////////////////////////////////////////////
//	Created		: 07.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SOLID_VISUAL_WRITER_H_INCLUDED
#define SOLID_VISUAL_WRITER_H_INCLUDED

#include <xray/ogf.h>
#include "NvMender2003/nvMeshMender.h"

typedef vector<u16>			u16vec;
typedef vector<u32>			u32vec;
typedef vector<float3>		float3vec;
typedef vector<float2>		float2vec;

struct render_surface
{
	MString						m_shader_name;
	MString						m_texture_name;
	u16vec						m_indices;

	float3vec					m_vertices;
	float3vec					m_normals;
	float3vec					m_tangents;
	float3vec					m_binormals;
	float2vec					m_uvs;
	xray::math::aabb			m_bbox;

	MStatus		calculate_tb	( MArgDatabase const& arglist );
	MStatus		optimize_uv		( MArgDatabase const& arglist );
	MStatus		optimize_mesh	( MArgDatabase const& arglist );

	int			find_vertex		(	float3 const& pos, 
									float3 const& norm, 
									float2 const& uv );

	void		add_vertex		(	float3 const& pos, 
									float3 const& norm, 
									float2 const& uv );

	MStatus	save				( xray::memory::writer& F );
	MStatus	prepare				( MArgDatabase const& arglist );

private:
	void	fill_mender_input	(vector< MeshMender::Vertex >& theVerts, u16vec& theIndices );

	void	retrive_data_from_mender_output(	const vector< MeshMender::Vertex >&	theVerts,
									 		const u16vec&				theIndices,
									 		const u16vec&				mappingNewToOldVert );
};

struct collision_surface
{
	u32vec						m_indices;
	float3vec					m_vertices;
	xray::math::aabb			m_bbox;

	void		add_vertex		(	float3 const& pos );

	MStatus		save			( xray::memory::writer& F );
	MStatus		prepare			( MArgDatabase const& arglist );
	void		clear			( );
};

class solid_visual_writer :public boost::noncopyable
{
public:
				solid_visual_writer	( MArgDatabase const& arglist, MStatus& status );
				~solid_visual_writer( );

	MStatus		extract_render_geometry				( MDagPath& fnDagPath );
	MStatus		build_collision_geometry_by_render	( );
	MStatus		extract_collision_geometry			( MDagPath& fnDagPath );
	MStatus		write_render_geometry				( xray::memory::writer& w );
	MStatus		write_collision_geometry			( xray::memory::writer& w );

private:
	MObject				findShader		( const MObject& setNode, MFnMesh& fnMesh, MStatus& status ) ;
	MStatus				shader_props	( const MObject& shaderNode, MString& shader_name, MString& texture_name, bool& doublesided ) ;
	//for storing DAG objects
	MArgDatabase const&	m_arglist;
	xray::math::aabb	m_bbox;
	
	typedef xray::maya::vector<render_surface*>	render_surfaces;
	typedef render_surfaces::iterator			render_surfaces_it;
	render_surfaces								m_render_surfaces;
	collision_surface							m_collision_surface;
}; // class solid_visual_writer

#endif // #ifndef SOLID_VISUAL_WRITER_H_INCLUDED