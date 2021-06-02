////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

namespace xray {
namespace render {
namespace utils {

float3		wform	(float4x4 const& m, float3& v)
{
	float4	r;
	r.x			= v.x*m.e00 + v.y*m.e10 + v.z*m.e20 + m.e30;
	r.y			= v.x*m.e01 + v.y*m.e11 + v.z*m.e21 + m.e31;
	r.z			= v.x*m.e02 + v.y*m.e12 + v.z*m.e22 + m.e32;
	r.w			= v.x*m.e03 + v.y*m.e13 + v.z*m.e23 + m.e33;
	// ASSERT		(r.w>0.f);
	float const invW = 1.0f/r.w;
	float3	r3(r.x*invW, r.y*invW, r.z*invW);
	return		r3;
}

template <typename list>
bool calc_lists_diff_range( list const& first, list const& second, u32 &min, u32 &max)
{
	list::const_iterator	it	= first.begin();
	list::const_iterator	end = first.end();

	list::const_iterator	it_oth	= second.begin();
	list::const_iterator	end_oth = second.end();

	min = max = 0;
	for( ; it != end && it_oth != end_oth; ++it, ++it_oth, ++min)
		if( *it != *end_oth)
			break;


	if( first.size() != second.size())
	{
		max = math::max( first.size(), second.size());
		return min != max;
	}

	int ind = min;
	for( ; it != end && it_oth != end_oth; ++it, ++it_oth, ++ind)
	{
		if( *it != *end_oth)
		{
			max = ind+1;
//			*it = *it_oth;
		}
	}

	return min != max;
}

//////////////////////////////////////////////////////////////////////////
u32 calc_surface_size ( u32 width, u32 height, DXGI_FORMAT format, u32 row_min_pitch)
{
	u32 row_pitch_stub;
	return calc_surface_size ( width, height, format, row_min_pitch, row_pitch_stub);
}

//////////////////////////////////////////////////////////////////////////
inline u32 log_2( u32 val)
{
#pragma warning( push)
#pragma warning( disable:4146)
#pragma warning( disable:4554)

	ASSERT( (val & -val) != 0 || val == 0, "The function works only for powers of 2." );

	float f = (float)( val & -val); // cast the least significant bit in v to a float
	return (*(u32*)&f >> 23) - 0x7f;	// cast the least significant bit in v to a float

#pragma warning( pop)
}

} // namespace utils
} // namespace render
} // namespace xray
