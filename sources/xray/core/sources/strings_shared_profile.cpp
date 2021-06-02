////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <boost/crc.hpp>
#include "strings_shared_manager.h"

using xray::strings::shared::profile;
using xray::threading::mutex;

u32 profile::create_checksum		( pcvoid const begin, pcvoid const end )
{
	boost::crc_32_type			processor;
	processor.process_block		( begin, end );
	return						( processor.checksum( ) );
}

profile* profile::create			( mutex& mutex, pcstr const value, profile const& temp )
{
	ASSERT						( value );
	ASSERT						( strings::length( value ) == temp.m_length, "shared string is corrupted" );
	ASSERT						( create_checksum( value, value + temp.m_length ) == temp.m_checksum, "shared string is corrupted" );

	u32							buffer_length = ( temp.m_length + 1 )*sizeof( char );
	R_ASSERT					( ( sizeof( profile ) + buffer_length ) < max_length, "alignment is corrupted, check compiler options" );

	mutex.lock					( );
	g_allocator.user_current_thread_id	( );
	profile* const result		= ( profile* ) XRAY_MALLOC_IMPL( g_allocator, sizeof( profile ) + buffer_length, "shared::string" );
	mutex.unlock				( );

	result->next_in_hashset		= NULL;
	result->m_reference_count	= 0;
	result->m_length			= temp.m_length;
	result->m_checksum			= temp.m_checksum;

	memory::copy				( const_cast<pstr>( result->value( ) ), buffer_length, value, buffer_length );

	return						( result );
}

void profile::create_temp			( pcstr const value, profile& result )
{
#ifdef DEBUG
	result.m_reference_count	= threading::atomic32_value_type(-1);
#endif // #ifdef DEBUG
	result.m_length				= strings::length( value );
	result.m_checksum			= create_checksum( value, value + result.m_length );
}

void profile::destroy				( mutex& mutex, profile* string )
{
	string->~profile			( );

	mutex.lock					( );
	g_allocator.user_current_thread_id	( );
	XRAY_FREE_IMPL				( g_allocator, string );
	mutex.unlock				( );
}