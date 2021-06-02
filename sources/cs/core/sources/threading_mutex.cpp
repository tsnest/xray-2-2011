////////////////////////////////////////////////////////////////////////////
//	Module 		: threading_mutex.cpp
//	Created 	: 26.08.2006
//  Modified 	: 26.08.2006
//	Author		: Dmitriy Iassenev
//	Description : threading mutex class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <cs/core/threading_mutex.h>
#include <cs/core/os_include.h>			// mutex_type and the others

#if defined(_MSC_VER)
	typedef CRITICAL_SECTION	mutex_type;
#elif defined(__GNUC__) // #if defined(_MSC_VER)
#	include <sys/synchronization.h>
	typedef sys_lwmutex_t		mutex_type;
#else // #elif defined(__GNUC__)
#	error typedef mutex type for your platform here
#endif // #if defined(_MSC_VER)

threading::mutex::mutex			()
{
	STATIC_CHECK				( sizeof(mutex_type) == sizeof(m_mutex), buffer_size_doesnt_correspond_to_the_actual_length );

#if defined(_MSC_VER)
	InitializeCriticalSection	( (mutex_type*)&m_mutex );
#elif defined(__GNUC__) // #if defined(_MSC_VER)
	sys_lwmutex_attribute_t		attribute;
	attribute.attr_protocol		= SYS_SYNC_PRIORITY_INHERIT;
	attribute.attr_recursive	= SYS_SYNC_RECURSIVE;
	attribute.name[0]			= 0;
	sys_lwmutex_create			( (mutex_type*)&m_mutex, &attribute );
#else // #elif defined(__GNUC__)
#	error implement mutex initialization in your platform
#endif // #if defined(_MSC_VER)
}

threading::mutex::~mutex		()
{
#if defined(_MSC_VER)
	DeleteCriticalSection		( (mutex_type*)&m_mutex );
#elif defined(__GNUC__) // #if defined(_MSC_VER)
	sys_lwmutex_destroy			( (mutex_type*)&m_mutex );
#else // #elif defined(__GNUC__)
#	error implement mutex uninitialization in your platform
#endif // #if defined(_MSC_VER)
}

void threading::mutex::lock		()
{
//	string256					temp;
//	sprintf_s					(temp, "lock   0x%x[%d]\r\n", *(u32*)&m_mutex, GetCurrentThreadId());
//	OUTPUT_DEBUG_STRING			(temp);
#if defined(_MSC_VER)
	EnterCriticalSection		( (mutex_type*)&m_mutex );
#elif defined(__GNUC__) // #if defined(_MSC_VER)
	sys_lwmutex_lock			( (mutex_type*)&m_mutex, 0 );
#else // #elif defined(__GNUC__)
#	error implement lock for mutex in your platform
#endif // #if defined(_MSC_VER)
}

void threading::mutex::unlock	()
{
//	string256					temp;
//	sprintf_s					(temp, "unlock 0x%x[%d]\r\n", *(u32*)&m_mutex, GetCurrentThreadId());
//	OUTPUT_DEBUG_STRING			(temp);
#if defined(_MSC_VER)
	LeaveCriticalSection		( (mutex_type*)&m_mutex );
#elif defined(__GNUC__) // #if defined(_MSC_VER)
	sys_lwmutex_unlock			( (mutex_type*)&m_mutex );
#else // #elif defined(__GNUC__)
#	error implement unlock for mutex in your platform
#endif // #if defined(_MSC_VER)
}

bool threading::mutex::trylock	()
{
#if defined(_MSC_VER)
	return						!!TryEnterCriticalSection((mutex_type*)&m_mutex);
#elif defined(__GNUC__) // #if defined(_MSC_VER)
	return						sys_lwmutex_trylock( (mutex_type*)&m_mutex ) == CELL_OK;
#else // #elif defined(__GNUC__)
#	error implement try_lock for mutex in your platform
#endif // #if defined(_MSC_VER)
}
