////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_SPEEDTREE_TREE_COMPONENT_LEAFCARD_H_INCLUDED
#define XRAY_RENDER_SPEEDTREE_TREE_COMPONENT_LEAFCARD_H_INCLUDED

#include "speedtree_tree.h"

namespace SpeedTree {
	struct SLeafCards;
}

namespace xray {
namespace render {

class speedtree_tree_component_leafcard: public speedtree_tree_component
{
public:
										speedtree_tree_component_leafcard	(speedtree_tree& parent);
	virtual void						set_material						(material_ptr mtl_ptr);
	virtual void						set_default_material				();
	virtual SpeedTree::EGeometryType	get_geometry_type					() const { return SpeedTree::GEOMETRY_TYPE_LEAF_CARDS; };
	virtual void						render								(lod_entry const*, renderer_context*);
	virtual enum_vertex_input_type		get_vertex_input_type				() { return xray::render::speedtree_leafcard_vertex_input_type; }
			void						init_index_buffer					(SpeedTree::SLeafCards const* lod, render::vector<u16>& out_indices, u32 num_accumulated_vertices);
}; // class speedtree_tree_component_leafmesh

} // namespace render 
} // namespace xray 

#endif // #ifndef XRAY_RENDER_SPEEDTREE_TREE_COMPONENT_LEAFCARD_H_INCLUDED