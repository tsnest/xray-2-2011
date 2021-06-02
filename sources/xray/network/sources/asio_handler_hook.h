////////////////////////////////////////////////////////////////////////////
//	Created		: 11.12.2009
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ASIO_HANDLER_HOOK_H_INCLUDED
#define ASIO_HANDLER_HOOK_H_INCLUDED

#include "network_memory.h"

namespace boost {
namespace asio {

template<typename Handler>
inline void* asio_handler_allocate(std::size_t size, Handler* h)
{
	h;
	return XN_MALLOC((u32)size, "asio_handler_allocate");
}

template<typename Handler>
inline void asio_handler_deallocate(void* pointer, std::size_t size, Handler* h)
{
	size;
	h;
	XN_FREE(pointer);
}

} // namespace asio
} // namespace boost

#endif // #ifndef ASIO_HANDLER_HOOK_H_INCLUDED