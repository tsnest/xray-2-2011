////////////////////////////////////////////////////////////////////////////
//	Created 	: 21.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ENGINE_WORLD_H_INCLUDED
#define ENGINE_WORLD_H_INCLUDED

#include <xray/core/engine.h>
#include <xray/editor/world/engine.h>
#include <xray/network/engine.h>
#include <xray/sound/engine.h>
#include <xray/engine/engine_user.h>
#include <xray/intrusive_spsc_queue.h>
#include <xray/one_way_threads_channel.h>
#include <xray/fs/asynchronous_device_interface.h>
#include <xray/fs/windows_hdd_file_system.h>
#include "apc.h"

#if !defined(_MSC_VER)
	typedef pvoid HWND;
#endif // #if !defined(_MSC_VER)

namespace xray {

namespace editor {
	struct world;
} // namespace editor

namespace render {
namespace engine {
	class renderer;
} // namespace engine
} // namespace render

namespace engine {

typedef boost::function< void () >					logic_thread_callback_type;

class engine_world : 
	public core::engine,
	public engine_user::engine,
	public editor::engine,
	public network::engine,
	public sound::engine,
	private boost::noncopyable
{
public:
							engine_world			( xray::engine_user::module_proxy& proxy, pcstr command_line, pcstr application, pcstr build_date );
	virtual					~engine_world			( );
			void			initialize				( );
			void			finalize				( );
			void			run						( );

	virtual void			set_exit_code			( int exit_code );
	virtual int				get_exit_code			( ) const;

	virtual	pcstr			get_mounts_path			( ) const;
	virtual	void			create_physical_path	( string_path& result, pcstr resources_path, pcstr inside_resources_path ) const { strings::join( result, resources_path,inside_resources_path); }
	virtual	HWND			get_main_window_handle	( ) const { return m_main_window_handle; }
	virtual	HWND			get_render_window_handle( ) const { return m_render_window_handle; }
	virtual	float2			get_render_window_size	( ) const;

			bool			command_line_editor		( );
			bool	command_line_no_splash_screen	( );

public:
	render::world&			render_world			( )	const	{ return *m_render_world;	}
	xray::engine_user::world& engine_user_world		( ) const	{ return *m_engine_user_world; }
	editor::world*			editor_world			( )	const	{ return m_editor;			}
	virtual void			on_application_activate	( );
	virtual void			on_application_deactivate ( );

	virtual bool			is_closing_timeout_set	( ) const;
	inline	bool			is_destruction_started	( ) const { return m_destruction_started || m_early_destruction_started; }

private:
	virtual	void			tick					( );

	// editor
	virtual	pcstr			get_resources_path		( ) const;
	virtual	void			enable_game				( bool value );
	virtual	void 			load_level				( pcstr project_resource_name, pcstr project_resource_path=NULL );
	virtual	void 			unload_level			( );
	virtual bool			is_application_active	( );
	virtual	void			run_renderer_commands	( );
	virtual	void			on_resize				( );
	virtual	render::world&	get_renderer_world		( );
	virtual	sound::world&	get_sound_world			( );
	virtual	bool			on_before_editor_tick	( );
			void		on_after_editor_tick_impl	( );
	virtual	void			on_after_editor_tick	( );
	virtual	void			enter_editor_mode		( );
	virtual xray::engine::console* create_editor_console( ui::world& uw );

	virtual	void			exit					( int exit_code );
	virtual	bool			command_line_editor_singlethread	( );

	virtual xray::engine::console* create_game_console	( ui::world& uw, input::world& iw );

			void			editor_message_out		( pcstr );
private:
			void			initialize_core			( );
			void			initialize_editor		( );
			void	initialize_editor_thread_ids	( );
			void			initialize_render		( );
			void			initialize_logic		( );
			void			initialize_logic_thread	( );
			void			post_initialize			( bool const show_window );
			void			enable_game_impl		( bool const value );

private:
			void			initialize_network		( );
			void			initialize_network_modules ( );
			void			initialize_sound		( );
			void			initialize_sound_modules( );
			void			initialize_build		( pcstr const project_id );
			void	initialize_terminate_on_timeout	( );
			void			initialize_logic_modules( );

private:
			void		on_render_resources_created ( resources::queries_result& data );
			void			create_render			( );
			void			destroy_render			( );
			void			logic_dispatch_callbacks( );
			void			logic_tick				( );
			void			logic					( );
			void			logic_finalize_modules	( );
			void			editor					( );
			void			network_tick			( );
			void			network					( );
			void			sound_tick				( );
			void			sound					( );
			void			build_tick				( );
			void			build					( pcstr project_id );
			void			terminate_on_timeout	( float time_limit );
			void			try_load_editor			( );
			void			unload_editor			( );
			void			show_window				( bool call_application_activate );
			bool			process_messages		( );
			void			logic_clear_resources	( );
			void			network_clear_resources	( );
			void			sound_clear_resources	( );
			void			build_clear_resources	( );
#ifndef MASTER_GOLD
			void			editor_clear_resources	( );
			void		 editor_dispatch_callbacks	( );
#endif // #ifndef MASTER_GOLD

			void			initialize_resources			( );
			void			finalize_resources				( );
			void			resources_thread				( apc::threads_enum const apc_thread_id );
			void			cooker_thread					( apc::threads_enum const apc_thread_id );
#ifndef MASTER_GOLD
			void			watcher_thread					( apc::threads_enum const apc_thread_id );
#endif // #ifndef MASTER_GOLD

 			void			initialize_file_system_devices	( );
			void			finalize_file_system_device		( fs_new::asynchronous_device_interface * const device );
			void			finalize_file_system_devices	( );
 			void			file_system_devices_tick		( );
			void			initialize_file_system_device	( fs_new::asynchronous_device_interface& device,
															  apc::threads_enum const apc_thread_id,
															  pcstr const debug_thread_id );
 			template < typename TickFunctionType >
 			void			thread_function			( apc::threads_enum const apc_thread_id,  TickFunctionType const& functor );

			logging::log_file_usage		log_file_usage ( ) const;

private:
	typedef memory::doug_lea_allocator_type			doug_lea_allocator_type;

private:
	doug_lea_allocator_type			m_network_allocator;
	doug_lea_allocator_type			m_sound_allocator;
	doug_lea_allocator_type			m_render_allocator;
	doug_lea_allocator_type			m_editor_allocator;

	render::world*					m_render_world;
	editor::world*					m_editor;

	threading::atomic_ptr<network::world>::type	m_network_world;
	threading::atomic_ptr<sound::world>::type	m_sound_world;

//	engine_user
	xray::engine_user::module_proxy& m_engine_user_module_proxy;
	xray::engine_user::world*		 m_engine_user_world;

#if !XRAY_PLATFORM_PS3
	fs_new::windows_hdd_file_system			m_hdd_device;
	fs_new::windows_hdd_file_system			m_dvd_device;
#endif // !if XRAY_PLATFORM_PS3

	uninitialized_reference<fs_new::asynchronous_device_interface>	m_hdd_async_interface;
	uninitialized_reference<fs_new::asynchronous_device_interface>	m_dvd_async_interface;

	HWND							m_main_window_handle;
	HWND							m_render_window_handle;
	u32								m_logic_frame_id;
	u32								m_editor_frame_id;
	int								m_exit_code;
	threading::atomic32_type		m_destruction_started;
	threading::atomic32_type		m_file_system_devices_destruction_started;
	bool							m_early_destruction_started;
	bool							m_initialized;
	bool							m_game_enabled;
	bool							m_last_game_enabled_value;
	bool							m_game_paused_last;
	bool							m_application_active;
	bool							m_application_activated;

	resources::mount_ptr			m_resources_mount;
	resources::mount_ptr			m_user_data_mount;
	resources::mount_ptr			m_test_resources_mount;

	threading::atomic32_type		m_resources_destruction_started;
	threading::atomic32_type		m_resources_cooker_destruction_started;

}; // class engine_world

} // namespace engine

bool	command_line_editor					( );
bool	command_line_editor_singlethread	( );

} // namespace xray

#endif // #ifndef ENGINE_WORLD_H_INCLUDED