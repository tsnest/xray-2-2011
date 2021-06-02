////////////////////////////////////////////////////////////////////////////
//	Created		: 30.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "renderer.h"
#include <xray/render/core/effect_manager.h>

// Effect descriptors
#include "effect_forward_system.h"
#include "effect_fstage_default_materials.h"
#include "effect_lighting_stage_default_materials.h"
#include "effect_lighting_stage_skin_base_materials.h"
#include "effect_fstage_fire_fresnel_materials.h"
#include "effect_fstage_soft_materials.h"
#include "effect_fstage_volume_sphere_base_materials.h"
#include "effect_fstage_volume_cone_base_materials.h"
#include "effect_distortion_stage_default_materials.h"
#include "effect_distortion_stage_panner_materials.h"
#include "effect_gstage_default_materials.h"
#include "effect_fstage_blend_subuv_materials.h"
#include "effect_gbuffer_nomaterial_materials.h"
#include "effect_post_process_blend_texture_materials.h"
#include "effect_post_process_distortion_materials.h"
#include "effect_post_process_terrain_debug_materials.h"
#include "effect_gstage_burning_wood_materials.h"
#include "effect_fstage_smoke_custom_lighted_materials.h"
#include "effect_debug_editor_wireframe.h"
#include "depth_accumulate_material_effect.h"
#include "decal_default_material_effect.h"
#include "effect_lighting_stage_organic_base_materials.h"
#include "sky_default_material_effect.h"

// This file is temporary to find a correct place for effect descriptor initialization.

namespace xray {
namespace render {

void register_effect_descriptors()
{
	effect_manager::ref().register_effect_desctiptor( "#forward_system",				NEW(effect_forward_system));
	effect_manager::ref().register_effect_desctiptor("forward_fire_fresnel",			NEW(effect_fstage_fire_fresnel_materials));
	effect_manager::ref().register_effect_desctiptor("forward_default",					NEW(effect_fstage_default_materials));
	effect_manager::ref().register_effect_desctiptor("forward_default_tiled",			NEW(effect_fstage_default_materials));
	effect_manager::ref().register_effect_desctiptor("forward_default_subuv",			NEW(effect_fstage_blend_subuv_materials));
	effect_manager::ref().register_effect_desctiptor("forward_soft",					NEW(effect_fstage_soft_materials));
	effect_manager::ref().register_effect_desctiptor("lighting_default",				NEW(effect_lighting_stage_default_materials));
	effect_manager::ref().register_effect_desctiptor("lighting_organic_base",			NEW(effect_lighting_stage_organic_base_materials));
	effect_manager::ref().register_effect_desctiptor("lighting_skin_base",				NEW(effect_lighting_stage_skin_base_materials));
	effect_manager::ref().register_effect_desctiptor("forward_volume_sphere_base",		NEW(effect_fstage_volume_sphere_base_materials));
	effect_manager::ref().register_effect_desctiptor("forward_volume_cone_base",		NEW(effect_fstage_volume_cone_base_materials));
	effect_manager::ref().register_effect_desctiptor("forward_smoke_custom_lighted",	NEW(effect_fstage_smoke_custom_lighted_materials));
	effect_manager::ref().register_effect_desctiptor("distortion_panner",				NEW(effect_distortion_stage_panner_materials));
	effect_manager::ref().register_effect_desctiptor("distortion_default",				NEW(effect_distortion_stage_default_materials));
	effect_manager::ref().register_effect_desctiptor("g_stage_default",					NEW(effect_gstage_default_materials));
	effect_manager::ref().register_effect_desctiptor("g_stage_default_tiled",			NEW(effect_gstage_default_materials));
	effect_manager::ref().register_effect_desctiptor("g_stage_burning_wood",			NEW(effect_gstage_burning_wood_materials));
	effect_manager::ref().register_effect_desctiptor("post_process_blend_texture",		NEW(effect_post_process_blend_texture_materials));
	effect_manager::ref().register_effect_desctiptor("post_process_distortion",			NEW(effect_post_process_distortion_materials));
	effect_manager::ref().register_effect_desctiptor("debug_post_process_terrain_debug",NEW(effect_post_process_terrain_debug_materials));
	effect_manager::ref().register_effect_desctiptor("debug_editor_wireframe",			NEW(effect_debug_editor_wireframe));
	effect_manager::ref().register_effect_desctiptor("shadow_accumulate",				NEW(depth_accumulate_material_effect));
	effect_manager::ref().register_effect_desctiptor("decals_default",					NEW(decal_default_material_effect)(false));
	effect_manager::ref().register_effect_desctiptor("forward_decal",					NEW(decal_default_material_effect)(true));
	effect_manager::ref().register_effect_desctiptor("sky_default",						NEW(sky_default_material_effect));
}


} // namespace render 
} // namespace xray 
