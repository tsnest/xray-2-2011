////////////////////////////////////////////////////////////////////////////
//	Module 		: string_stream.h
//	Created 	: 15.06.2005
//  Modified 	: 25.08.2007
//	Author		: Dmitriy Iassenev
//	Description : string stream class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_STRING_STREAM_H_INCLUDED
#define CS_CORE_STRING_STREAM_H_INCLUDED

class CS_CORE_API string_stream {
private:
	char*	m_buffer;
	u32		m_buffer_size;
	u32		m_allocated_size;

public:
							string_stream	();
							~string_stream	();
			void __cdecl	msg				(pcstr format, ...);
	inline	pcvoid			buffer			() const;
	inline	u32 			buffer_size		() const;
	inline	void			clear			();
};

#include "string_stream_inline.h"

#endif // #ifndef CS_CORE_STRING_STREAM_H_INCLUDED