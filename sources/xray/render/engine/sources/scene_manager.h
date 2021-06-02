////////////////////////////////////////////////////////////////////////////
//	Created		: 06.09.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_SCENE_MANAGER_H_INCLUDED
#define XRAY_RENDER_ENGINE_SCENE_MANAGER_H_INCLUDED

#include <xray/render/core/quasi_singleton.h>

namespace xray {
namespace render {

struct editor_renderer_configuration;

class scene;
class scene_view;
class render_output_window;

class scene_manager : public quasi_singleton<scene_manager>
{
public:
	~scene_manager();

	void		add_scene		( scene* in_scene);
	void		remove_scene	( scene* in_scene);

	void		add_scene_view		( scene_view* in_scene_view);
	void		remove_scene_view	( scene_view* in_scene_view);
	
	void		add_render_output_window		( render_output_window* in_output_window);
	void		remove_render_output_window	( render_output_window* in_output_window);

	
	scene*		create_scene	( xray::render::editor_renderer_configuration const& renderer_configuration);
	void		destroy			( scene* scene);

	scene_view*	create_scene_view();
	void		destroy			( scene_view* view);

	render_output_window*	create_render_output_window( HWND window);
	void		destroy			( render_output_window* output_window );
	inline	u32	scene_count		( ) const { return m_scenes.size(); }

private:
	typedef		vector<scene*>					scenes;
	typedef		vector<scene_view*>				scene_views;
	typedef		vector<render_output_window*>	render_output_windows;

	scenes					m_scenes;
	scene_views				m_views;
	render_output_windows	m_output_windows;
}; // class scene_manager

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_SCENE_MANAGER_H_INCLUDED