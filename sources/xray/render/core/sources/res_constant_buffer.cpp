////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/device.h>
#include <xray/render/core/res_constant_buffer.h>
#include <xray/render/core/resource_manager.h>

namespace xray { 
namespace render_dx10 {

res_constant_buffer::res_constant_buffer( name_string const & name, enum_shader_type dest, D3D_CBUFFER_TYPE type, u32 size):
m_name			( name),
m_type			( type),
m_dest			( dest),
m_buffer_size	( size),
m_changed		( true)
{
	D3D_BUFFER_DESC	desc;
	desc.ByteWidth		= m_buffer_size;
	desc.Usage			= D3D_USAGE_DYNAMIC;
	desc.BindFlags		= D3D_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;
	desc.MiscFlags		= 0;

	HRESULT res = device::ref().d3d_device()->CreateBuffer( &desc, 0, &m_hw_buffer);
	R_CHK(res);
	ASSERT( m_hw_buffer);

	m_buffer_data = ALLOC( byte, m_buffer_size);
	ASSERT( m_buffer_data);

	memset( m_buffer_data, 0, m_buffer_size);
}


void res_constant_buffer::_free()
{
	resource_manager::ref().release( this);
}

res_constant_buffer::~res_constant_buffer()
{
	//	Flush();
	safe_release	( m_hw_buffer);
	FREE			( m_buffer_data);
}

bool res_constant_buffer::similiar( res_constant_buffer & other)
{
	return ( m_buffer_size == other.m_buffer_size) && ( m_type == other.m_type) && ( m_dest == other.m_dest) && (m_name == other.m_name);
}

void res_constant_buffer::update()
{
	if ( m_changed)
	{
		void	*data;
#if USE_DX10
		HRESULT res = m_hw_buffer->Map( D3D_MAP_WRITE_DISCARD, 0, &data);
		R_CHK( res);
#else
		D3D11_MAPPED_SUBRESOURCE	pSubRes;
		HRESULT res = device::ref().d3d_context()->Map( m_hw_buffer, 0, D3D_MAP_WRITE_DISCARD, 0, &pSubRes);
		R_CHK( res);
		data = pSubRes.pData;
#endif

		ASSERT( data);
		ASSERT( m_buffer_data);
		CopyMemory( data, m_buffer_data, m_buffer_size);


#if USE_DX10
		m_hw_buffer->Unmap();
#else
		device::ref().d3d_context()->Unmap(  m_hw_buffer, 0);
#endif
		m_changed = false;
	}
}

} // namespace render
} // namespace xray
