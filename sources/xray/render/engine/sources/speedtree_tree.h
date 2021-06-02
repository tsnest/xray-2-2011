////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_SPEEDTREE_TREE_H_INCLUDED
#define XRAY_RENDER_SPEEDTREE_TREE_H_INCLUDED

#include "render_model.h"
#include <xray/render/facade/vertex_input_type.h>

namespace SpeedTree {
	class CCore;
}

namespace xray {
namespace render {

// Add render_object interface

class material;
typedef	resources::resource_ptr<material,resources::unmanaged_intrusive_base> material_ptr;

struct material_effects_instance;
typedef	resources::resource_ptr<material_effects_instance, resources::unmanaged_intrusive_base> material_effects_instance_ptr;

class renderer_context;
class speedtree_tree;
class speedtree_tree_component_billboard;
class speedtree_tree_component_branch;

struct lod_entry
{
	lod_entry(): start_index(0), num_indices(0) {}
	u32 start_index;
	u32 num_indices;
}; // struct lod_entry

struct lod_render_info
{
	lod_render_info(): lods(0), num_lods(0) {}
	
	lod_entry*	lods;
	u32			num_lods;
	
	inline bool has_geometry(u32 lod_index) const 
	{
		lod_entry& l = lods[lod_index];
		return l.num_indices!=0;
	}

	inline lod_entry const* is_active(u32 lod_index) const 
	{
		lod_entry* l = &lods[lod_index];
		
		if (l->num_indices==0)
			return NULL;
		
		return l;
	}
}; // struct lod_render_info

class speedtree_tree_component
{
public:
										speedtree_tree_component	(speedtree_tree& parent);
	virtual								~speedtree_tree_component	();
	virtual void						set_material				(material_ptr mtl_ptr) = 0;
	virtual void						set_default_material		() = 0;
	virtual void						render						(lod_entry const*, renderer_context*)	= 0;
	virtual SpeedTree::EGeometryType	get_geometry_type			() const = 0;
			void						set_material_effects		(material_effects_instance_ptr mtl_instance_ptr, pcstr material_name);
	material_effects&					get_material_effects		();
	virtual enum_vertex_input_type		get_vertex_input_type		() = 0;
	
public:
	speedtree_tree*						m_parent;
	render_geometry						m_render_geometry;
	
protected:
	//material_effects					m_material_effects;
	material_effects_instance_ptr		m_materail_effects_instance;
}; // class speedtree_tree_component


class speedtree_tree: 
	public speedtree_tree_base,
	public SpeedTree::CTree
{
public:
	enum component_type
	{
		branch = 0,
		frond,
		leafmesh,
		leafcard,
		billboard,
	};
										speedtree_tree			(pcvoid data, u32 size);
	virtual								~speedtree_tree			();
	
	lod_render_info const&				get_lod_render_info		(SpeedTree::EGeometryType type) const;
	lod_render_info&					get_lod_render_info		(SpeedTree::EGeometryType type);
	
	void								set_material_effects	(material_effects_instance_ptr mtl_instance_ptr, 
																 component_type in_component_type, 
																 pcstr material_name);
	
protected:
	void load(pcvoid data, u32 size);
	
	friend class						speedtree_tree_component;
	friend struct						speedtree_cook;
	friend struct						speedtree_forest;
	
	speedtree_tree_component*			m_branch_component;
	speedtree_tree_component*			m_frond_component;
	speedtree_tree_component*			m_leafmesh_component;
	speedtree_tree_component*			m_leafcard_component;
	speedtree_tree_component_billboard*	m_billboard_component;
	
	lod_render_info						m_lod_render_info[SpeedTree::GEOMETRY_TYPE_COUNT];
}; // class speedtree_tree

} // namespace render 
} // namespace xray 

#endif // #ifndef XRAY_RENDER_SPEEDTREE_TREE_H_INCLUDED