////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.05.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/backend.h>
#include <xray/render/core/shader_constant_defines.h>
#include <xray/render/core/shader_constant_host.h>
#include "speedtree_shader_parameters.h"
#include "speedtree_convert_type.h"
#include "speedtree_tree.h"
#include "speedtree.h"
#include "speedtree_forest.h"
#include "renderer_context.h"
#include "scene.h"
#include "material.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/custom_config.h>

namespace xray {
namespace render {

using namespace SpeedTree;

speedtree_wind_parameters::speedtree_wind_parameters()
{
	m_wind_dir_parameter				= backend::ref().register_constant_host( "wind_direction_parameter", rc_float);
	m_wind_times_parameter				= backend::ref().register_constant_host( "wind_times_parameter", rc_float);
	m_wind_distances_parameter			= backend::ref().register_constant_host( "wind_distances_parameter", rc_float);
	m_wind_leaves_parameter				= backend::ref().register_constant_host( "wind_leaves_parameter", rc_float);
	m_wind_frond_ripple_parameter		= backend::ref().register_constant_host( "wind_frond_ripple_parameter", rc_float);
	m_wind_gust_parameter				= backend::ref().register_constant_host( "wind_gust_parameter", rc_float);
	m_wind_gust_hints_parameter			= backend::ref().register_constant_host( "wind_gust_hints_parameter", rc_float);
	m_wind_rolling_branches_parameter	= backend::ref().register_constant_host( "wind_rolling_branches", rc_float);
	m_wind_rolling_leaves_parameter		= backend::ref().register_constant_host( "wind_rolling_leaves", rc_float);
	m_wind_twitching_leaves_parameter	= backend::ref().register_constant_host( "wind_twitching_leaves", rc_float);
	m_wind_tumbling_leaves_parameter	= backend::ref().register_constant_host( "wind_tumbling_leaves", rc_float);
}

void speedtree_wind_parameters::set(SpeedTree::CWind const& wind)
{
	float const* shader_values			= wind.GetShaderValues( );
	
	backend::ref().set_vs_constant		(m_wind_dir_parameter,				speedtree_to_xray(Vec3(shader_values + CWind::SH_WIND_DIR_X)));
	backend::ref().set_vs_constant		(m_wind_times_parameter,			speedtree_to_xray(Vec4(shader_values + CWind::SH_TIME_PRIMARY)));
	backend::ref().set_vs_constant		(m_wind_distances_parameter,		speedtree_to_xray(Vec4(shader_values + CWind::SH_DIST_PRIMARY)));
	backend::ref().set_vs_constant		(m_wind_leaves_parameter,			speedtree_to_xray(Vec3(shader_values + CWind::SH_DIST_LEAVES)));
	backend::ref().set_vs_constant		(m_wind_gust_parameter,				speedtree_to_xray(Vec3(shader_values + CWind::SH_STRENGTH_COMBINED)));
	backend::ref().set_vs_constant		(m_wind_gust_hints_parameter,		speedtree_to_xray(Vec3(shader_values + CWind::SH_HEIGHT_OFFSET)));
	backend::ref().set_vs_constant		(m_wind_frond_ripple_parameter,		speedtree_to_xray(Vec3(shader_values + CWind::SH_DIST_FROND_RIPPLE)));
	backend::ref().set_vs_constant		(m_wind_rolling_branches_parameter,	speedtree_to_xray(Vec4(shader_values + CWind::SH_ROLLING_BRANCHES_MAX_SCALE)));
	backend::ref().set_vs_constant		(m_wind_rolling_leaves_parameter,	speedtree_to_xray(Vec4(shader_values + CWind::SH_ROLLING_LEAVES_MAX_SCALE)));
	backend::ref().set_vs_constant		(m_wind_twitching_leaves_parameter,	speedtree_to_xray(Vec4(shader_values + CWind::SH_LEAVES_TWITCH_AMOUNT)));
	backend::ref().set_vs_constant		(m_wind_tumbling_leaves_parameter,	speedtree_to_xray(Vec4(shader_values + CWind::SH_LEAVES_TUMBLE_X)));
}


speedtree_billboard_parameters::speedtree_billboard_parameters()
{
	m_billboard_dimensions_parameter	= backend::ref().register_constant_host("billboard_dimensions_parameter", rc_float );
	m_camera_azimuth_trig_parameter		= backend::ref().register_constant_host("camera_azimuth_trig_parameter", rc_float );
	m_camera_angles_parameter			= backend::ref().register_constant_host("camera_angles_parameter", rc_float );
	m_num360images_parameter			= backend::ref().register_constant_host("num360images_parameter", rc_float );
	m_tex_coords_360_parameter			= backend::ref().register_constant_host("tex_coords_360_parameter", rc_float );
	m_billboard_tangents_parameter		= backend::ref().register_constant_host("billboard_tangents_parameter", rc_float );
	m_view_location_parameter			= backend::ref().register_constant_host("view_location_parameter", rc_float );
}

void speedtree_billboard_parameters::set(renderer_context* context, speedtree_tree_component* tree_component)
{
	float const azimuth					= context->scene()->get_speedtree_forest()->get_speedtree_view().GetCameraAzimuth( );
	
	set_billboard_tangents				(azimuth);
	
	SpeedTree::SVerticalBillboards const& vbb = tree_component->m_parent->GetGeometry()->m_sVertBBs;
	backend::ref().set_vs_constant		(m_billboard_dimensions_parameter, float4(0.5f * vbb.m_fWidth, -0.5f * vbb.m_fWidth, vbb.m_fTopCoord, vbb.m_fBottomCoord));
	
	// upload rotation parameters, common to all billboards
	float sine	  = math::sin			(-azimuth);
	
	backend::ref().set_vs_constant		(m_num360images_parameter, float(vbb.m_nNumBillboards));
	backend::ref().set_vs_constant		(
		m_camera_angles_parameter, 
		float4(
			context->scene()->get_speedtree_forest()->get_speedtree_view().GetCameraAzimuth(), 
			context->scene()->get_speedtree_forest()->get_speedtree_view().GetCameraPitch(),
			0, 0
		)
	);
	
	float4* arr = (float4*)ALLOCA(sizeof(float4)*vbb.m_nNumBillboards);
	for (u32 i=0; i<u32(vbb.m_nNumBillboards); ++i)
	{
		arr[i] = float4(
			vbb.m_pTexCoords[i * 4 + 0], 
			vbb.m_pTexCoords[i * 4 + 1],
			vbb.m_pTexCoords[i * 4 + 2],
			vbb.m_pTexCoords[i * 4 + 3]
		);
	}
	
	backend::ref().set_vs_constant		(m_tex_coords_360_parameter, arr, vbb.m_nNumBillboards);
	backend::ref().set_vs_constant		(m_camera_azimuth_trig_parameter, float4(sine, math::cos(azimuth), -sine, 0.0f));
	backend::ref().set_vs_constant		(m_view_location_parameter, context->get_view_pos());
}

void speedtree_billboard_parameters::set_billboard_tangents(float camera_azimuth)
{
	camera_azimuth		+= math::pi;
	
	if (CCoordSys::IsLeftHanded( ))
		camera_azimuth	= math::pi - camera_azimuth;
	
	Vec3 binormal		= CCoordSys::UpAxis( );
	Vec3 normal			= CCoordSys::ConvertFromStd(math::cos(camera_azimuth), math::sin(camera_azimuth), 0.0f);
	Vec3 tangent		= binormal.Cross(normal);
	
	// negate the tangent if RH/Yup or LH/Zup
	if ((CCoordSys::IsYAxisUp() && !CCoordSys::IsLeftHanded()) ||
		(!CCoordSys::IsYAxisUp() && CCoordSys::IsLeftHanded()))
		tangent = -tangent;
	
	float4 billboard_tangents[3] = 
	{
		float4(normal.x,	normal.y,	 normal.z,   0.0f),
		float4(binormal.x,	binormal.y,  binormal.z, 0.0f),
		float4(tangent.x,	tangent.y,	 tangent.z,  0.0f)
	};
	
	backend::ref().set_vs_constant(m_billboard_tangents_parameter, billboard_tangents);
}

speedtree_common_parameters::speedtree_common_parameters()
{
	m_camera_facing_matrix_parameter			= backend::ref().register_constant_host( "camera_facing_matrix_parameter", rc_float);
	m_lod_profile_parameter						= backend::ref().register_constant_host( "lod_profile_parameter", rc_float );
	m_lod_reference_position_parameter			= backend::ref().register_constant_host( "lod_reference_position_parameter", rc_float );
}

void speedtree_common_parameters::set(renderer_context* context, speedtree_tree_component* tree_component, xray::math::float3 const& lod_reference_position)
{
	backend::ref().set_vs_constant				(m_lod_reference_position_parameter, float4(lod_reference_position, 1.0f));
	
	backend::ref().set_vs_constant				(
		m_camera_facing_matrix_parameter, 
		transpose( speedtree_to_xray(SpeedTree::Mat4x4(context->scene()->get_speedtree_forest()->get_speedtree_view().GetCameraFacingMatrix())) )
	);
	
	SpeedTree::SLodProfile const& lod_profile	= tree_component->m_parent->GetLodProfile();
	if (lod_profile.m_bLodIsPresent)
	{
		backend::ref().set_vs_constant			(
			m_lod_profile_parameter, 
			float4(
				lod_profile.m_fHighDetail3dDistance,
				lod_profile.m_fLowDetail3dDistance,
				lod_profile.m_fBillboardStartDistance,
				lod_profile.m_fBillboardFinalDistance
			)
		);
	}
	else
	{
		float const very_far_away				= 999999.0f;
		backend::ref().set_vs_constant			(m_lod_profile_parameter, float4(0.0f, very_far_away, very_far_away * 2.0f, very_far_away * 3.0f));
	}
}


speedtree_tree_parameters::speedtree_tree_parameters()
{
	m_tree_position_and_scale_parameter			= backend::ref().register_constant_host( "tree_position_and_scale_parameter", rc_float);
	m_tree_rotation_parameter					= backend::ref().register_constant_host( "tree_rotation_parameter", rc_float);
}

void speedtree_tree_parameters::set(
		speedtree_tree_component* tree_component, 
		SpeedTree::CInstance const* instance, 
		SpeedTree::SInstanceLod const* instance_lod)
{
	float4 tree_pos								= speedtree_to_xray(SpeedTree::Vec4(instance->GetPosAndScale()));
	backend::ref().set_vs_constant				(m_tree_position_and_scale_parameter, tree_pos);
	
	float const* rotation_vector				= instance->GetRotationVector();
	backend::ref().set_vs_constant				( 
		m_tree_rotation_parameter,
		math::float4(
			rotation_vector[0], 
			rotation_vector[1], 
			instance_lod->m_afShaderLerps[ tree_component->get_geometry_type() ], 
			instance_lod->m_fLod
		) 
	);
			
}
} // namespace render
} // namespace xray
