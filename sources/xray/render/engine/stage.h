////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_H_INCLUDED
#define STAGE_H_INCLUDED

#include "scene_context.h"
#include <xray/render/core/backend.h>
#include <xray/render/core/pix_event_wrapper.h>

namespace xray {
namespace render_dx10 {

// This will temporary live here 
const u32 stencil_material_mask			= 0x01;
const u32 stencil_light_marker_start	= 0x05;


class stage : boost::noncopyable
{
public:
	stage(scene_context* context);

	// not sure if this need to live here
	static void rm_near		();
	static void rm_normal	();
	static void rm_far		();

	void set_default_rt	();
//	static void set_render_target	(ref_rt _1, ref_rt _2, ref_rt _3, ref_rt zb);
	
	// This need to be moved some utility class or namespace
	void u_compute_texgen_screen(float4x4& m_Texgen);
	void u_compute_texgen_jitter(float4x4& m_Texgen_J);

protected:
	scene_context*	m_context;
}; // class stage

} // namespace render
} // namespace xray

#endif // #ifndef STAGE_H_INCLUDED