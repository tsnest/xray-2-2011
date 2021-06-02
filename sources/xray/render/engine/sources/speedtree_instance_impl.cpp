////////////////////////////////////////////////////////////////////////////
//	Created		: 30.05.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "speedtree_instance_impl.h"
#include "speedtree_convert_type.h"

namespace xray {
namespace render {

speedtree_instance_impl::speedtree_instance_impl(render::speedtree_tree_ptr in_speedtree_tree_ptr):
	speedtree_instance(in_speedtree_tree_ptr),
	m_speedtree_instance_index(-1)
{
	m_speedtree_instance = NEW(SpeedTree::CInstance);
}

speedtree_instance_impl::~speedtree_instance_impl()
{
	DELETE(m_speedtree_instance);
}

void speedtree_instance_impl::set_transform(float4x4 const& transform)
{
	m_transform							= transform;
	float3 rotation_angles				= transform.get_angles(math::rotation_zxy);
	float3 scale						= transform.get_scale();
	m_speedtree_instance->SetPos		(xray_to_speedtree(transform.lines[3].xyz()));
	m_speedtree_instance->SetRotation	(-rotation_angles.y);
	m_speedtree_instance->SetScale		(math::max(scale.x, math::max(scale.y, scale.z)));
}

SpeedTree::CInstance const& speedtree_instance_impl::get_speedtree_instance() const
{
	return *m_speedtree_instance;
}

} // namespace render
} // namespace xray
