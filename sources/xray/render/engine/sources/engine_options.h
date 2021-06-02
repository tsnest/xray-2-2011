////////////////////////////////////////////////////////////////////////////
//	Created		: 04.11.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_OPTION_H_INCLUDED
#define XRAY_RENDER_ENGINE_OPTION_H_INCLUDED

namespace xray {
namespace render {

	/*
enum enum_shadow_quality
{
	enum_shadow_quality_low		= 0,
	enum_shadow_quality_medium,
	enum_shadow_quality_high,
};

enum enum_gbuffer_pos_packing
{
	enum_gbuffer_pos_packing_xyz_param = 0,
	enum_gbuffer_pos_packing_z_quadratic_norm_param,
};

enum enum_gbuffer_normal_packing
{
	enum_gbuffer_normal_packing_xyz_mat = 0,
	enum_gbuffer_normal_packing_xy_unorm_mat_zsign,
	enum_gbuffer_normal_packing_xyz_unorm_mat,
};

template <class T>
class option;

template<typename T>
class option_enum;

typedef option<bool>  option_bool;
typedef option<float> option_float;
typedef option<int>   option_int;

extern option_bool*						use_parallax_option;
extern option_bool*						use_branching_option;
extern option_bool*						support_fp16_blend_option;
extern option_int*						shadow_map_size_option;
extern option_int*						spot_shadow_map_size_option;
extern option_float*					test_float_option;

extern option_enum<enum_gbuffer_pos_packing>*		gbuffer_pos_packing_option;
extern option_enum<enum_gbuffer_normal_packing>*	gbuffer_normal_packing_option;
extern option_enum<enum_shadow_quality>*			shadow_quality_option;

*/
} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_OPTION_H_INCLUDED