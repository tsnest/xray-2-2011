////////////////////////////////////////////////////////////////////////////
//	Created		: 06.09.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "scene_manager.h"
#include "scene.h"
#include "scene_view.h"
#include "render_output_window.h"
#include <xray/render/core/render_target.h>
#include "material.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/untyped_buffer.h>

namespace xray {
namespace render {

scene_manager::~scene_manager()
{
	scenes::iterator	it_c = m_scenes.begin(),
						en_c = m_scenes.end();

//!	ASSERT( it_c == en_c, "Some scenes were not deleted before render engine destruction." );
	if ( it_c != en_c )
		LOG_ERROR		( "Some scenes were not deleted before render engine destruction." );

	for( ; it_c != en_c; ++it_c)
		DELETE			(*it_c);

	scene_views::iterator	it_v = m_views.begin(),
							en_v = m_views.end();

	ASSERT( it_v == en_v, "Some scene views were not deleted before render engine destruction." );

	for( ; it_v != en_v; ++it_v)
		DELETE(*it_v);
}

void scene_manager::add_scene		( scene* in_scene)
{
	m_scenes.push_back( in_scene);
}

void scene_manager::remove_scene	( scene* in_scene)
{
	scenes::iterator found = std::find(m_scenes.begin(), m_scenes.end(), in_scene);
	
	if (found == m_scenes.end())
		return;
	
	m_scenes.erase(found);
}

void scene_manager::add_scene_view		( scene_view* in_scene_view)
{
	m_views.push_back(in_scene_view);
}

void scene_manager::remove_scene_view	( scene_view* in_scene_view)
{
	scene_views::iterator found = std::find(m_views.begin(), m_views.end(), in_scene_view);
	
	if(found == m_views.end())
		return;
	
	m_views.erase(found);
}

void scene_manager::add_render_output_window		( render_output_window* in_output_window)
{
	m_output_windows.push_back(in_output_window);
}

void scene_manager::remove_render_output_window	( render_output_window* in_output_window)
{
	render_output_windows::iterator found = std::find(m_output_windows.begin(), m_output_windows.end(), in_output_window);
	
	if(found == m_output_windows.end())
		return;
	
	m_output_windows.erase(found);
}

scene*	scene_manager::create_scene( xray::render::editor_renderer_configuration const& renderer_configuration)
{
	scene* new_scene	= NEW(scene) (renderer_configuration);
	m_scenes.push_back(new_scene);

	return new_scene;
}

void	scene_manager::destroy( scene* _scene)
{
	scenes::iterator it = std::find( m_scenes.begin(), m_scenes.end(), _scene);

	ASSERT( it != m_scenes.end());
	if( it == m_scenes.end())
		return;

	DELETE			(*it);
	m_scenes.erase	(it);
}

scene_view*	scene_manager::create_scene_view()
{
	scene_view* new_view	= NEW(scene_view);
	m_views.push_back(new_view);

	return new_view;
}

void	scene_manager::destroy( scene_view* view)
{
	scene_views::iterator it = std::find( m_views.begin(), m_views.end(), view);

	ASSERT( it != m_views.end());
	if( it == m_views.end())
		return;

	DELETE			(*it);
	m_views.erase	(it);
}

struct render_output_window_predicate 
{
	render_output_window_predicate ( HWND window) : m_window(window){}

	bool operator ()( render_output_window * other)
	{
		return m_window == other->window();
	}

	HWND		m_window;
};

render_output_window*	scene_manager::create_render_output_window( HWND window)
{
 	render_output_windows::iterator it = std::find_if( m_output_windows.begin(), m_output_windows.end(), render_output_window_predicate( window) );
 	if( it != m_output_windows.end())
	{
		ASSERT( 0, "A window output with the specified handle has been crate already.");
 		return *it;
	}

	render_output_window* new_output	= NEW(render_output_window)( window);
	m_output_windows.push_back(new_output);

	return new_output;
}

void scene_manager::destroy( render_output_window* output_window)
{
	render_output_windows::iterator const found	= std::find( m_output_windows.begin(), m_output_windows.end(), output_window );
	R_ASSERT									( found != m_output_windows.end() );
	DELETE										( *found );
	m_output_windows.erase						( found );
	R_ASSERT									( std::find( m_output_windows.begin(), m_output_windows.end(), output_window ) == m_output_windows.end() );
}

} // namespace render
} // namespace xray
