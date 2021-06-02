////////////////////////////////////////////////////////////////////////////
//	Created		: 07.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "solid_visual_collector.h"

namespace xray{
namespace maya{

solid_visual_collector::solid_visual_collector( MArgDatabase const& arglist, MStatus& status, u32 max_verts_count )
:m_arglist					( arglist ),
m_collision_from_geometry	( false ),
m_max_verts_count			( max_verts_count ),
m_bbox						( math::create_zero_aabb() )
{
	m_locator_matrix_inv	= MMatrix::identity;
	status = MStatus::kSuccess;
}

solid_visual_collector::~solid_visual_collector()
{
	clear			( );
}

void solid_visual_collector::clear()
{
	render_static_surfaces_it it		= m_render_static_surfaces.begin();
	render_static_surfaces_it it_e		= m_render_static_surfaces.end();
	for( ;it!=it_e; ++it)
	{
		render_static_surface* s	= *it;
		DELETE				( s );
	}
	m_render_static_surfaces.clear		( );
	m_collision_surface.clear	( );
}

void solid_visual_collector::dump_statistic( bool brief )
{
	MString info;
	u32 scount = m_render_static_surfaces.size();
	if(!brief)
	{
		for(u32 si=0; si<scount; ++si)
		{	
			render_static_surface*	s			= m_render_static_surfaces[si];

			info			= "Render surface [";
			info			+= si;
			info			+= "] MayaSG:";
			info			+= s->m_sg_name;
			info			+= " ";
			info			+= s->m_indices.size()/3;
			info			+= " triangles (";
			info			+= s->m_vertices.size();
			info			+= " vertices, ";
			info			+= s->m_indices.size();
			info			+= " indices) tex[";
			info			+= s->m_texture_name;
			info			+= "]";
			display_info( info );
		}
	}

	u32 t=0;
	u32 v=0;
	u32 i=0;
	for(u32 si=0; si<scount; ++si)
	{	
		render_static_surface*	s			= m_render_static_surfaces[si];
		v		+= s->m_vertices.size();
		i		+= s->m_indices.size();
	}

	t = i/3;
	info			= "Render: ";
	info			+= scount;
	info			+= " surface(s) ";
	info			+= t;
	info			+= " triangles, ";
	info			+= v;
	info			+= " vertices, ";
	info			+= i;
	info			+= " indices";
	display_info( info );

	info			= "Collision: ";
	if(m_collision_from_geometry)
	{
		info			+= "(by render geometry) ";
	}else
	{
		info			+= "(custom) ";
	}

	info			+= m_collision_surface.indices_count()/3;
	info			+= " triangles, ";
	info			+= m_collision_surface.vertices_count();
	info			+= " vertices, ";
	info			+= m_collision_surface.indices_count();
	info			+= " indices";
	display_info( info );
	
}

void solid_visual_collector::check_integrity( pcstr message ) const
{
	render_static_surfaces_cit it		= m_render_static_surfaces.begin();
	render_static_surfaces_cit it_e		= m_render_static_surfaces.end();
	for(; it!=it_e; ++it)
	{
		render_static_surface* s		= *it;
		s->check_integrity				( message );
	}
}

} //namespace maya
} //namespace xray
