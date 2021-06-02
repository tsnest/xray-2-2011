////////////////////////////////////////////////////////////////////////////
//	Created		: 20.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SHARED_NAMES_H_INCLUDED
#define SHARED_NAMES_H_INCLUDED

#define		r2_t_null			"$user$null"			// NULL texture
#define		r2_rt_depth			"$user$depth"			
#define		r2_rt_p				"$user$position"		// mrt
#define		r2_rt_p_ex			"$user$position_ex"		// mrt
#define		r2_rt_n				"$user$normal"			// mrt
#define		r2_rt_albedo		"$user$albedo"			// mrt
//

#define		r2_rt_tangents		"$user$tangents"

//// other
//#define		r2_rt_accum			"$user$accum"			// ---	16 bit fp or 16 bit fx
#define		r2_rt_accum_diffuse	"$user$accum_diffuse"	// 
#define		r2_rt_accum_specular "$user$accum_specular"	// 
//#define		r2_rt_accum_temp	"$user$accum_temp"		// ---	16 bit fp - only for hw which doesn't feature fp16 blend
//
#define		r2_t_envs0			"$user$env_s0"			// ---
//#define		r2_t_envs1			"$user$env_s1"			// ---
//
//#define		r2_t_sky0			"$user$sky0" 
//#define		r2_t_sky1			"$user$sky1" 
//
//
#define		r2_rt_present		"$user$present"			// ---
#define		r2_rt_generic0		"$user$generic0"		// ---
#define		r2_rt_generic1		"$user$generic1"		// ---
//#define		r2_rt_generic2		"$user$generic2"		// ---	//	igor: for volumetric lights
//
#define		r2_rt_blur0			"$user$blur0"			// ---
#define		r2_rt_blur1			"$user$blur1"			// ---

#define		r2_rt_decals_diffuse	"$user$decals_diffuse"		// ---
#define		r2_rt_decals_normal		"$user$decals_normal"		// ---


#define		r2_rt_gbuffer_position_downsampled		"$user$gbuffer_position_downsampled"		// ---
#define		r2_rt_gbuffer_normal_downsampled		"$user$gbuffer_normal_downsampled"		// ---

#define		r2_rt_indirect_lighting		"$user$indirect_lighting"		// ---

//
#define		r2_rt_emissive		"$user$emmisive"		// ---
//
#define		r2_rt_distortion	"$user$distortion"		// ---
//
#define		r2_rt_ssao_accumulator			"$user$ssao_accumulator" //
#define		r2_rt_ssao_accumulator_small	"$user$ssao_accumulator_small" //

#define		r2_rt_frame_luminance	"$user$frame_luminance"		// ---
#define		r2_rt_frame_luminance_previous	"$user$frame_luminance_previous"		// ---
#define		r2_rt_frame_luminance_histogram	"$user$frame_luminance_histogram"		// ---

#define		r2_rt_skin_scattering_stretch	"$user$skin_scattering_stretch"		// ---
#define		r2_rt_skin_scattering_position	"$user$skin_scattering_position"		// ---
#define		r2_rt_skin_scattering_temp	"$user$skin_scattering_temp"		// ---
#define		r2_rt_skin_scattering	"$user$skin_scattering"		// ---
#define		r2_rt_skin_scattering_small	"$user$skin_scattering_small"		// ---
#define		r2_rt_skin_scattering_blurred_0	"$user$skin_scattering_blurred_0"		// ---
#define		r2_rt_skin_scattering_blurred_1	"$user$skin_scattering_blurred_1"		// ---
#define		r2_rt_skin_scattering_blurred_2	"$user$skin_scattering_blurred_2"		// ---
#define		r2_rt_skin_scattering_blurred_3	"$user$skin_scattering_blurred_3"		// ---
#define		r2_rt_skin_scattering_blurred_4	"$user$skin_scattering_blurred_4"		// ---

#define		r2_rt_light_scattering	"$user$light_scattering"

#define		r2_rt_mlaa_edges			"$user$mlaa_edges"
#define		r2_rt_mlaa_blended_weights	"$user$mlaa_blended_weights"


// RSM
#define		r2_rt_lpv_rsm_albedo_source		"$user$lpv_rsm_albedo_source"
#define		r2_rt_lpv_rsm_normal_source		"$user$lpv_rsm_normal_source"
#define		r2_rt_lpv_rsm_position_source	"$user$lpv_rsm_position_source"

