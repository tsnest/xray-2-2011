////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/threading_mutex.h>
#include <sys/synchronization.h>

using xray::threading::mutex;

#define XRAY_USE_LIGHTWEIGHT_MUTEX	1

#if XRAY_USE_LIGHTWEIGHT_MUTEX

typedef sys_lwmutex_t			mutex_type;

mutex::mutex		()
{
	COMPILE_ASSERT				( sizeof( mutex_type ) <= sizeof( m_mutex ), please_change_buffer_size );

	sys_lwmutex_attribute_t		attribute;
	attribute.attr_protocol		= SYS_SYNC_FIFO;
	attribute.attr_recursive	= SYS_SYNC_RECURSIVE;
	attribute.name[0]			= 0;

	int const error				= sys_lwmutex_create( (mutex_type*)&m_mutex, &attribute );
	R_ASSERT_CMP				( error, ==, CELL_OK );

	R_ASSERT					( try_lock() );
#ifndef MASTER_GOLD
	unlock						( );
#endif // #ifndef MASTER_GOLD
}

mutex::~mutex		()
{
	int const error				= sys_lwmutex_destroy( (mutex_type*)&m_mutex );

	if ( error != CELL_OK ) {
		if ( error == EINVAL )
			LOG_ERROR			( "mutex has been destroyed already");
		else if ( error == ESRCH )
			LOG_ERROR			( "user program destroyed mutex data");
		else if ( error == EBUSY )
			LOG_ERROR			( "mutex is busy");
		else
			UNREACHABLE_CODE	( );
	}

	R_ASSERT_CMP				( error, ==, CELL_OK );
}

void mutex::lock	() const
{
//	printf						("lock   0x%x %x\r\n", *(u32*)&m_mutex, current_thread_id());
	int const error				= sys_lwmutex_lock( (mutex_type*)&m_mutex, 0 );
	R_ASSERT_CMP				( error, ==, CELL_OK );
}

void mutex::unlock	() const
{
//	printf						("unlock 0x%x %x\r\n", *(u32*)&m_mutex, current_thread_id());
	int const error				= sys_lwmutex_unlock( (mutex_type*)&m_mutex );
	R_ASSERT_CMP				( error, ==, CELL_OK );
}

bool mutex::try_lock	() const
{
	return						sys_lwmutex_trylock( (mutex_type*)&m_mutex ) == CELL_OK;
}

#else // #if XRAY_USE_LIGHTWEIGHT_MUTEX

typedef sys_mutex_t				mutex_type;

mutex::mutex		()
{
	COMPILE_ASSERT				( sizeof( mutex_type ) == sizeof( m_mutex ), please_change_buffer_size );

	sys_mutex_attribute_t		attribute;
	attribute.attr_protocol		= SYS_SYNC_FIFO;
	attribute.attr_recursive	= SYS_SYNC_RECURSIVE;
	attribute.attr_pshared		= SYS_SYNC_NOT_PROCESS_SHARED;
	attribute.name[0]			= 0;

	int const error				= sys_mutex_create( (mutex_type*)&m_mutex, &attribute );
	R_ASSERT_CMP				( error, ==, CELL_OK );

	R_ASSERT					( try_lock() );
	R_ASSERT					( try_lock() );
#ifndef MASTER_GOLD
	unlock						( );
	unlock						( );
#endif // #ifndef MASTER_GOLD
}

mutex::~mutex		()
{
	int const error				= sys_mutex_destroy( *(mutex_type*)&m_mutex );

	if ( error != CELL_OK ) {
		if ( error == EINVAL )
			LOG_ERROR			( "mutex has been destroyed already");
		else if ( error == ESRCH )
			LOG_ERROR			( "user program destroyed mutex data");
		else if ( error == EBUSY )
			LOG_ERROR			( "mutex is busy");
		else
			UNREACHABLE_CODE	( );
	}

	R_ASSERT_CMP				( error, ==, CELL_OK );
}

void mutex::lock	()
{
//	printf						("lock   0x%x %x\r\n", *(u32*)&m_mutex, current_thread_id());
	int const error				= sys_mutex_lock( *(mutex_type*)&m_mutex, 0 );
	R_ASSERT_CMP				( error, ==, CELL_OK );
}

void mutex::unlock	()
{
//	printf						("unlock 0x%x %x\r\n", *(u32*)&m_mutex, current_thread_id());
	int const error				= sys_mutex_unlock( *(mutex_type*)&m_mutex );
	R_ASSERT_CMP				( error, ==, CELL_OK );
}

bool mutex::try_lock	()
{
	return						sys_mutex_trylock( *(mutex_type*)&m_mutex ) == CELL_OK;
}
#endif // #if XRAY_USE_LIGHTWEIGHT_MUTEX