////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.05.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "speedtree_forest.h"
#include "speedtree.h"
#include "speedtree_cook.h"
#include "speedtree_tree.h"
#include "speedtree_instance_impl.h"
#include "speedtree_convert_type.h"
#include "speedtree_tree_component_billboard.h"

#include "renderer_context.h"
#include "scene_view.h"
#include <xray/render/core/backend.h>

#include <xray/engine/console.h>
#include <xray/console_command.h>
#include <xray/console_command_processor.h>

#include "statistics.h"
#include "material.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/custom_config.h>

#include <xray/render/core/options.h>

static u32 s_speedtree_lod_index_value = 0;
static xray::console_commands::cc_u32 s_speedtree_lod_index("speedtree_lod", s_speedtree_lod_index_value, 0, 10, true, xray::console_commands::command_type_engine_internal);

static u32 num_speedtree_memory_used = 0;

namespace xray {
namespace render {

using namespace SpeedTree;

float speedtree_forest::scale_speedtree_to_xray = 1.0f;//0.3048f;


speedtree_forest::speedtree_forest()
{
	m_forest = st_new(CForest, "CForest");
	
	m_forest->EnableWind			(true);
	m_forest->SetGlobalWindDirection(Vec3(0.5f,0.0f,0.5f));
	m_forest->SetGlobalWindStrength	(0.1f);
	m_forest->InitBillboardSystem	();
	m_forest->SetCullCellSize		(25);
	
// 	m_grass.Init("null", 2, 2, 1.0f);
// 	
// 	m_grass.SetHint(CGrass::HINT_MAX_NUM_VISIBLE_CELLS, 2 * 50);
// 	m_grass.SetHeightHints(0.0f, 1.0f);
// 	
// 	const char* pError = SpeedTree::CCore::GetError();
// 	while (pError)
// 	{
// 		LOG_ERROR("SpeedTree Error: %s\n", pError);
// 		pError = CCore::GetError();
// 	}
}

speedtree_forest::~speedtree_forest()
{
	st_delete(m_forest);
}

void speedtree_forest::set_wind_direction(xray::math::float3 const& wind_direction)
{
	m_wind_leader.SetDirection( xray_to_speedtree(wind_direction) );
}

void speedtree_forest::set_wind_strength(float wind_strength)
{
	m_wind_leader.SetStrength( wind_strength );
}

u32 speedtree_forest::get_num_visible_instances() const
{
	// TODO
	
	u32 result = 0;
	
//	if (m_visible_trees.m_pBaseTrees)
//	{
//		for (s32 base_tree_index = 0; base_tree_index < s32(m_visible_trees.m_pBaseTrees->size()); base_tree_index++)
//		{
//			result += m_visible_trees.VisibleInstances(base_tree_index)->size();
//		}
//	}
	
	return result;
}

void speedtree_forest::get_visible_tree_components(renderer_context* context, render::vector<render::speedtree_instance_ptr> const& instances, bool sort_result, tree_render_info_array_type& out_tree_render_info_array)
{
	tree_render_info_array_type visible_trees;
	
	speedtree_forest::get_visible_tree_components(context, context->get_v_inverted().c.xyz(), sort_result, visible_trees);
	
	for (render::vector<render::speedtree_instance_ptr>::const_iterator instance_it = instances.begin(); instance_it != instances.end(); ++instance_it)
	{
		speedtree_instance_impl* instances_impl = (speedtree_instance_impl*)(*instance_it).c_ptr();
		
		for (speedtree_forest::tree_render_info_array_type::iterator it = visible_trees.begin(); it != visible_trees.end(); ++it)
		{
			SpeedTree::CInstance const* st_instance = it->instance;
			
			if (!st_instance)
				continue;
			
			if (st_instance->GetPos()==instances_impl->get_speedtree_instance().GetPos())
			{
				out_tree_render_info_array.push_back( *it );
			}
		}
	}
}

void speedtree_forest::tick(renderer_context* context)
{
	set_wind_direction				(context->get_scene_view()->post_process_parameters().wind_direction);
	set_wind_strength				(context->get_scene_view()->post_process_parameters().wind_strength);
	m_forest->SetGlobalTime			(context->get_current_time());
	m_forest->SetWindLeader			(&m_wind_leader);
	m_forest->AdvanceGlobalWind		();
}

math::float4x4 speedtree_forest::get_instance_transform(SpeedTree::CInstance const& in_instance)
{
	for (tree_instances_type::const_iterator st_instance_it = m_tree_instances.begin(); st_instance_it != m_tree_instances.end(); ++st_instance_it)
	{
		speedtree_instance_impl* impl = static_cast_checked<speedtree_instance_impl*>((*st_instance_it).c_ptr());
		
		if (in_instance.GetPos() == impl->get_speedtree_instance().GetPos())
			return impl->m_transform;
	}
	return math::float4x4().identity();
}

void speedtree_forest::get_visible_tree_components(renderer_context* context, 
												   math::float3 const& lod_reference_point,
												   bool sort_result, 
												   tree_render_info_array_type& out_tree_render_info_array)
{
	BEGIN_TIMER(statistics::ref().speedtree_stat_group.culling_time);
	cull_and_compute_lod			(context, lod_reference_point, sort_result);
	END_TIMER;
	
	for (s32 base_tree_index=0; base_tree_index<s32(m_visible_trees.m_pBaseTrees->size()); base_tree_index++)
	{
		speedtree_tree* base_tree = (speedtree_tree*)m_visible_trees.m_pBaseTrees->at(base_tree_index);
		
		if (options::ref().m_enabled_draw_speedtree_billboards &&
			base_tree->m_billboard_component && base_tree->m_billboard_component->m_render_geometry.geom)
		{
			out_tree_render_info_array.push_back( tree_render_info(0, 0, 0, base_tree->m_billboard_component) );
		}
		
		const TInstanceLodArray* instance_lods = m_visible_trees.VisibleInstances(base_tree_index);

		if (options::ref().m_enabled_draw_speedtree_branches && base_tree->HasGeometryType(GEOMETRY_TYPE_BRANCHES))
		{
			if (instance_lods && !instance_lods->empty( ))
			{
				s32 num_instances = s32(instance_lods->size());
				
				lod_render_info const& lod_info = base_tree->get_lod_render_info(SpeedTree::GEOMETRY_TYPE_BRANCHES);
				
				for (s32 i=0; i<num_instances; i++)
				{
					const SInstanceLod* instance_lod = &instance_lods->at(i);
					const SLodSnapshot& lod_snapshot = instance_lod->m_sLodSnapshot;
					
					if (lod_snapshot.m_nBranchLodIndex <= -1)
						continue;
					
					const CInstance* instance = instance_lod->m_pInstance;
					ASSERT(instance);
					
					lod_entry const* lod_entry = lod_info.is_active(lod_snapshot.m_nBranchLodIndex+s_speedtree_lod_index_value);
					
					if (!lod_entry)
						continue;
					
					if (base_tree->m_branch_component && base_tree->m_branch_component->m_render_geometry.geom)
						out_tree_render_info_array.push_back( tree_render_info(lod_entry, instance, instance_lod, base_tree->m_branch_component) );
				}
			}
		}

		if (options::ref().m_enabled_draw_speedtree_fronds && base_tree->HasGeometryType(GEOMETRY_TYPE_FRONDS))
		{
			instance_lods = m_visible_trees.VisibleInstances(base_tree_index);
			if (instance_lods && !instance_lods->empty( ))
			{
				s32 num_instances = s32(instance_lods->size());
				
				lod_render_info const& lod_info = base_tree->get_lod_render_info(SpeedTree::GEOMETRY_TYPE_FRONDS);
				
				for (s32 i=0; i<num_instances; i++)
				{
					const SInstanceLod* instance_lod = &instance_lods->at(i);
					const SLodSnapshot& lod_snapshot = instance_lod->m_sLodSnapshot;
					
					if (lod_snapshot.m_nFrondLodIndex <= -1)
						continue;
					
					const CInstance* instance = instance_lod->m_pInstance;
					ASSERT(instance);
					
					lod_entry const* lod_entry = lod_info.is_active(lod_snapshot.m_nFrondLodIndex+s_speedtree_lod_index_value);
					
					if (!lod_entry)
						continue;
					
					if (base_tree->m_frond_component && base_tree->m_frond_component->m_render_geometry.geom)
						out_tree_render_info_array.push_back( tree_render_info(lod_entry, instance, instance_lod, base_tree->m_frond_component) );
				}
			}
		}
		
		if (options::ref().m_enabled_draw_speedtree_leafmeshes && base_tree->HasGeometryType(GEOMETRY_TYPE_LEAF_MESHES))
		{
			instance_lods = m_visible_trees.VisibleInstances(base_tree_index);
			if (instance_lods && !instance_lods->empty( ))
			{
				s32 num_instances = s32(instance_lods->size());
				
				lod_render_info const& lod_info = base_tree->get_lod_render_info(SpeedTree::GEOMETRY_TYPE_LEAF_MESHES);
				
				for (s32 i=0; i<num_instances; i++)
				{
					const SInstanceLod* instance_lod = &instance_lods->at(i);
					const SLodSnapshot& lod_snapshot = instance_lod->m_sLodSnapshot;
					
					if (lod_snapshot.m_nLeafMeshLodIndex <= -1)
						continue;
					
					const CInstance* instance = instance_lod->m_pInstance;
					ASSERT(instance);
					
					lod_entry const* lod_entry = lod_info.is_active(lod_snapshot.m_nLeafMeshLodIndex+s_speedtree_lod_index_value);
					
					if (!lod_entry)
						continue;
					
					if (base_tree->m_leafmesh_component && base_tree->m_leafmesh_component->m_render_geometry.geom)
						out_tree_render_info_array.push_back( tree_render_info(lod_entry, instance, instance_lod, base_tree->m_leafmesh_component) );
				}
			}
		}
		
		if (options::ref().m_enabled_draw_speedtree_leafcards && base_tree->HasGeometryType(GEOMETRY_TYPE_LEAF_CARDS))
		{
			instance_lods = m_visible_trees.VisibleInstances(base_tree_index);
			if (instance_lods && !instance_lods->empty( ))
			{
				s32 num_instances = s32(instance_lods->size());
				
				lod_render_info const& lod_info = base_tree->get_lod_render_info(SpeedTree::GEOMETRY_TYPE_LEAF_CARDS);
				
				for (s32 i=0; i<num_instances; i++)
				{
					const SInstanceLod* instance_lod = &instance_lods->at(i);
					const SLodSnapshot& lod_snapshot = instance_lod->m_sLodSnapshot;
					
					if (lod_snapshot.m_nLeafCardLodIndex <= -1)
						continue;
					
					const CInstance* instance = instance_lod->m_pInstance;
					ASSERT(instance);
					
					lod_entry const* lod_entry = lod_info.is_active(lod_snapshot.m_nLeafCardLodIndex+s_speedtree_lod_index_value);
					
					if (!lod_entry)
						continue;
					
					if (base_tree->m_leafcard_component && base_tree->m_leafcard_component->m_render_geometry.geom)
						out_tree_render_info_array.push_back( tree_render_info(lod_entry, instance, instance_lod, base_tree->m_leafcard_component) );
				}
			}
		}
	}
}

void speedtree_forest::populate_forest()
{
	m_forest->ClearInstances			();
	
	CArray<CTree*>						base_trees;
	CArray<TInstanceArray>				base_tree_instances;
	
	base_trees.reserve					(m_trees.size());
	base_tree_instances.reserve			(m_trees.size());
	
	for (trees_type::const_iterator tree_it=m_trees.begin(); tree_it!=m_trees.end(); ++tree_it)
	{
		speedtree_tree* tree			= (speedtree_tree*)tree_it->c_ptr();
		SpeedTree::CTree* base_tree		= (SpeedTree::CTree*)tree;
		base_trees.push_back			(base_tree);
		base_tree_instances.push_back	(TInstanceArray());
	}
	
	u32 tree_index = 0;
	for (trees_type::const_iterator tree_it=m_trees.begin(); tree_it!=m_trees.end(); ++tree_it)
	{
		TInstanceArray& instances		= base_tree_instances.at(tree_index);
		
		for (tree_instances_type::const_iterator st_instance_it=m_tree_instances.begin(); st_instance_it!=m_tree_instances.end(); ++st_instance_it)
		{
			speedtree_instance_impl* impl = static_cast_checked<speedtree_instance_impl*>((*st_instance_it).c_ptr());
			if (impl->m_speedtree_tree_ptr==*tree_it)
			{
				instances.push_back		(impl->get_speedtree_instance());
			}
		}
		
		tree_index++;
	}
	
	CForest::SCompletePopulation sWholePop(base_trees.size());
	
	sWholePop.m_aBaseTrees				= base_trees;
	sWholePop.m_aaInstances				= base_tree_instances;
	
	if (m_forest->PopulateAtOnce(sWholePop))
	{
		m_forest->UpdateTreeCellExtents();
		m_forest->EndInitialPopulation ();
	}
	
	tree_index = 0;
	for (trees_type::const_iterator tree_it=m_trees.begin(); tree_it!=m_trees.end(); ++tree_it)
	{
		speedtree_tree* tree			= (speedtree_tree*)tree_it->c_ptr();
// 		
// 		if (tree_index==0)
// 		{
// 			//SpeedTree::CWind* wind_leader = &tree->GetWind();
// 			//m_forest->SetWindLeader		(wind_leader);
// 		}
// 		
		if (tree->HasGeometryType(GEOMETRY_TYPE_VERTICAL_BILLBOARDS))
		{
			// if we repopulate the forest.
			if (tree->m_billboard_component)
			{
				TInstanceArray const& instances_of_tree = base_tree_instances.at(tree_index);
				
				if (instances_of_tree.size())
					tree->m_billboard_component->init(*this, instances_of_tree);
			}
		}
		
		tree_index++;
	}
	
	SpeedTree::CForest::SPopulationStats sStats;
	m_forest->GetPopulationStats		(sStats);
}

static math::float4x4 fix_view_matrix(math::float4x4 const& in_view_matrix)
{
	const float epsilon			= 0.1f;
	float4x4 view_matrix		= in_view_matrix;
	float3 fixed_view_dir		= float3(view_matrix.i.z, view_matrix.j.z, view_matrix.k.z);
	bool const looking_up		= math::is_similar(fixed_view_dir, float3(0.0f,  1.0f, 0.0f), epsilon);
	bool const looking_down		= math::is_similar(fixed_view_dir, float3(0.0f, -1.0f, 0.0f), epsilon);
	if (looking_up || looking_down)
	{
		view_matrix.i.z			+= epsilon * 10.0f;
		view_matrix.k.z			+= epsilon * 10.0f;
		view_matrix.j.z			= (float)math::floor(view_matrix.j.z);
		
		math::clamp				(view_matrix.j.z, -0.99f, 0.99f);
		math::clamp				(view_matrix.j.z, -0.99f, 0.99f);
		math::clamp				(view_matrix.k.z, -0.99f, 0.99f);
	}
	math::clamp				(view_matrix.j.x, -0.99f, 0.99f);
	math::clamp				(view_matrix.j.x, -0.99f, 0.99f);
	math::clamp				(view_matrix.k.x, -0.99f, 0.99f);

	math::clamp				(view_matrix.j.y, -0.99f, 0.99f);
	math::clamp				(view_matrix.j.y, -0.99f, 0.99f);
	math::clamp				(view_matrix.k.y, -0.99f, 0.99f);

	math::clamp				(view_matrix.j.z, -0.99f, 0.99f);
	math::clamp				(view_matrix.j.z, -0.99f, 0.99f);
	math::clamp				(view_matrix.k.z, -0.99f, 0.99f);
	return						view_matrix;
}

void speedtree_forest::cull_and_compute_lod(renderer_context* context, math::float3 const& lod_reference_point, bool sort_result)
{
	// Fix view direction for speedtree to avoid math error.
	m_view.Set					(
		xray_to_speedtree		(context->get_culling_v_inverted().c.xyz()),
		xray_to_speedtree		(context->get_p()),
		xray_to_speedtree		(fix_view_matrix(context->get_culling_v())),
		context->get_near		(), 
		context->get_far		()
	);
	
	m_view.SetLodRefPoint		(xray_to_speedtree(lod_reference_point));
	m_forest->CullAndComputeLOD	(m_view, m_visible_trees, sort_result);
	
// 	if (m_grass.IsEnabled())
// 		m_grass.Cull			(m_view, m_visuble_grass);
}

void speedtree_forest::add_instance(speedtree_instance_ptr st_instance_ptr, math::float4x4 const& transform)
{
	speedtree_tree*				tree			= static_cast_checked<speedtree_tree*>(st_instance_ptr->m_speedtree_tree_ptr.c_ptr());
	SpeedTree::CTree*			base_tree		= static_cast_checked<SpeedTree::CTree*>(tree);
	
	if (!m_forest->TreeIsRegistered(base_tree))
	{
		m_forest->RegisterTree	(base_tree);
		
		m_trees.push_back		(st_instance_ptr->m_speedtree_tree_ptr);
		
		SLodProfile sLodProfile	= tree->GetLodProfile( );
		sLodProfile.Scale		(scale_speedtree_to_xray );
		tree->SetLodProfile		(sLodProfile);
	}
	
	m_tree_instances.push_back					(st_instance_ptr);
	speedtree_instance_impl*	instance_impl	= static_cast_checked<speedtree_instance_impl*>(st_instance_ptr.c_ptr());
	instance_impl->set_transform				(transform);
	
	// TODO: Add editor button "Populate Forest" for this.
	// Call populate_forest when the level is loaded.
//.	populate_forest								();
}

void speedtree_forest::remove_instance(speedtree_instance_ptr st_instance_ptr)
{
	speedtree_instance_impl*	instance_impl			= static_cast_checked<speedtree_instance_impl*>(st_instance_ptr.c_ptr());
	speedtree_tree*				tree					= static_cast_checked<speedtree_tree*>(instance_impl->m_speedtree_tree_ptr.c_ptr());
	SpeedTree::CTree*			base_tree				= static_cast_checked<SpeedTree::CTree*>(tree);
	
	SpeedTree::CInstance		speedtree_instance		= instance_impl->get_speedtree_instance();
	
	bool const result			= get_forest().DeleteInstances(
		base_tree, 
		&speedtree_instance, 
		1, 
		true
		);
	
	(void)&result;
	
	tree_instances_type::iterator found			= std::find(m_tree_instances.begin(), m_tree_instances.end(), st_instance_ptr);
	if (found != m_tree_instances.end())
	{
		TInstanceArray							instances_of_tree;
		get_forest().GetInstances				(base_tree, instances_of_tree);
		
		if (!instances_of_tree.size())
		{
			get_forest().UnregisterTree			(base_tree);
			
			trees_type::iterator found_tree		= std::find(m_trees.begin(), m_trees.end(), instance_impl->m_speedtree_tree_ptr);
			if (found_tree != m_trees.end())
				m_trees.erase					(found_tree);
		}
		
		m_tree_instances.erase					(found);
	}
	else
	{
		LOG_ERROR("Cant find speedtree instance.");
		return;
	}
	
	// TODO: Add editor button "Populate Forest" for this.
	//populate_forest();
	//get_forest().UpdateTreeCellExtents();
//.	populate_forest();
	print_speedtree_errors();
}

void speedtree_forest::set_transform(speedtree_instance_ptr st_instance_ptr, math::float4x4 const& transform)
{
	speedtree_instance_impl*	instance_impl			= static_cast_checked<speedtree_instance_impl*>(st_instance_ptr.c_ptr());
	speedtree_tree*				tree					= static_cast_checked<speedtree_tree*>(instance_impl->m_speedtree_tree_ptr.c_ptr());
	SpeedTree::CTree*			base_tree				= static_cast_checked<SpeedTree::CTree*>(tree);
	
	SpeedTree::CInstance		old_speedtree_instance	= instance_impl->get_speedtree_instance();
	instance_impl->set_transform						(transform);
	SpeedTree::CInstance		new_speedtree_instance	= instance_impl->get_speedtree_instance();
	
	bool const result			= get_forest().ChangeInstance(
		base_tree, 
		old_speedtree_instance, 
		new_speedtree_instance
		);
	
	(void)&result;
	
	// force cull engine to adjust to the changes
	//get_forest().UpdateTreeCellExtents();
//.	populate_forest();
}

} // namespace render
} // namespace xray