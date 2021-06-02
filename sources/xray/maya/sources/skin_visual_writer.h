////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SKIN_VISUAL_WRITER_H_INCLUDED
#define SKIN_VISUAL_WRITER_H_INCLUDED

#include "skinned_surface.h"
#include "model_collector.h"
#include <xray/animation/skeleton.h>

namespace xray{
namespace maya{


class skin_visual_collector :public model_collector
{
public:
						skin_visual_collector	( MArgDatabase const& arglist, MStatus& status );
						~skin_visual_collector	( );
	
	virtual u32			get_surfaces_count		( )			const;
	virtual surface const* get_surface			( u32 idx ) const	{ return m_render_skinned_surfaces[idx];}

	MStatus				export_skin_visual		( MString const& skin_name );
	MStatus				write_properties		( configs::lua_config_value& value );
	MStatus				write_skeleton_properties( configs::lua_config_value& value );
	MStatus				extract_render_skinned_geometry	( MDagPath& fnMeshDagPath );
	MStatus				write_bones				( memory::writer& w ) const;

	MString						m_skeleton_name;
	animation::skeleton_ptr		m_skeleton;

private:

	void				clear					( );
	MStatus				extract_bone_map		( );
	MStatus				remap_bones				( u16 const* indices );
	MStatus				goto_bind_pose			( );
	void				on_skeleton_loaded		( resources::queries_result& resource );

	MArgDatabase const&			m_arglist;
	bones_vec					m_bones;
	mesh_influences_vec			m_mesh_influences_vec;
	MObjectArray				m_skin_cluster_objects;

	typedef vector<render_skinned_surface*>		render_skinned_surfaces;
	typedef render_skinned_surfaces::iterator	render_skinned_surfaces_it;
	
	render_skinned_surfaces						m_render_skinned_surfaces;
}; // class skin_visual_writer


} //namespace maya
} //namespace xray

#endif // #ifndef SKIN_VISUAL_WRITER_H_INCLUDED