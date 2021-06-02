////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SKINNED_SURFACE_H_INCLUDED
#define SKINNED_SURFACE_H_INCLUDED

#include "surface.h"
#include "NvMender2003/nvMeshMender.h"

namespace xray{
namespace maya{

#define max_influence (4)

struct sk_bone
{
	MString		m_name;

	MDagPath	m_path;

	float3		m_bind_translation;
	float3		m_bind_rotation;
	float3		m_bind_scale;

	MStatus		save		( xray::memory::writer& w ) const;
};

struct bone_influence
{
    u16		bone_index; // "in m_bones" container of skin_visual_writer
    float   weight;
};

struct bone_influences
{
	bone_influence	influence_arr	[max_influence];
	void			normalize		( u16 weighted_bone_count );
};

typedef xray::maya::vector<bone_influences>	influences_vec;

struct mesh_influences
{
	MDagPath			m_shape_dag_path_;
	MString				m_shape_string_path;
	influences_vec		m_all_influences;
};

typedef xray::maya::vector<mesh_influences*>	mesh_influences_vec;
typedef xray::maya::vector<sk_bone>				bones_vec;

struct render_skinned_surface : public surface
{
	typedef surface super;

	u16vec				m_influences_idx;	// index in m_all_influences container
											//m_vertices.size()==m_influences_idx.size()
	mesh_influences*	m_influences_;

	void		add_vertex		(	float3 const& position, 
									float3 const& normal, 
									float2 const& uvset,
									u16 infl_id );

	virtual MStatus		prepare			( MArgDatabase const& arglist );
	virtual MStatus		save_properties	( configs::lua_config_value& value ) const;
	virtual MStatus		save_vertices	( memory::writer& writer ) const;

private:
	int			find_vertex		(	float3 const& position, 
									float3 const& normal, 
									float2 const& uvset,
									u16 infl_id ) const;

	MStatus		write_4w_vertices	( memory::writer& w ) const;
	MStatus		write_3w_vertices	( memory::writer& w ) const;
	MStatus		write_2w_vertices	( memory::writer& w ) const;
	MStatus		write_1w_vertices	( memory::writer& w ) const;

	MStatus		optimize_mesh	( MArgDatabase const& arglist );

	MStatus		calculate_tb	( MArgDatabase const& arglist );
	void		retrive_data_from_mender_output	( const vector< MeshMender::Vertex >& theVerts,
									 			const u16vec& theIndices,
									 			const u16vec& mappingNewToOldVert );
};

} //namespace maya
} //namespace xray

#endif // #ifndef SKINNED_SURFACE_H_INCLUDED