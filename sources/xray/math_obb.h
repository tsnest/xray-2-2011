////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_OBB_H_INCLUDED
#define XRAY_MATH_OBB_H_INCLUDED

namespace xray {
namespace math {

class XRAY_CORE_API obb : public float4x4 {
public:
			bool	intersect	( float3 const& position, float3 const& direction, type& distance );
}; // class obb

} // namespace math
} // namespace xray

#endif // #ifndef XRAY_MATH_OBB_H_INCLUDED