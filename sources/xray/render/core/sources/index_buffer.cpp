////////////////////////////////////////////////////////////////////////////
//	Created		: 23.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/index_buffer.h>
#include <xray/render/core/resource_manager.h>

namespace xray {
namespace render {

int	rs_dib_size = 512;

index_buffer::index_buffer( u32 const size ) :
	m_size			( size ),
	m_position		( 0 ),
	m_discard_id	( 0 ),
	m_lock_size		( 0 )
{
	m_buffer		= resource_manager::ref().create_buffer( m_size, 0, enum_buffer_type_index, true);
	R_ASSERT		( m_buffer );
	LOG_INFO		("index buffer created: %dKb", m_size/1024);
}

u16* index_buffer::lock(u32 i_count, u32& i_offset)
{
//	LOG_INFO("pgo:ib_lock:%d", i_count);
	i_offset = 0;
	char* locked_data = 0;

	// ensure there is enough space in the vb for this data
	R_ASSERT((2*i_count<=m_size) && i_count);

	// if either user forced us to flush,
	// or there is not enough space for the index data,
	// then flush the buffer contents
	D3D_MAP map_mode = D3D_MAP_WRITE_NO_OVERWRITE;
	if (sizeof(u16)*(i_count + m_position) >= m_size)
	{
		m_position = 0;						// clear position
		map_mode = D3D_MAP_WRITE_DISCARD;		// discard it's content
		m_discard_id++;
	}

	locked_data = (char*)m_buffer->map( map_mode);
	locked_data += m_position*sizeof(u16);

	ASSERT(locked_data);

	i_offset = m_position;

	m_lock_size = i_count;
	return	(u16*)locked_data;
}

void	index_buffer::unlock()
{
//	LOG_INFO("pgo:ib_unlock:%d", i_count);
	m_position += m_lock_size;

	m_buffer->unmap();
}

// void index_buffer::reset_begin()
// {
// 	old_ib = m_buffer;
// 	destroy();
// }
// void index_buffer::reset_end()
// {
// 	create();
// 	//old_pib = null;
// }

} // namespace render
} // namespace xray

