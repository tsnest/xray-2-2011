////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_SPEEDTREE_TREE_COMPONENT_LEAFMESH_H_INCLUDED
#define XRAY_RENDER_SPEEDTREE_TREE_COMPONENT_LEAFMESH_H_INCLUDED

#include "speedtree_tree.h"

namespace SpeedTree {
	struct SIndexedTriangles;
}

namespace xray {
namespace render {

class speedtree_tree_component_leafmesh: public speedtree_tree_component
{
public:
										speedtree_tree_component_leafmesh	(speedtree_tree& parent);
	virtual void						set_material						(material_ptr mtl_ptr);
	virtual void						set_default_material				();
	virtual SpeedTree::EGeometryType	get_geometry_type					() const { return SpeedTree::GEOMETRY_TYPE_LEAF_MESHES; };
	virtual void						render								(lod_entry const*, renderer_context*);
	virtual enum_vertex_input_type		get_vertex_input_type				() { return xray::render::speedtree_leafmesh_vertex_input_type; }
			void						init_index_buffer					(SpeedTree::SIndexedTriangles const* lod, render::vector<u16>& out_indices);
}; // class speedtree_tree_component_leafmesh

} // namespace render 
} // namespace xray 

#endif // #ifndef XRAY_RENDER_SPEEDTREE_TREE_COMPONENT_LEAFMESH_H_INCLUDED