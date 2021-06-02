////////////////////////////////////////////////////////////////////////////
//	Created		: 23.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "vertex_stream.h"

namespace xray {
namespace render{

int	rs_dvb_size = 512+1024;

void vertex_stream::create()
{
	resource_manager::get_ref().evict();
	m_size = rs_dvb_size*1024;

#ifdef	USE_DX10
	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth        = mSize;
	bufferDesc.Usage            = D3D10_USAGE_DYNAMIC;	
	bufferDesc.BindFlags        = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags   = D3D10_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags        = 0;

	R_CHK					(HW.pDevice->CreateBuffer( &bufferDesc, 0, &pVB ));
#else	//	USE_DX10
	R_CHK(hw_wrapper::get_ref().device()->CreateVertexBuffer( m_size, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_vb, NULL));
#endif	//	USE_DX10

	R_ASSERT(m_vb);

	m_position = 0;
	m_discard_id = 0;

	LOG_INFO("* DVB created: %dK", m_size/1024);
}

void vertex_stream::destroy	()
{
	safe_release(m_vb);
	clear();
}

void* vertex_stream::lock(u32 v_count, u32 v_stride, u32& v_offset)
{
#ifdef DEBUG
//	LOG_INFO("PGO:VB_LOCK:%d",v_count);
	ASSERT(0==m_dbg_lock);
	m_dbg_lock++;
#endif

	// Ensure there is enough space in the VB for this data
	u32	bytes_need = v_count * v_stride;
	R_ASSERT(bytes_need<=m_size && v_count);

	// Vertex-local info
	u32 vl_size     = m_size/v_stride;
	u32 vl_position	= m_position/v_stride + 1;

	// Check if there is need to flush and perform lock
	pcbyte p_data			= 0;
	if (v_count+vl_position >= vl_size)
	{
		// FLUSH-LOCK
		m_position = 0;
		v_offset = 0;
		m_discard_id++;

#ifdef	USE_DX10
		pVB->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&pData);
		pData += vOffset;
#else	//	USE_DX10
		m_vb->Lock( m_position, bytes_need, (void**)&p_data, D3DLOCK_DISCARD);
#endif	//	USE_DX10
	}
	else
	{
		// APPEND-LOCK
		m_position = vl_position*v_stride;
		v_offset = vl_position;

#ifdef	USE_DX10
		pVB->Map(D3D10_MAP_WRITE_NO_OVERWRITE, 0, (void**)&pData);
		pData += vOffset*Stride;
#else	//	USE_DX10
		m_vb->Lock( m_position, bytes_need, (void**)&p_data, D3DLOCK_NOOVERWRITE);
#endif	//	USE_DX10
	}
	
	ASSERT(p_data);

	return LPVOID(p_data);
}

void vertex_stream::unlock(u32 v_count, u32 v_stride /* Need to remove this parameter*/)
{
#ifdef DEBUG
//	LOG_INFO("PGO:VB_UNLOCK:%d",v_count);
	ASSERT(1==m_dbg_lock);
	m_dbg_lock--;
#endif
	
	m_position += v_count*v_stride;

	ASSERT(m_vb);

#ifdef	USE_DX10
	pVB->Unmap();
#else	//	USE_DX10
	m_vb->Unlock();
#endif	//	USE_DX10
}

void	vertex_stream::reset_begin	()
{
	old_vb = m_vb;
	destroy();
}
void	vertex_stream::reset_end	()
{
	create				();
	//old_pVB				= NULL;
}

vertex_stream::vertex_stream()
{
	clear();
};

void vertex_stream::clear()
{
    m_vb = NULL;
    m_size = 0;
    m_position = 0;
    m_discard_id = 0;
#ifdef DEBUG
	m_dbg_lock = 0;
#endif
}

} // namespace render
} // namespace xray

