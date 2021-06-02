////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_SPEEDTREE_TREE_COMPONENT_BILLBOARD_H_INCLUDED
#define XRAY_RENDER_SPEEDTREE_TREE_COMPONENT_BILLBOARD_H_INCLUDED

#include "speedtree_tree.h"

namespace SpeedTree {
	struct SIndexedTriangles;
}

namespace xray {
namespace render {

class speedtree_tree_component_billboard: public speedtree_tree_component
{
public:
										speedtree_tree_component_billboard	(speedtree_tree& parent);
			void						init								(speedtree_forest& forest, SpeedTree::TInstanceArray const& instances_of_tree);
	virtual void						set_material						(material_ptr mtl_ptr);
	virtual void						set_default_material				();
	virtual SpeedTree::EGeometryType	get_geometry_type					() const { return SpeedTree::GEOMETRY_TYPE_VERTICAL_BILLBOARDS; };
	virtual void						render								(lod_entry const*, renderer_context*);
	virtual	enum_vertex_input_type		get_vertex_input_type				() { return xray::render::speedtree_billboard_vertex_input_type; }
			bool						is_initialized						() const { return m_is_init; };
			bool						m_is_init;
}; // class speedtree_tree_component_billboard

} // namespace render 
} // namespace xray 

#endif // #ifndef XRAY_RENDER_SPEEDTREE_TREE_COMPONENT_BILLBOARD_H_INCLUDED