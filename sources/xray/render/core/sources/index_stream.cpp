////////////////////////////////////////////////////////////////////////////
//	Created		: 23.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/index_stream.h>
#include <xray/render/core/resource_manager.h>

namespace xray {
namespace render_dx10 {

int	rs_dib_size = 512;

void index_stream::create( u32 size)
{
	//m_size = rs_dib_size*1024;
	m_size = size;

	resource_manager::ref().evict();

	m_ib = resource_manager::ref().create_buffer( m_size, 0, enum_buffer_type_index, true);

	R_ASSERT(m_ib);

	m_position = 0;
	m_discard_id = 0;
	m_lock_size = 0;

	LOG_INFO("* dib created: %dk", m_size/1024);
}


u16* index_stream::lock(u32 i_count, u32& i_offset)
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

	locked_data = (char*)m_ib->map( map_mode);
	locked_data += m_position*sizeof(u16);

	ASSERT(locked_data);

	i_offset = m_position;

	m_lock_size = i_count;
	return	(u16*)locked_data;
}

void	index_stream::unlock()
{
//	LOG_INFO("pgo:ib_unlock:%d", i_count);
	m_position += m_lock_size;

	m_ib->unmap();
}

// void index_stream::reset_begin()
// {
// 	old_ib = m_ib;
// 	destroy();
// }
// void index_stream::reset_end()
// {
// 	create();
// 	//old_pib = null;
// }

} // namespace render
} // namespace xray

