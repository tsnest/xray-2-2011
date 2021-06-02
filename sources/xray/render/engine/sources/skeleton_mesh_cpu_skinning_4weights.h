////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SKELETON_MESH_CPU_SKINNING_4WEIGHTS_H_INCLUDED
#define SKELETON_MESH_CPU_SKINNING_4WEIGHTS_H_INCLUDED

#include "render_model_skeleton.h"
#include <xray/render/core/untyped_buffer.h>

namespace xray {
namespace render {
struct vert_boned_4w;

class skeleton_mesh_cpu_skinning_4weights : public skeleton_render_surface {
	typedef skeleton_render_surface			super;
public:
					skeleton_mesh_cpu_skinning_4weights( );
	virtual			~skeleton_mesh_cpu_skinning_4weights( );
	virtual	void	load								( configs::binary_config_value const& properties, memory::chunk_reader& r );
	virtual	void	update								( render::vector<float4x4> const& bones );

private:
	void			skinning_sw							( render::vector<float4x4> const& bones );

private:
	vert_boned_4w*		m_vertices_4w;
	untyped_buffer_ptr	m_vertex_buffer;
}; // class skeleton_mesh_cpu_skinning_4weights

} // namespace render
} // namespace xray

#endif // #ifndef SKELETON_MESH_CPU_SKINNING_4WEIGHTS_H_INCLUDED