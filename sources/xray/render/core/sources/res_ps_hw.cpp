////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/res_ps_hw.h>
#include <xray/render/core/res_xs_hw.h>
#include <xray/render/core/device.h>
#include <xray/render/core/res_xs_hw_impl.h>

namespace xray {
namespace render {

template<>
HRESULT res_xs_hw<ps_data>::create_hw_shader( ID3DBlob* shader_code, ps_data::hw_interface **hardware_shader)
{
	void const* bytecode_buffer	= shader_code->GetBufferPointer();
	u32 bytecode_buffer_size	= (u32)shader_code->GetBufferSize();

#if USE_DX10
	return device::ref().d3d_device()->CreatePixelShader( bytecode_buffer, bytecode_buffer_size, hardware_shader);
#else
	return device::ref().d3d_device()->CreatePixelShader( bytecode_buffer, bytecode_buffer_size, NULL, hardware_shader);
#endif
}

// template<>
// void res_xs_hw<ps_data>::apply()
// {
// 
// }


template class res_xs_hw<ps_data>;

} // namespace render
} // namespace xray
