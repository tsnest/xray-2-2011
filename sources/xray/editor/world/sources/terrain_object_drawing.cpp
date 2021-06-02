////////////////////////////////////////////////////////////////////////////
//	Created		: 16.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_object.h"
#include "terrain_quad.h"
#include <xray/render/facade/editor_renderer.h>

namespace xray {
namespace editor {

void terrain_node::render()
{

	// debug drawing, will be replaced with object-visual drawing later
	//super::render	();

	//if(!m_vertices || !draw_wireframe)
	//	return;

	//typedef xray::vectora< xray::render::vertex_colored >	editor_vertices_type;
	//typedef xray::vectora< u16 >								editor_indices_type;

	//editor_vertices_type				render_vertices(g_allocator);
	//u32 vert_count				= (m_dimension.Width+1)*(m_dimension.Height+1); 
	//render_vertices.resize		(vert_count);

	//editor_indices_type			render_indices(g_allocator);
	//u32 max_idx_count			= (m_dimension.Width)*(m_dimension.Height) * 3; 
	//max_idx_count				+= m_dimension.Height;
	//max_idx_count				+= m_dimension.Width;
	//max_idx_count				*= 2; // line described as 2 indices

	//render_indices.resize		(max_idx_count);

	//for(u32 vindex=0; vindex<vert_count; ++vindex)
	//{
	//	terrain_vertex& v				= m_vertices[vindex];
	//	render_vertices[vindex].position= v.vposition * *m_transform;
	//	render_vertices[vindex].color	= math::color_xrgb(255u, 0u, 0u);
	//}

	//terrain_quad						quad;
	//u16 quad_count						= u16(m_dimension.Height * m_dimension.Width);
	//u32 iindex							= 0;
	//for(u16 quad_index=0; quad_index<quad_count; ++quad_index)
	//{
	//	bool res = get_quad				(quad, (u16)quad_index);
	//	R_ASSERT_U (res);
	//	quad.export_lines				(render_indices, iindex);
	//}

	//ASSERT								(iindex==max_idx_count);

	//get_editor_renderer().draw_lines(render_vertices, render_indices);

	//if(m_selected)
	//{
	//}
}

} // namespace editor
} // namespace xray