////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.05.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_SPEEDTREE_SHADER_PARAMETERS_H_INCLUDED
#define XRAY_RENDER_SPEEDTREE_SHADER_PARAMETERS_H_INCLUDED

namespace SpeedTree {
	class CForest;
	class CCore;
	class CInstance;
	struct SInstanceLod;
	class CView;
	class CWind;
}

namespace xray {
namespace render {

class shader_constant_host;
class renderer_context;
class speedtree_tree_component;

struct speedtree_wind_parameters
{
	speedtree_wind_parameters		();
	void set						(SpeedTree::CWind const& wind);
	
	shader_constant_host*			m_wind_dir_parameter;
	shader_constant_host*			m_wind_times_parameter;
	shader_constant_host*			m_wind_distances_parameter;
	shader_constant_host*			m_wind_leaves_parameter;
	shader_constant_host*			m_wind_gust_parameter;
	shader_constant_host*			m_wind_gust_hints_parameter;
	shader_constant_host*			m_wind_frond_ripple_parameter;
	shader_constant_host*			m_wind_rolling_branches_parameter;
	shader_constant_host*			m_wind_rolling_leaves_parameter;
	shader_constant_host*			m_wind_twitching_leaves_parameter;
	shader_constant_host*			m_wind_tumbling_leaves_parameter;
}; // struct speedtree_wind_parameters

struct speedtree_billboard_parameters
{
	speedtree_billboard_parameters	();
	
	void set						(renderer_context* context, speedtree_tree_component* tree_component);
	
	shader_constant_host*			m_billboard_tangents_parameter;
	shader_constant_host*			m_billboard_dimensions_parameter;
	shader_constant_host*			m_camera_azimuth_trig_parameter;
	shader_constant_host*			m_camera_angles_parameter;
	shader_constant_host*			m_num360images_parameter;
	shader_constant_host*			m_tex_coords_360_parameter;
	shader_constant_host*			m_view_location_parameter;
	
private:
	void set_billboard_tangents		(float fCameraAzimuth);
}; // struct speedtree_billboard_parameters


struct speedtree_common_parameters
{
	speedtree_common_parameters		();
	void set						(renderer_context* context, speedtree_tree_component* tree_component, xray::math::float3 const& lod_reference_position);
	
	shader_constant_host*			m_camera_facing_matrix_parameter;
	shader_constant_host*			m_lod_profile_parameter;
	shader_constant_host*			m_lod_reference_position_parameter;
}; // struct speedtree_common_parameters


struct speedtree_tree_parameters
{
	speedtree_tree_parameters		();
	void set						(speedtree_tree_component* tree_component, 
									 SpeedTree::CInstance const* instance, 
									 SpeedTree::SInstanceLod const* instance_lod);
	
	shader_constant_host*			m_tree_position_and_scale_parameter;
	shader_constant_host*			m_tree_rotation_parameter;
}; // struct speedtree_transform_parameters


} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_SPEEDTREE_SHADER_PARAMETERS_H_INCLUDED