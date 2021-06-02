////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SAMPLER_STATE_SIMULATOR_INLINE_H_INCLUDED
#define SAMPLER_STATE_SIMULATOR_INLINE_H_INCLUDED

namespace xray {
namespace render {

sampler_state_descriptor &	sampler_state_descriptor::reset()
{
	m_desc.Filter			= D3D_FILTER_MIN_MAG_MIP_LINEAR;
	m_desc.AddressU			= D3D_TEXTURE_ADDRESS_CLAMP;
	m_desc.AddressV			= D3D_TEXTURE_ADDRESS_CLAMP;
	m_desc.AddressW			= D3D_TEXTURE_ADDRESS_CLAMP;
	m_desc.MipLODBias		= 0;
	m_desc.MaxAnisotropy	= 1;
	m_desc.ComparisonFunc	= D3D_COMPARISON_NEVER;
	m_desc.BorderColor[0]	= 1.0f;
	m_desc.BorderColor[1]	= 1.0f;
	m_desc.BorderColor[2]	= 1.0f;
	m_desc.BorderColor[3]	= 1.0f;
	m_desc.MinLOD			= -FLT_MAX;
	m_desc.MaxLOD			= FLT_MAX;

	return *this;
}

sampler_state_descriptor &	sampler_state_descriptor::set					( D3D_FILTER filter, D3D_TEXTURE_ADDRESS_MODE addr_mode)
{
	m_desc.Filter	= filter;
	m_desc.AddressU = addr_mode;
	m_desc.AddressV = addr_mode;
	m_desc.AddressW = addr_mode;

	return *this;
}
sampler_state_descriptor &	sampler_state_descriptor::set_filter			( D3D_FILTER filter)
{
	m_desc.Filter = filter;

	return *this;
}
sampler_state_descriptor &	sampler_state_descriptor::set_address_mode	( D3D_TEXTURE_ADDRESS_MODE addr_u, D3D_TEXTURE_ADDRESS_MODE addr_v, D3D_TEXTURE_ADDRESS_MODE addr_w)
{
	m_desc.AddressU = addr_u;
	m_desc.AddressV = addr_v;
	m_desc.AddressW = addr_w;

	return *this;
}
sampler_state_descriptor &	sampler_state_descriptor::set_mip				( float lod_bias, float min_lod , float max_lod )
{
	m_desc.MipLODBias	= lod_bias;
	m_desc.MinLOD		= min_lod;
	m_desc.MaxLOD		= max_lod;

	return *this;
}
sampler_state_descriptor &	sampler_state_descriptor::set_max_anisotropy	( u32 max_anisotropy)
{
	m_desc.MaxAnisotropy = max_anisotropy;

	return *this;
}
sampler_state_descriptor &	sampler_state_descriptor::set_comparison_function	( D3D_COMPARISON_FUNC func)
{
	m_desc.ComparisonFunc = func;

	return *this;
}
sampler_state_descriptor &	sampler_state_descriptor::set_border_color	( float4 color)
{
	m_desc.BorderColor[0] = color.x;
	m_desc.BorderColor[1] = color.y;
	m_desc.BorderColor[2] = color.z;
	m_desc.BorderColor[3] = color.w;

	return *this;
}

} // namespace render
} // namespace xray

#endif // #ifndef SAMPLER_STATE_SIMULATOR_INLINE_H_INCLUDED