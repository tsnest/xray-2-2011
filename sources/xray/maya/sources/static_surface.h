////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef STATIC_SURFACE_H_INCLUDED
#define STATIC_SURFACE_H_INCLUDED

#include "surface.h"
#include "NvMender2003/nvMeshMender.h"

namespace xray{
namespace maya{

struct render_static_surface :public surface
{
	typedef surface		super;

							render_static_surface	( u32 max_verts_count );
	math::aabb const&		get_bbox				( )			{ return m_bbox; }
	void					add_vertex				(	float3 const& pos, 
														float3 const& norm, 
														float2 const& uv );

	virtual MStatus			prepare					( MArgDatabase const& arglist );
	virtual MStatus			save_properties			( configs::lua_config_value& value ) const;
	virtual MStatus			save_vertices			( memory::writer& writer ) const;

	void					check_integrity			( pcstr message ) const;
private:
	math::aabb				m_bbox;
	
	MStatus					calculate_tb			( MArgDatabase const& arglist );
	MStatus					optimize_mesh			( MArgDatabase const& arglist );
	int						find_vertex				(	float3 const& pos, 
														float3 const& norm, 
														float2 const& uv ) const;

	int						find_vertex_test		(	float3 const& pos, 
														float3 const& norm, 
														float3 const& tangent, 
														float3 const& binormal, 
														float2 const& uv ) const;

	void	retrive_data_from_mender_output	( const vector< MeshMender::Vertex >& theVerts,
									 			const u16vec& theIndices,
									 			const u16vec& mappingNewToOldVert );
	u32 m_max_verts_count;
};

} //namespace maya
} //namespace xray

#endif // #ifndef STATIC_SURFACE_H_INCLUDED