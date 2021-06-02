////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.05.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_SPEEDTREE_FOREST_H_INCLUDED
#define XRAY_RENDER_SPEEDTREE_FOREST_H_INCLUDED

#include <xray/render/facade/model.h>
#include "speedtree_shader_parameters.h"

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

class speedtree_tree_component;
class renderer_context;
struct lod_entry;
class speedtree_tree;

class shader_constant_host;

struct speedtree_forest
{
	struct tree_render_info
	{
		tree_render_info(lod_entry const*  in_lod, 
			SpeedTree::CInstance const*	   in_instance, 
			SpeedTree::SInstanceLod const* in_instance_lod,
			speedtree_tree_component*	   in_tree_component
		):
			lod(in_lod), 
			instance(in_instance),
			instance_lod(in_instance_lod),
			tree_component(in_tree_component) 
		{}
		lod_entry const*				lod;
		SpeedTree::CInstance const*		instance;
		SpeedTree::SInstanceLod const*	instance_lod;
		speedtree_tree_component*		tree_component;
	}; // struct tree_render_info
	
	typedef render::vector<tree_render_info>			tree_render_info_array_type;
	typedef render::vector<speedtree_tree_ptr>			trees_type;
	typedef render::vector<speedtree_instance_ptr>		tree_instances_type;
	typedef render::vector<speedtree_tree_component*>	tree_render_instances_type;
	
						speedtree_forest	();
						~speedtree_forest	();
	SpeedTree::CForest& get_forest			() const { return *m_forest; }
	void				populate_forest		();
	void				add_instance		(speedtree_instance_ptr st_instance_ptr, math::float4x4 const& transform);
	void				remove_instance		(speedtree_instance_ptr st_instance_ptr);
	void				set_transform		(speedtree_instance_ptr st_instance_ptr, math::float4x4 const& transform);
	
	void				set_wind_direction	(xray::math::float3 const& wind_direction);
	void				set_wind_strength	(float wind_strength);
	
	speedtree_wind_parameters&				get_speedtree_wind_parameters		() { return m_speedtree_wind_parameters; };
	speedtree_billboard_parameters&			get_speedtree_billboard_parameters	() { return m_speedtree_billboard_parameters; };
	speedtree_tree_parameters&				get_speedtree_tree_parameters		() { return m_speedtree_tree_parameters; };
	speedtree_common_parameters&			get_speedtree_common_parameters		() { return m_speedtree_common_parameters; };
	
	void				get_visible_tree_components	(renderer_context* context,
													 xray::math::float3 const& lod_reference_point,
													 bool sort_result, 
													 tree_render_info_array_type& 
													 out_tree_render_info_array);
	
	void				get_visible_tree_components	(renderer_context* context, 
													 render::vector<render::speedtree_instance_ptr> const& instances, 
													 bool sort_result, 
													 tree_render_info_array_type& out_tree_render_info_array);
	
	u32					get_num_visible_instances	() const;

	void				tick						(renderer_context* context);
	
	static float							scale_speedtree_to_xray;
	SpeedTree::CView&						get_speedtree_view					() { return m_view; }

	math::float4x4							get_instance_transform				(SpeedTree::CInstance const& in_instance);
private:
	
	void cull_and_compute_lod				(renderer_context* context, xray::math::float3 const& lod_reference_point, bool sort_result);
	
	speedtree_wind_parameters				m_speedtree_wind_parameters;
	speedtree_billboard_parameters			m_speedtree_billboard_parameters;
	speedtree_tree_parameters				m_speedtree_tree_parameters;
	speedtree_common_parameters				m_speedtree_common_parameters;
	
	SpeedTree::SForestCullResults			m_visible_trees;
	SpeedTree::SGrassCullResults			m_visuble_grass;
	SpeedTree::CView						m_view;
	
	SpeedTree::CGrass						m_grass;
	SpeedTree::CForest*						m_forest;
	
	trees_type								m_trees;
	tree_instances_type						m_tree_instances;

	SpeedTree::CWind						m_wind_leader;
	
}; // struct speedtree_forest

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_SPEEDTREE_FOREST_H_INCLUDED