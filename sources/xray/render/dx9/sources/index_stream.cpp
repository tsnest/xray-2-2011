////////////////////////////////////////////////////////////////////////////
//	Created		: 23.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "index_stream.h"

namespace xray {
namespace render{

int	rs_dib_size = 512;

void index_stream::create()
{
	resource_manager::get_ref().evict();

	m_size = rs_dib_size*1024;

#ifdef	USE_DX10
	d3d10_buffer_desc bufferdesc;
	bufferdesc.bytewidth        = m_size;
	bufferdesc.usage            = d3d10_usage_dynamic;	
	bufferdesc.bindflags        = d3d10_bind_index_buffer;
	bufferdesc.cpuaccessflags   = d3d10_cpu_access_write;
	bufferdesc.miscflags        = 0;

	R_CHK					(hw.pdevice->createbuffer( &bufferdesc, 0, &pib ));
#else	//	use_dx10
	R_CHK(hw_wrapper::get_ref().device()->CreateIndexBuffer(m_size, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_ib, NULL));
#endif	//	use_dx10
	R_ASSERT(m_ib);

	m_position = 0;
	m_discard_id = 0;

	LOG_INFO("* dib created: %dk", m_size/1024);
}

void index_stream::destroy()
{
	safe_release(m_ib);
	clear();
}

u16* index_stream::lock(u32 i_count, u32& i_offset)
{
//	LOG_INFO("pgo:ib_lock:%d", i_count);
	i_offset = 0;
	void* locked_data = 0;

	// ensure there is enough space in the vb for this data
	R_ASSERT((2*i_count<=m_size) && i_count);

	// if either user forced us to flush,
	// or there is not enough space for the index data,
	// then flush the buffer contents
	u32 flags = D3DLOCK_NOOVERWRITE;
	if (2*(i_count + m_position) >= m_size)
	{
		m_position = 0;						// clear position
		flags = D3DLOCK_DISCARD;			// discard it's contens
		m_discard_id++;
	}
#ifdef	USE_DX10
	d3d10_map mapmode = (dwflags==lockflags_append) ? 
		d3d10_map_write_no_overwrite : d3d10_map_write_discard;
	pib->map( mapmode, 0, (void**)&plockeddata);
	plockeddata += mposition * 2;

#else	//	use_dx10
	m_ib->Lock(m_position*2, i_count*2, (void**)&locked_data, flags);
#endif	//	use_dx10
	ASSERT(locked_data);

	i_offset = m_position;

	return	static_cast<u16*>(locked_data);
}

void	index_stream::unlock(u32 i_count)
{
//	LOG_INFO("pgo:ib_unlock:%d", i_count);
	m_position += i_count;
	ASSERT(m_ib);
#ifdef	USE_DX10
	pib->unmap();
#else	//	use_dx10
	m_ib->Unlock();
#endif	//	use_dx10
}

void index_stream::reset_begin()
{
	old_ib = m_ib;
	destroy();
}
void index_stream::reset_end()
{
	create();
	//old_pib = null;
}

} // namespace render
} // namespace xray

