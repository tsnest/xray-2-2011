////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SKELETON_MESH_GPU_SKINNING_4WEIGHTS_H_INCLUDED
#define SKELETON_MESH_GPU_SKINNING_4WEIGHTS_H_INCLUDED

#include "render_model_skeleton.h"
#include <xray/render/core/untyped_buffer.h>

namespace xray {
namespace render {

class skeleton_mesh_gpu_skinning_4weights : public skeleton_render_surface {
	typedef skeleton_render_surface			super;
public:
					skeleton_mesh_gpu_skinning_4weights	( );
	virtual			~skeleton_mesh_gpu_skinning_4weights( );
	virtual	void	load						( configs::binary_config_value const& properties, memory::chunk_reader& r );
	virtual	void	update						( render::vector<float4x4> const& bones );

private:
	untyped_buffer_ptr	m_vertex_buffer;
}; // class skeleton_mesh_gpu_skinning_4weights

} // namespace render
} // namespace xray

#endif // #ifndef SKELETON_MESH_GPU_SKINNING_4WEIGHTS_H_INCLUDED