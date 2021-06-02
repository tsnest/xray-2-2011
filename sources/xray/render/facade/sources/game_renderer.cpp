////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/facade/game_renderer.h>
#include <xray/render/world.h>
#include <xray/render/engine/world.h>
#include "functor_command.h"
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/facade/ui_renderer.h>
#include <xray/render/facade/scene_renderer.h>
#include "functor_with_big_buffer_to_copy_command.h"
#include <xray/render/facade/model.h>

namespace xray {
namespace render {
namespace game {

renderer::renderer	( world& world, engine::world& engine_world ) :
	m_world					( world ),
	m_render_engine_world	( engine_world )
{
	m_debug					= NEW ( debug::renderer )	( world.logic_channel(), *logic::g_allocator, engine_world );
	m_ui					= NEW ( ui::renderer )		( world.logic_channel(), *logic::g_allocator, engine_world );
	m_scene					= NEW ( scene_renderer )	( world.logic_channel(), *logic::g_allocator, engine_world, &m_debug->frustum );
}

renderer::~renderer		( )
{
	DELETE					( m_scene );
	DELETE					( m_ui );
	DELETE					( m_debug );
}

void renderer::end_frame		( )
{
	m_world.logic_channel().owner_push_back	( L_NEW( functor_command ) ( boost::bind( &world::end_frame_logic, &m_world) ) );
}

struct renderer::draw_scene_params {
	scene_ptr					scene;
	scene_view_ptr				scene_view;
	render_output_window_ptr	render_output_window;
	viewport_type				viewport;
};

void			render_on_draw_scene3		( bool use_depth );

void renderer::draw_scene_impl	(
		renderer::draw_scene_params const& params
	)
{
	R_ASSERT					( params.scene );
	R_ASSERT					( params.scene_view );
	R_ASSERT					( params.render_output_window );

 	m_render_engine_world.draw_scene(
 		params.scene,
 		params.scene_view,
 		params.render_output_window,
 		params.viewport,
 		boost::bind( &one_way_render_channel::render_on_draw_scene, (one_way_render_channel *) &m_world.logic_channel(), (render::scene_ptr const&)params.scene, (render::scene_view_ptr)params.scene_view, _1 )
 	);
}

void renderer::draw_scene		(
		scene_ptr const& scene,
		scene_view_ptr const& scene_view,
		render_output_window_ptr const& render_output_window,
		viewport_type const& viewport
	)
{
	draw_scene_params			params;
	params.scene				= scene;
	params.scene_view			= scene_view;
	params.render_output_window	= render_output_window;
	params.viewport				= viewport;

	m_world.logic_channel().owner_push_back	(
		L_NEW( functor_with_big_buffer_to_copy_command<draw_scene_params> ) (
			boost::bind(
				&renderer::draw_scene_impl,
				this,
				_1
			),
			params
		)
	);
}

xray::render::debug::renderer& renderer::debug	( ) const
{
	ASSERT					( m_debug );
	return					*m_debug;
}

ui::renderer& renderer::ui						( ) const
{
	ASSERT					( m_ui );
	return					*m_ui;
}

xray::render::scene_renderer& renderer::scene				( ) const
{
	ASSERT					( m_scene );
	return					*m_scene;
}

} // namespace game
} // namespace render
} // namespace xray