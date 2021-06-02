////////////////////////////////////////////////////////////////////////////
//	Created 	: 08.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_SPEEDTREE_CONVERT_TYPE_H_INCLUDED
#define XRAY_RENDER_SPEEDTREE_CONVERT_TYPE_H_INCLUDED

#include <xray/render/facade/model.h>

namespace SpeedTree {
	class Vec3;
	class Mat4x4;
}

namespace xray {
namespace render {

SpeedTree::Vec3		xray_to_speedtree (math::float3 const& v);
SpeedTree::Vec4		xray_to_speedtree (math::float4 const& v);
SpeedTree::Mat4x4	xray_to_speedtree (math::float4x4 const& m);

math::float3		speedtree_to_xray (SpeedTree::Vec3 const& v);
math::float4		speedtree_to_xray (SpeedTree::Vec4 const& v);
math::float4x4		speedtree_to_xray (SpeedTree::Mat4x4 const& m);

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_SPEEDTREE_CONVERT_TYPE_H_INCLUDED