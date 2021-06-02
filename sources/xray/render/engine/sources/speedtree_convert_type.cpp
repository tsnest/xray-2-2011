////////////////////////////////////////////////////////////////////////////
//	Created 	: 08.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "speedtree_convert_type.h"

namespace xray {
namespace render {

SpeedTree::Vec3 xray_to_speedtree(math::float3 const& v)
{
	return SpeedTree::Vec3(v.elements);
}

SpeedTree::Vec4 xray_to_speedtree(math::float4 const& v)
{
	return SpeedTree::Vec4(v.elements);
}

SpeedTree::Mat4x4 xray_to_speedtree(math::float4x4 const& m)
{
	SpeedTree::Mat4x4 result;
	xray::memory::copy(&result.m_afRowCol[0], sizeof(m.elements), &m.elements[0], sizeof(m.elements));
	return result;
}

math::float3 speedtree_to_xray(SpeedTree::Vec3 const& v)
{
	return math::float3(v.x, v.y, v.z);
}

math::float4 speedtree_to_xray(SpeedTree::Vec4 const& v)
{
	return math::float4(v.x, v.y, v.z, v.w);
}

math::float4x4 speedtree_to_xray(SpeedTree::Mat4x4 const& m)
{
	math::float4x4 result;
	xray::memory::copy(&result.elements[0], sizeof(m.m_afRowCol), &m.m_afRowCol[0], sizeof(m.m_afRowCol));
	return result;
}

} // namespace render
} // namespace xray