////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

namespace xray {
namespace render{
namespace utils{

float3		wform	(float4x4 const& m, float3& v)
{
	math::float4	r;
	r.x			= v.x*m.e00 + v.y*m.e10 + v.z*m.e20 + m.e30;
	r.y			= v.x*m.e01 + v.y*m.e11 + v.z*m.e21 + m.e31;
	r.z			= v.x*m.e02 + v.y*m.e12 + v.z*m.e22 + m.e32;
	r.w			= v.x*m.e03 + v.y*m.e13 + v.z*m.e23 + m.e33;
	// ASSERT		(r.w>0.f);
	float const invW = 1.0f/r.w;
	float3	r3(r.x*invW, r.y*invW, r.z*invW);
	return		r3;
}

}// namespace utils
}// namespace render
}// namespace xray
