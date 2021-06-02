////////////////////////////////////////////////////////////////////////////
//	Created		: 23.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RTP_WORLD_H_INCLUDED
#define XRAY_RTP_WORLD_H_INCLUDED

namespace xray {

namespace render {
namespace debug {
	class renderer;
} // namespace debug
} // namespace render

namespace input {
	struct world;
} // namespace input

namespace rtp {

class base_controller;

struct XRAY_NOVTABLE world {
	virtual							~world								( ) { }
	virtual	void					tick								( ) = 0;
	virtual	bool					learn_step							( float stop_residual, u32 stop_iterations ) = 0;
	virtual	bool					is_loaded							( ) = 0;
	virtual	base_controller			*create_navigation_controller		( pcstr name, pcstr animation_set_path ) = 0;
	virtual	base_controller			*create_navigation_controller		( pcstr name, const xray::configs::binary_config_ptr &cfg ) = 0;
	virtual	void					destroy_controller					( base_controller* &ctrl ) = 0;
	virtual	void					clear_resources						( ) = 0;
	virtual	void					dbg_move_control					( float4x4 &view_inverted, input::world&  input ) = 0;
#ifndef MASTER_GOLD
	virtual	void					get_controllers_dump				( string4096 &s ) const = 0;
#endif
	virtual void					set_scene							( render::scene_ptr const& scene ) = 0;
	virtual	void					testing								( ) = 0;
	virtual	void					save_testing						( ) = 0;
}; // class world

} // namespace rtp
} // namespace xray

#endif // #ifndef XRAY_RTP_WORLD_H_INCLUDED