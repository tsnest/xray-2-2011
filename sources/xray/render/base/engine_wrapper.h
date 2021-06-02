////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_BASE_ENGINE_WRAPPER_H_INCLUDED
#define XRAY_RENDER_BASE_ENGINE_WRAPPER_H_INCLUDED

#include <xray/limited_intrusive_list.h>
#include <boost/function.hpp>

namespace xray {

namespace collision {
	struct world;
} // namespace input
	
namespace render {
namespace engine {

struct XRAY_NOVTABLE command {
	inline			command					( ) : remove_frame_id( 0 ) {}
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( command )
	virtual	void	execute					( ) = 0;

	command*		next;
	XRAY_MAX_CACHE_LINE_PAD;
	u32				remove_frame_id;
}; // struct command

typedef boost::function< void () >			window_thread_callback_type;

struct XRAY_NOVTABLE wrapper {
	virtual	void	delete_on_logic_tick	( command* command ) = 0;
	virtual	void	delete_on_editor_tick	( command* command ) = 0;

	virtual	void	draw_frame_logic		( ) = 0;
	virtual	void	draw_frame_editor		( ) = 0;

	virtual	void	run_in_window_thread	( window_thread_callback_type const& callback ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( wrapper )
}; // struct wrapper

} // namespace engine
} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_BASE_ENGINE_WRAPPER_H_INCLUDED