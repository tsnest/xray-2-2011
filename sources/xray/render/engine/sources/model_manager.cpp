////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "model_manager.h"
#include <xray/render/engine/model_format.h>
#include "render_model_static.h"
#include "terrain_render_model.h"
#include "skeleton_mesh_gpu_skinning_4weights.h"
#include "render_model_user.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/custom_config.h>

namespace xray {
namespace render {
namespace model_factory {

void destroy_render_model( render_model* v )
{
	DELETE( v );
}

void destroy_render_surface( render_surface* v )
{
	DELETE( v );
}

u16 get_model_type(memory::chunk_reader& chunk)
{
	memory::reader	ogf = chunk.open_reader( model_chunk_header );
	model_header		header;
	ogf.r			(&header, sizeof(header), sizeof(header));
	return			header.type;
}

u16 get_surface_type(memory::chunk_reader& chunk)
{
	memory::reader	ogf = chunk.open_reader( model_chunk_header );
	model_header		header;
	ogf.r			(&header, sizeof(header), sizeof(header));
	return			header.type;
}

render_surface* create_render_surface(u16 type)
{
	render_surface* result	= NULL;
	switch(type)
	{
	case mt_static_submesh:
		result = NEW(static_render_surface)();
		break;

	case mt_skinned_submesh_4w:
#if 1
//		result	= NEW(skeleton_mesh_cpu_skinning_4weights)();
		result	= NEW(skeleton_mesh_gpu_skinning_4weights)();
#else
		result	= NEW(skeleton_submesh_4w_hw)();
#endif
		break;
	case mt_user_mesh_editable:
		result = NEW(user_render_surface_editable)();
		break;
	case mt_user_mesh_wire:
		result = NEW(user_render_surface_wire)();
		break;

	default:
		NODEFAULT();
	};
	return result;
}

render_model* create_render_model(u16 type)
{
	render_model* result	= NULL;
	switch(type)
	{
	case mt_static_mesh:
		result = NEW(static_render_model)();
		break;

	case mt_terrain_cell:
		result = NEW(terrain_render_model)();
		break;

	case mt_skinned_mesh:
		result = NEW(skeleton_render_model)();
		break;
	default:
		NODEFAULT();
	};

	return result;
}

} // namespace model_factory
} // namespace render 
} // namespace xray