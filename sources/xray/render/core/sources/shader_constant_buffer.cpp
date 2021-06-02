////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/shader_constant_buffer.h>
#include <xray/render/core/device.h>
#include <xray/render/core/resource_manager.h>
#include "com_utils.h"

namespace xray { 
namespace render {

shader_constant_buffer::shader_constant_buffer( name_string_type const & name, enum_shader_type dest, D3D_CBUFFER_TYPE type, u32 size):
m_name			( name),
m_type			( type),
m_dest			( dest),
m_buffer_size	( size),
m_changed		( true),
m_is_registered	( false )
{
	D3D_BUFFER_DESC	desc;
	desc.ByteWidth		= m_buffer_size;
	desc.Usage			= D3D_USAGE_DYNAMIC;
	desc.BindFlags		= D3D_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;
	desc.MiscFlags		= 0;

	HRESULT res = device::ref().d3d_device()->CreateBuffer( &desc, 0, &m_hardware_buffer);
	CHECK_RESULT(res);
	ASSERT( m_hardware_buffer);

	m_buffer_data = ALLOC( byte, m_buffer_size);
	ASSERT( m_buffer_data);

	memset( m_buffer_data, 0, m_buffer_size);
}


void shader_constant_buffer::destroy_impl() const
{
	resource_manager::ref().release( this);
}

shader_constant_buffer::~shader_constant_buffer()
{
	//	Flush();
	safe_release	( m_hardware_buffer);
	FREE			( m_buffer_data);
}

bool shader_constant_buffer::similiar( shader_constant_buffer const& other) const
{
	return ( m_buffer_size == other.m_buffer_size) && ( m_type == other.m_type) && ( m_dest == other.m_dest) && (m_name == other.m_name);
}

void shader_constant_buffer::update()
{
	if ( m_changed)
	{
		void	*data;
#if USE_DX10
		HRESULT res = m_hardware_buffer->Map( D3D_MAP_WRITE_DISCARD, 0, &data);
		CHECK_RESULT( res);
#else
		D3D11_MAPPED_SUBRESOURCE	pSubRes;
		HRESULT res = device::ref().d3d_context()->Map( m_hardware_buffer, 0, D3D_MAP_WRITE_DISCARD, 0, &pSubRes);
		CHECK_RESULT( res);
		data = pSubRes.pData;
#endif

		ASSERT( data);
		ASSERT( m_buffer_data);
		CopyMemory( data, m_buffer_data, m_buffer_size);


#if USE_DX10
		m_hardware_buffer->Unmap();
#else
		device::ref().d3d_context()->Unmap(  m_hardware_buffer, 0);
#endif
		m_changed = false;
	}
}

} // namespace render
} // namespace xray
