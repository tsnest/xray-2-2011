////////////////////////////////////////////////////////////////////////////
//	Created		: 30.05.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_SPEEDTREE_INSTANCE_IMPL_H_INCLUDED
#define XRAY_RENDER_SPEEDTREE_INSTANCE_IMPL_H_INCLUDED

#include <xray/render/facade/model.h>

namespace SpeedTree {
	class CInstance;
}

namespace xray {
namespace render {

class speedtree_instance_impl: public xray::render::speedtree_instance
{
public:
								speedtree_instance_impl		(render::speedtree_tree_ptr in_speedtree_tree_ptr);
	virtual						~speedtree_instance_impl	();
	void						set_transform				(float4x4 const& transform);
	SpeedTree::CInstance const&	get_speedtree_instance		() const;
	
protected:
								friend struct speedtree_forest;
	SpeedTree::CInstance*		m_speedtree_instance;
	s32							m_speedtree_instance_index;
}; // class speedtree_instance_impl


} // namespace render 
} // namespace xray 

#endif // #ifndef XRAY_RENDER_SPEEDTREE_INSTANCE_IMPL_H_INCLUDED