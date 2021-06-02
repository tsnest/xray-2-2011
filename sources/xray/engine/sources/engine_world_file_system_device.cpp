////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Dmitriy Iassenev & Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "engine_world.h"
#include "apc.h"

namespace xray { 
namespace engine {

void engine_world::initialize_file_system_devices	( )
{
#if XRAY_PLATFORM_PS3
	NOT_IMPLEMENTED							( );
#else // #if XRAY_PLATFORM_PS3
	XRAY_CONSTRUCT_REFERENCE				(m_hdd_async_interface, fs_new::asynchronous_device_interface) 
												(& m_hdd_device, fs_new::watcher_enabled_true);
	XRAY_CONSTRUCT_REFERENCE				(m_dvd_async_interface, fs_new::asynchronous_device_interface) 
												(& m_dvd_device, fs_new::watcher_enabled_true);

	initialize_file_system_device			(* m_hdd_async_interface.c_ptr(), apc::hdd, "hdd");
	initialize_file_system_device			(* m_dvd_async_interface.c_ptr(), apc::dvd, "dvd");
#endif // #if XRAY_PLATFORM_PS3
}

class device_ticker {
public:
	inline device_ticker	( fs_new::asynchronous_device_interface& device ) :
		m_device		( &device )
	{
	}

	inline void operator ( )( ) const
	{
		m_device->tick	( true );
	}

private:
	fs_new::asynchronous_device_interface * m_device;
}; // struct device_ticker

template < typename TickFunctionType >
void engine_world::thread_function	( apc::threads_enum const apc_thread_id,  TickFunctionType const& functor )
{
	apc::assign_thread_id			( apc_thread_id, threading::current_thread_id( ) );
	apc::process					( apc_thread_id );

	for (;;) {
		if ( m_file_system_devices_destruction_started )
			break;

		functor						( );
	}
	
	apc::process					( apc_thread_id );
}

void engine_world::initialize_file_system_device	(
		fs_new::asynchronous_device_interface& device,
		apc::threads_enum const apc_thread_id,
		pcstr const debug_thread_id
	)
{
	if ( threading::g_debug_single_thread )
		return;

	apc::assign_thread_id			( apc_thread_id, u32(-1) );

	threading::spawn				(
		boost::bind( &engine_world::thread_function< device_ticker >, this, apc_thread_id, device_ticker( device ) ), 
		debug_thread_id, 
		debug_thread_id, 
		0,
		7 % threading::core_count()
	);
}

void engine_world::finalize_file_system_device	( fs_new::asynchronous_device_interface * const device )
{
	device->finalize					( );
}

void engine_world::finalize_file_system_devices	( )
{
	threading::interlocked_exchange		( m_file_system_devices_destruction_started, true );
	apc::run							( apc::hdd,	boost::bind(&engine_world::finalize_file_system_device, this, m_hdd_async_interface.c_ptr()), apc::break_process_loop, apc::wait_for_completion );
	apc::run							( apc::dvd,	boost::bind(&engine_world::finalize_file_system_device, this, m_dvd_async_interface.c_ptr()), apc::break_process_loop, apc::wait_for_completion );

	m_hdd_async_interface->finalize_thread_usage	( );
	m_dvd_async_interface->finalize_thread_usage	( );

	XRAY_DESTROY_REFERENCE				( m_hdd_async_interface );
	XRAY_DESTROY_REFERENCE				( m_dvd_async_interface );

	apc::wait							( apc::hdd );
	apc::wait							( apc::dvd );
}

void engine_world::file_system_devices_tick	( )
{
	m_hdd_async_interface->tick			( false );
	m_dvd_async_interface->tick			( false );
}

} // namespace engine
} // namespace xray