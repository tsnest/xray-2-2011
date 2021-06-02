////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/device.h>
#include <xray/render/core/res_buffer.h>
#include <xray/render/core/resource_manager.h>


namespace xray {
namespace render_dx10 {

res_buffer::res_buffer( u32 size, void const * data, enum_buffer_type type, bool dynamic):
m_size			( size),
m_type			( type)
{
	D3D_BUFFER_DESC desc;
	desc.ByteWidth		= size;
	//desc.Usage		= bImmutable ? D3D_USAGE_IMMUTABLE : D3D_USAGE_DEFAULT;
	desc.Usage			= dynamic ? D3D_USAGE_DYNAMIC : D3D_USAGE_DEFAULT;
	desc.BindFlags		= (type == enum_buffer_type_index) ? D3D_BIND_INDEX_BUFFER : D3D_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = dynamic ? D3D_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags		= 0;

	D3D_SUBRESOURCE_DATA init_data;
	init_data.pSysMem = data;

	D3D_SUBRESOURCE_DATA * ptr_init_data = data == NULL ? NULL : &init_data;

	HRESULT res = device::ref().d3d_device()->CreateBuffer( &desc, ptr_init_data, &m_hw_buffer);
	R_CHK	( res);
	ASSERT	( m_hw_buffer);
}

res_buffer::~res_buffer()																	
{ 	
	safe_release( m_hw_buffer);
}

void res_buffer::_free() const			
{ 
	resource_manager::ref().release( const_cast<res_buffer*>(this));
}

} // namespace render
} // namespace xray

