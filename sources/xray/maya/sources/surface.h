////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SURFACE_H_INCLUDED
#define SURFACE_H_INCLUDED

namespace xray{
namespace maya{

struct lookup_item
{
	float		square_magnitude;
	u16			vertex_id;

	inline bool operator < (lookup_item const& other)	const	{ return square_magnitude<other.square_magnitude; }
	inline bool operator < (float const& sqm)			const	{ return square_magnitude<sqm; }
};

typedef vector<lookup_item>		lookup_table;

struct surface :private boost::noncopyable
{
	bool				m_full_;

	MString				m_sg_name;
	MString				m_mesh_full_name;
	MDagPath			m_mesh_dag_path;
//	MString				m_material_name;
	MString				m_texture_name;
	
	u16vec				m_indices;
	float3vec			m_vertices;
	float3vec			m_normals;
	float3vec			m_tangents;
	float3vec			m_binormals;
	float2vec			m_uvs;

						surface			( );
	void				set_full		( );
	bool				empty			( ) const							{ return m_indices.empty(); }
	virtual MStatus		prepare			( MArgDatabase const& arglist )		= 0;
	virtual MStatus		save_properties	( configs::lua_config_value& value ) const;
	virtual MStatus		save_vertices	( memory::writer& writer ) const	= 0;
			MStatus		save_indices	( memory::writer& writer ) const;

			MString		save_name		( ) const;

	void				increment_add_idx( );
protected:
	void				fix_tangents_and_binormals( );

	int					m_full_idx;
	int					m_add_idx;
	lookup_table		m_lookup;
};

extern int g_uniq_surface_counter;

} //namespace maya
} //namespace xray

#endif // #ifndef SURFACE_H_INCLUDED