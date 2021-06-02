////////////////////////////////////////////////////////////////////////////
//	Created		: 30.06.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "threading_functions.h"

namespace xray {
namespace threading {

inline
u32   create_tls_key_mt_safe			(threading::atomic32_type &		key, 
										 threading::atomic32_type &		key_created_flag)
{
	if ( !key )
	{
		if ( threading::interlocked_increment(key_created_flag) == 1 )
		{
			threading::interlocked_exchange	(key, threading::tls_create_key());
		}
		else
			while ( !key ) {;}		
	}

	return									key;
}

template <class T>
struct value_and_allocator
{
	T 									value;
	memory::base_allocator *			allocator;
};

template <class T>
T *   get_or_create_tls_value			(u32 const key, memory::base_allocator * const allocator)
{
	value_and_allocator<T> * tls		=	(value_and_allocator<T> * )threading::tls_get_value(key);
	if ( tls )
		return								& tls->value;

	if ( !allocator )
		return								NULL;

	tls									=	XRAY_NEW_IMPL(allocator, value_and_allocator<T>);
	tls->allocator						=	allocator;
	threading::tls_set_value				(key, tls);
	
	return									& tls->value;
}

template <class T>
void   free_tls_value					(u32 const key)
{
	value_and_allocator<T> *	tls		=	(value_and_allocator<T> *)threading::tls_get_value(key);
	if ( !tls )
		return;

	memory::base_allocator * allocator	=	tls->allocator;
	XRAY_DELETE_IMPL						(allocator, tls);
	threading::tls_set_value				(key, NULL);
}

} // namespace threading
} // namespace xray
