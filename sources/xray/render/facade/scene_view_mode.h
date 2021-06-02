////////////////////////////////////////////////////////////////////////////
//	Created		: 11.01.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_BASE_SCENE_VIEW_MODE_H_INCLUDED
#define XRAY_RENDER_BASE_SCENE_VIEW_MODE_H_INCLUDED

namespace xray {
namespace render {

enum scene_view_mode
{
	wireframe_view_mode			= 0,
	wireframe_two_sided_view_mode,
	
	unlit_view_mode,
	lit_view_mode,
	
	normals_view_mode,
	
	specular_intencity_view_mode,
	specular_power_view_mode,
	
	miplevel_view_mode,
	
	lighting_view_mode,
	lighting_diffuse_view_mode,
	lighting_specular_view_mode,
	
	shader_complexity_view_mode,
	texture_density_view_mode,
	geometry_complexity_view_mode,
	
	emissive_only_view_mode,
	
	distortion_only_view_mode,
	
	ambient_occlusion_only_view_mode,
	unlit_with_ao_view_mode,
	
	lit_with_histogram_view_mode,
	
	overdraw_view_mode,
	
	indirect_lighting_mode,
	
	num_view_modes,
}; // enum scene_view_mode

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_BASE_SCENE_VIEW_MODE_H_INCLUDED