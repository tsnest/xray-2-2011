////////////////////////////////////////////////////////////////////////////
//	Created		: 19.03.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ENGINE_ENGINE_USER_H_INCLUDED
#define XRAY_ENGINE_ENGINE_USER_H_INCLUDED

#ifndef _MSC_VER
	typedef pvoid HWND;
#endif // #ifndef _MSC_VER

namespace xray {

namespace render {
	class world;

	namespace engine {
		struct command;
	} // namespace engine
} // namespace render

namespace network {
	struct engine;
	struct world;
} // namespace game

namespace sound {
	struct engine;
	struct world;
} // namespace game

namespace engine{
	class console;	
} //namespace math

namespace ui{	struct world;	} //namespace ui
namespace input{	struct world;	} //namespace input

namespace engine_user {

struct XRAY_NOVTABLE engine {
//	virtual	float	get_last_frame_time			( ) = 0;
	virtual	void	exit						( int exit_code ) = 0;
	virtual	void	enter_editor_mode			( ) = 0;
	virtual	bool	command_line_editor			( ) = 0;
	virtual	pcstr	get_resources_path			( ) const = 0;
	virtual	HWND	get_main_window_handle		( ) const = 0;
	virtual	HWND	get_render_window_handle	( ) const = 0;
	virtual	float2	get_render_window_size		( ) const = 0;
	virtual xray::engine::console* create_game_console(  ui::world& uw, input::world& iw ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( engine )
}; // class engine

struct XRAY_NOVTABLE world {
	virtual	void	enable						( bool value ) = 0;
	virtual	void	tick						( u32 current_frame_id ) = 0;
	virtual	void	clear_resources				( ) = 0;
	virtual	void 	load						( pcstr project_resource_name, pcstr project_resource_path=NULL ) = 0;
	virtual	void 	unload						( pcstr, bool ) = 0;
	virtual	void 	on_application_activate		( ) = 0;
	virtual	void 	on_application_deactivate	( ) = 0;
	virtual ui::world&		ui_world			( ) = 0;
	virtual input::world&	input_world			( ) = 0;
protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( world )
}; // class world

struct XRAY_NOVTABLE module_proxy {
	virtual	world*	create_world				(
						xray::engine_user::engine& engine, 
						xray::render::world& render_world, 
						xray::sound::world& sound,
						xray::network::world& network
					) = 0;
	virtual	void	destroy_world				( world*& world ) = 0;
	virtual	void	register_memory_allocators	( ) = 0;
	virtual	memory::doug_lea_allocator_type& allocator	( ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( module_proxy )
}; // class module_proxy

} // namespace engine_user
} // namespace xray

#endif // #ifndef XRAY_ENGINE_ENGINE_USER_H_INCLUDED