// RSM downsampled
#define		r2_rt_lpv_rsm_albedo			"$user$lpv_rsm_albedo"
#define		r2_rt_lpv_rsm_normal			"$user$lpv_rsm_normal"
#define		r2_rt_lpv_rsm_position			"$user$lpv_rsm_position"


// Radiance
#define		r2_rt_lpv_radiance_r			"$user$lpv_radiance_r"
#define		r2_rt_lpv_radiance_g			"$user$lpv_radiance_g"
#define		r2_rt_lpv_radiance_b			"$user$lpv_radiance_b"

// Geometry occludes
#define		r2_rt_lpv_occluders				"$user$lpv_occluders"


//
//#define		r2_rt_ssao_temp		"$user$ssao_temp"		//temporary rt for ssao calculation
//
//#define		r2_rt_bloom1		"$user$bloom1"			// ---
//#define		r2_rt_bloom2		"$user$bloom2"			// ---
//
//#define		r2_rt_luminance_t64	"$user$lum_t64"			// --- temp
//#define		r2_rt_luminance_t8	"$user$lum_t8"			// --- temp
//
//#define		r2_rt_luminance_src	"$user$tonemap_src"		// --- prev-frame-result
//#define		r2_rt_luminance_cur	"$user$tonemap"			// --- result
//#define		r2_rt_luminance_pool "$user$luminance"		// --- pool
//
//

#define		r2_rt_shadow_map	"$user$shadow_map"		// ---directional
#define		r2_rt_shadow_map0	"$user$shadow_map0"		// ---directional
#define		r2_rt_shadow_map1	"$user$shadow_map1"		// ---directional
#define		r2_rt_shadow_map2	"$user$shadow_map2"		// ---directional
#define		r2_rt_shadow_map3	"$user$shadow_map3"		// ---directional

#define		r2_rt_shadow_map_size_1024	"$user$shadow_map_size_1024"
#define		r2_rt_shadow_map_size_512	"$user$shadow_map_size_512"
#define		r2_rt_shadow_map_size_256	"$user$shadow_map_size_256"

//
//#define		r2_rt_smap_surf		"$user$smap_surf"		// --- directional
#define		r2_rt_smap_depth	"$user$smap_depth"		// ---directional
//
//#define		r2_mat_parms		"$user$mat_params"		// ---
#define		r2_material			"$user$material"		// ---
//#define		r2_ds2_fade			"$user$ds2_fade"		// ---
//
#define		r2_jitter			"$user$jitter_"			// --- dither
//#define		r2_jitter_mipped	"$user$jitter_mipped"			// --- dither
#define		r2_sunmask			"sunmask"

#define		JITTER(a) r2_jitter #a

#define		r2_rt_cubemap			"$user$cubemap"			// ---

//const		float				smap_near_plane		= .1f;
//
//const		u32					smap_adapt_min		= 32;
//const		u32					smap_adapt_optimal	= 768;
//const		u32					smap_adapt_max		= 1536;
//
const		u32					tex_material_ldotn	= 128;	// diffuse,		x, almost linear = small res
const		u32					tex_material_ldoth	= 256;	// specular,	y
const		u32					tex_material_count	= 4;	
const		u32					tex_jitter			= 64;
//const		u32					tex_jitter_count	= 4;
//
//const		u32					bloom_size_x		= 256;
//const		u32					bloom_size_y		= 256;
//const		u32					luminance_size		= 16;

// deffer
#define		SE_R2_NORMAL_HQ		0	// high quality/detail
#define		SE_R2_NORMAL_LQ		1	// low quality
#define		SE_R2_SHADOW		2	// shadow generation

// spot
#define		SE_L_FILL			0
#define		SE_L_UNSHADOWED		1
#define		SE_L_NORMAL			2	// typical, scaled
#define		SE_L_FULLSIZE		3	// full texture coverage
#define		SE_L_TRANSLUENT		4	// with opacity/color mask

// mask
#define		SE_MASK_SPOT		0
#define		SE_MASK_POINT		1
#define		SE_MASK_DIRECT		2
#define		SE_MASK_ACCUM_VOL	3
#define		SE_MASK_ACCUM_2D	4
#define		SE_MASK_ALBEDO		5

// sun
#define		SE_SUN_NEAR			0
#define		SE_SUN_FAR			1
#define		SE_SUN_LUMINANCE	2

#endif // #ifndef SHARED_NAMES_H_INCLUDED