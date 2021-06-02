////////////////////////////////////////////////////////////////////////////
//	Created		: 23.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INDEX_STREAM_H_INCLUDED
#define INDEX_STREAM_H_INCLUDED

#include "res_buffer.h"

namespace xray {
namespace render_dx10 {

class index_stream
{
public:
	index_stream	():
	  m_ib			( NULL),
	  m_size		( 0),
	  m_position	( 0),
	  m_discard_id	( 0)
	  {}


	void	create( u32 size);
// 	void	reset_begin();
// 	void	reset_end();

	res_buffer *			buffer		()				{return &*m_ib;}
	ID3DIndexBuffer*		hw_buffer	()				{return m_ib->hw_buffer();}

	u32						discardid	()				{return m_discard_id;}
	void					flush		()				{m_position=m_size;}

	u16*					lock		(u32 i_count, u32& i_offset);
	void					unlock		();

// public:
// 	ID3DIndexBuffer*	old_ib;

private :
	ref_buffer			m_ib;
	u32					m_size;		// real size (usually mcount, aligned on 512b boundary)
	u32					m_position;
	u32					m_discard_id;

	u32					m_lock_size;
}; // class index_stream

} // namespace render 
} // namespace xray 



#endif // #ifndef INDEX_STREAM_H_INCLUDED