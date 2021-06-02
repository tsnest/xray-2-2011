////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RTP_WORLD_H_INCLUDED
#define RTP_WORLD_H_INCLUDED

#include <xray/rtp/world.h>
#include <xray/math_randoms_generator.h>
#include <xray/linkage_helper.h>
#include <xray/animation/api.h>

#include <xray/rtp/base_controller.h>

XRAY_INCLUDE_TO_LINKAGE(rtp_tester);

namespace xray {

namespace render {
namespace game {

	class renderer;

} // namespace game
} // namespace render

namespace animation {
	struct world;
} // namespace animation

namespace input {
	struct world;
} // namespace input

namespace rtp {

//class animation_grasping_controller;
class base_controller;
class resource_cooks;

class rtp_world :
	public xray::rtp::world,
	private boost::noncopyable
{
public:
							rtp_world				(
								rtp::engine& engine,
								animation::world& animation,
								render::scene_ptr const& scene,
								render::game::renderer* renderer,
								pcstr const resources_xray_path
							);
	virtual					~rtp_world				( );

public:
	virtual	base_controller	*create_navigation_controller		( pcstr name, pcstr animation_set_path );
	virtual	base_controller	*create_navigation_controller		( pcstr name, const xray::configs::binary_config_ptr &cfg );
	virtual	void			destroy_controller					( base_controller* &ctrl );

	virtual void			set_scene							( render::scene_ptr const& scene );

#ifndef MASTER_GOLD
	virtual	void			get_controllers_dump				( string4096 &s ) const;
#endif

private:
	virtual		void		render					( ) const;
	virtual		void		tick					( );
	virtual		void		dbg_move_control		( float4x4 &view_inverted, input::world&  input );

private:
				bool		dbg_input_commands		( input::world&  input );	

private:
				void		learn_single_step		( );

public:
	virtual		bool		learn_step				( float stop_residual, u32 stop_iterations );

	virtual		bool		learning				( );
	virtual		bool		is_loaded				( ) { return m_is_loaded; }
	
	virtual		void		testing					( );
	virtual		void		save_testing			( );
	
	virtual	void			clear_resources			( );
public:

	static	math::random32_with_count &random		( ) { return m_random; }

	animation::world*		animation_world			( )	{ return m_animation_world; }

private:
			u32				query_learned_controllers		( );
			u32				query_controller_sets			( );
			void			on_controllers_loaded			( resources::queries_result& data );

			void			create_test_controllers			( );

			void			load						( );
			
			void			clear_controllers		( );

			void			save_controllers		( const vector< animation_controller_ptr > &controllers, bool training );
			void			reset_controllers		( );
			

private:
	vector< animation_controller_ptr >		m_controllers;

	animation::world*				m_animation_world;
	xray::render::game::renderer*	m_renderer;

	
	timing::timer				m_timer_continious;


	configs::binary_config_ptr m_load_temp_settings;

private:
	timing::timer				m_timer;
	u32							m_previous_ms;

private:
	pcstr						m_resource_path;
	bool						m_is_loaded;

private:
	resource_cooks*			m_resources;
	render::scene_ptr			m_scene;

private:
	static math::random32_with_count	m_random;

}; // class rtp_world




void				learn_step_controllers(  vector< animation_controller_ptr > &controllers, float stop_residual );
void				learn_init_controllers(  vector< animation_controller_ptr > &controllers );
void				learn_stop_controllers(  vector< animation_controller_ptr > &controllers );

} // namespace rtp


} // namespace xray

#endif // #ifndef RTP_WORLD_H_INCLUDED