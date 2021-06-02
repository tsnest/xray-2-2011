////////////////////////////////////////////////////////////////////////////
//	Created		: 05.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RENDER_DEVICE_INLINE_H_INCLUDED
#define RENDER_DEVICE_INLINE_H_INCLUDED

namespace xray {
namespace render_dx10 {

u32 device::get_width() const
{
	return m_chain_desc.BufferDesc.Width;
}

u32 device::get_height() const
{
	return m_chain_desc.BufferDesc.Height;
}

xray::math::uint2 device::get_size() const
{
	return xray::math::uint2( m_chain_desc.BufferDesc.Width, m_chain_desc.BufferDesc.Height);
}

} // namespace render
} // namespace xray



#endif // #ifndef RENDER_DEVICE_INLINE_H_INCLUDED