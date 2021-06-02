////////////////////////////////////////////////////////////////////////////
//	Created		: 23.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INDEX_STREAM_H_INCLUDED
#define INDEX_STREAM_H_INCLUDED

#include <xray/render/core/untyped_buffer.h>

namespace xray {
namespace render {

class index_buffer {
public:
								index_buffer	( u32 size );
			u16*				lock			( u32 i_count, u32& i_offset );
			void				unlock			( );

public:
	inline	untyped_buffer&		buffer			( ) const;
	inline	u32					discard_id		( ) const;
	inline	void				flush			( );

private :
	untyped_buffer_ptr			m_buffer;
	u32							m_size;			// real size (usually mcount, aligned on 512b boundary)
	u32							m_position;
	u32							m_discard_id;
	u32							m_lock_size;
}; // class index_buffer

} // namespace render 
} // namespace xray 

#include <xray/render/core/index_buffer_inline.h>

#endif // #ifndef INDEX_STREAM_H_INCLUDED