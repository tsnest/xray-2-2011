////////////////////////////////////////////////////////////////////////////
//	Created		: 07.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SOLID_VISUAL_COLLECTOR_H_INCLUDED
#define SOLID_VISUAL_COLLECTOR_H_INCLUDED

#include "static_surface.h"
#include "model_collector.h"

namespace xray{
namespace maya{


class solid_visual_collector : public model_collector
{
public:
				solid_visual_collector				( MArgDatabase const& arglist, MStatus& status, u32 max_verts_count );
				~solid_visual_collector				( );

	virtual u32	get_surfaces_count					( )			const;
	virtual surface const*					get_surface			( u32 idx ) const	{ return m_render_static_surfaces[idx]; }
	geometry_collector const&	get_collision_surface( ) const			{ return m_collision_surface; }

	void		clear								( );

	MStatus		build_collision_geometry_by_render	( );

	MStatus		extract_render_static_geometry_from_path( MDagPath& fnDagPath );
	MStatus		extract_collision_geometry_from_path( MDagPath& fnDagPath );

	MStatus		prepare_surfaces					( );
	MStatus		write_properties					( configs::lua_config_value& value );
	
	void		dump_statistic						( bool brief );
	bool		is_collision_from_geometry			( ) const { return m_collision_from_geometry; }
	void		check_integrity						( pcstr message ) const;
	void		set_locator_matrix					( MMatrix const& m ){m_locator_matrix_inv = m.inverse(); }
private:
	MStatus		extract_render_static_geometry		( MDagPath& fnMeshDagPath );
	MStatus		extract_collision_geometry			( MDagPath& fnMeshDagPath );

	MArgDatabase const&			m_arglist;
	xray::math::aabb			m_bbox;
	bool						m_collision_from_geometry;
	u32							m_max_verts_count;
	typedef vector<render_static_surface*>			render_static_surfaces;
	typedef render_static_surfaces::iterator		render_static_surfaces_it;
	typedef render_static_surfaces::const_iterator	render_static_surfaces_cit;
	render_static_surfaces							m_render_static_surfaces;

	geometry_collector					m_collision_surface;

	MMatrix								m_locator_matrix_inv;
}; // class solid_visual_collector

} //namespace maya
} //namespace xray

#endif // #ifndef SOLID_VISUAL_COLLECTOR_H_INCLUDED