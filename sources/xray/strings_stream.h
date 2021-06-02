////////////////////////////////////////////////////////////////////////////
//	Created		: 08.04.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_STRINGS_STREAM_H_INCLUDED
#define XRAY_STRINGS_STREAM_H_INCLUDED

namespace xray {
namespace strings {

class XRAY_CORE_API stream {
public:
	inline	explicit		stream			( memory::base_allocator* allocator );
	inline					~stream			( );
			void __cdecl	append			( pcstr format, ... );
	inline	pcbyte			get_buffer		( ) const;
	inline	u32				get_buffer_size	( ) const;
	inline	void			clear			( );

private:
	memory::base_allocator*					m_allocator;
	pbyte									m_buffer;
	u32										m_buffer_size;
	u32										m_allocated_size;
}; // class strings_stream

} // namespace strings
} // namespace xray

#include <xray/strings_stream_inline.h>

#endif // #ifndef XRAY_STRINGS_STREAM_H_INCLUDED