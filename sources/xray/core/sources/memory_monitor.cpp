////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "memory_monitor.h"
#include <xray/fs/synchronous_device_interface.h>

#if XRAY_USE_MEMORY_MONITOR

#include <xray/fs/synchronous_device_interface.h>	// for xray::core::current_directory
#include <xray/core_entry_point.h>
#include <xray/fs/path_string_utils.h>	// for xray::core::current_directory
#include <time.h>				// for _time64, _localtime64_s, strftime
#include <direct.h>				// for _mkdir

namespace xray {
namespace memory {
namespace monitor {

using threading::mutex;

// constants
static bool						s_initialized = false;
static u32 const				s_buffer_size = 512*1024;
static char						s_buffer[ s_buffer_size ];
static fs_new::file_type*		s_file = 0;
static pcstr					s_output_extension = ".bin";

static xray::uninitialized_reference<mutex>	s_mutex;

static command_line::key		s_memory_monitor_key("memory_monitor", "", "memory", "turns on monitoring all the memory oprations and dumping them to file");

union _allocation_size {
#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4201)
		struct {
			u32	allocation			: 1;
			u32 size				: 31;
		};
#	pragma warning(pop)
#endif // #if defined(_MSC_VER)
	u32		allocation_size;
};

static inline bool enabled		( )
{
#if XRAY_USE_MEMORY_MONITOR
	static bool const s_enabled	= s_memory_monitor_key.is_set( );
	return						s_enabled;
#else // #if XRAY_USE_MEMORY_MONITOR
	return						false;
#endif // #if XRAY_USE_MEMORY_MONITOR
}

void initialize	( )
{
	ASSERT						( !s_initialized );

	if ( !enabled() )
		return;

	string_path					output_folder;
	strings::copy				( output_folder, xray::fs_new::get_current_directory().c_str() );
	strcat_s					( output_folder, "memory_monitor\\");

	if ( _mkdir( output_folder ) != 0 )
		(void)0;

	__time64_t					long_time;
	_time64						(&long_time);

	tm							new_time;
	errno_t error				= _localtime64_s( &new_time, &long_time);
	R_ASSERT					( !error );

	string_path					file_name;
	strftime					( file_name, sizeof( file_name ), "%Y.%m.%d.%H.%M.%S", &new_time );

	string_path					file;
	strings::join				( file, output_folder, file_name, s_output_extension );

	ASSERT						( !s_file );
	
	using namespace				fs_new;
	synchronous_device_interface const &	device	=	core::get_core_synchronous_device( );

	if ( !device->open			( &s_file, file, file_mode::create_always, file_access::write ) )
		NODEFAULT				( );

	ASSERT						( s_file );

	device->setvbuf				( s_file, s_buffer, _IOFBF, s_buffer_size );

	XRAY_CONSTRUCT_REFERENCE	( s_mutex, mutex );

	s_initialized				= true;
}

void finalize		( )
{
	if ( !enabled() )
		return;

	ASSERT						( s_initialized );

	XRAY_DESTROY_REFERENCE		( s_mutex );

	fs_new::synchronous_device_interface const &	device	=	core::get_core_synchronous_device( );
	device->close				( s_file );
	s_file						= 0;

	s_initialized				= false;
}

void on_alloc	( pvoid& allocation_address, size_t& allocation_size, size_t const previous_size, pcstr const allocation_description )
{
	XRAY_UNREFERENCED_PARAMETER	( previous_size );

	if ( !enabled() )
		return;

	s_mutex->lock				( );

	_allocation_size			temp;
	temp.allocation				= 1;
	temp.size					= allocation_size;

	fs_new::synchronous_device_interface const &	device	=	core::get_core_synchronous_device( );

	device->write				( s_file, &temp, sizeof( temp ) );
	device->write				( s_file, &allocation_address, sizeof( allocation_address ) );
	device->write				( s_file, allocation_description, ( xray::strings::length( allocation_description ) + 1 )*sizeof( char ) );

	static bool flush = false;
	if (flush)
		device->flush			( s_file );

	s_mutex->unlock				( );
}

void on_free	( pvoid& deallocation_address, bool const can_clear )
{
	XRAY_UNREFERENCED_PARAMETER	( can_clear );

	if ( !enabled() )
		return;

	s_mutex->lock				( );

	if ( deallocation_address ) {
		_allocation_size		temp;
		temp.allocation_size	= 0;

		fs_new::synchronous_device_interface const &	device	=	core::get_core_synchronous_device( );
		device->write			( s_file, &temp, sizeof( temp ) );
		device->write			( s_file, &deallocation_address, sizeof( deallocation_address ) );
	}

	s_mutex->unlock				( );
}

} // namespace monitor
} // namespace memory
} // namespace xray

#endif // #if XRAY_USE_MEMORY_MONITOR