////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_UNTYPED_BUFFER_INLINE_H_INCLUDED
#define XRAY_RENDER_CORE_UNTYPED_BUFFER_INLINE_H_INCLUDED

#if !USE_DX10
#	include <xray/render/core/device.h>
#endif // #if !USE_DX10

namespace xray {
namespace render {

pvoid untyped_buffer::map	( D3D_MAP const mode )
{
#if USE_DX10
	pvoid								result;
	m_hardware_buffer->Map				( mode, 0, &result );
	return								result;
#else // #if USE_DX10
	D3D11_MAPPED_SUBRESOURCE			mapped_result;
	device::ref().d3d_context()->Map	( m_hardware_buffer, 0, mode, 0, &mapped_result );
	return								mapped_result.pData;
#endif // #if USE_DX10
}

void untyped_buffer::unmap	( )
{
#if USE_DX10
	m_hw_buffer->Unmap					( );
#else // #if USE_DX10
	device::ref().d3d_context()->Unmap	( m_hardware_buffer, 0 );
#endif // #if USE_DX10
}

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_UNTYPED_BUFFER_INLINE_H_INCLUDED