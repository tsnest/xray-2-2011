////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MODEL_COLLECTOR_H_INCLUDED
#define MODEL_COLLECTOR_H_INCLUDED

namespace xray {
namespace maya {

struct surface;
struct locator_item
{
	MString		name;
	MString		bone_name;
	int			bone_idx;
	float3		position;
	float3		rotation;
};

typedef vector<locator_item>	locators_vec;

class model_collector : private boost::noncopyable
{
public:
	virtual surface const*		get_surface			( u32 idx ) const	=0;
	virtual u32					get_surfaces_count	( )			const	=0;
	MStatus						find_surface		( MDagPath const& fnMeshDagPath, MString const& sg_name, u32& surf_idx, bool use_dag_path );
	MStatus						extract_locators	( MString const& locators_root_path, MMatrix const& locator_matrix );
	MStatus						write_locators		( configs::lua_config_value& value );

	locators_vec								m_locators;
}; // class model_collector

struct geometry_collector_internal;

class geometry_collector
{
	xray::math::aabb					m_bbox;

public:
				geometry_collector	( );
				~geometry_collector	( );
	void		add_vertex			( float3 const& pos );

	bool		save_vertices		( memory::writer& F ) const;
	bool		save_indices		( memory::writer& F ) const;
	void		clear				( );
	u32			vertices_count		( ) const;
	u32			indices_count		( ) const;
private:
	geometry_collector_internal*	m_data;
};

} // namespace maya
} // namespace xray

#endif // #ifndef MODEL_COLLECTOR_H_INCLUDED