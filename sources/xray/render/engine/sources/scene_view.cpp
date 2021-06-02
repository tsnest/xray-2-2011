////////////////////////////////////////////////////////////////////////////
//	Created		: 03.09.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "scene_view.h"

#if XRAY_PLATFORM_WINDOWS
#	include <xray/render/core/res_texture.h>
#	include "material_manager.h"
#endif // #if XRAY_PLATFORM_WINDOWS

namespace xray {
namespace render {

scene_view::scene_view():
	m_view_mode(lit_view_mode),
	m_particles_render_mode(xray::particle::normal_particle_render_mode),
	m_use_post_process(true),
	m_render_frame_index(0)
{
	m_prev_frame_luminance_parameters	= math::float4(0.0f, 0.0f, 0.0f, 0.0f);
	m_frame_luminance_parameters		= math::float4(0.0f, 0.0f, 0.0f, 0.0f);
}

scene_view::~scene_view()
{
// #if XRAY_PLATFORM_WINDOWS
// 	material_manager::ref().remove_post_process_parameters(&m_post_process_parameters);
// #endif // #if XRAY_PLATFORM_WINDOWS
}

} // namespace render
} // namespace xray


