////////////////////////////////////////////////////////////////////////////
//	Module 		: string_stream_inline.h
//	Created 	: 15.06.2005
//  Modified 	: 25.08.2007
//	Author		: Dmitriy Iassenev
//	Description : string stream class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_STRING_STREAM_INLINE_H_INCLUDED
#define CS_CORE_STRING_STREAM_INLINE_H_INCLUDED

inline pcvoid string_stream::buffer			() const
{
	return			m_buffer;
}

inline u32  string_stream::buffer_size() const
{
	return			m_buffer_size;
}

inline void string_stream::clear			()
{
	m_buffer_size	= 0;
}

#endif // #ifndef CS_CORE_STRING_STREAM_INLINE_H_INCLUDED