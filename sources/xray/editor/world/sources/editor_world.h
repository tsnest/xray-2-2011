////////////////////////////////////////////////////////////////////////////
//	Created 	: 31.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_WORLD_H_INCLUDED
#define EDITOR_WORLD_H_INCLUDED

#include <xray/editor/world/world.h>
#include "project_defines.h"

#pragma managed( push, off )
#	include <xray/ui/engine.h>
#	include <xray/render/world.h>
#	include <xray/physics/engine.h>
#	include <xray/ai_navigation/api.h>
#	include <xray/ai_navigation/engine.h>
#pragma managed( pop )

#include <xray/sound/sound.h>

namespace xray {

namespace collision { struct space_partitioning_tree; class geometry_instance; }

namespace editor_base { 
	interface class editor_base; 
	ref class command_engine;
 	ref class action_shortcut;
 	ref class input_engine;
 	ref class gui_binder;
}

namespace ui{struct world;}
namespace physics{struct world;}
namespace sound{class sound_debug_stats;}

namespace editor {

ref class window_ide;
ref class editor_contexts_manager;
ref class console_wrapper;
ref class level_editor;
ref class tool_window_storage;

class editor_world :
	public xray::editor::world,
	private ui::engine,
	private physics::engine,
	private ai::navigation::engine,
	private boost::noncopyable
{
private:
	typedef WeifenLuo::WinFormsUI::Docking::IDockContent IDockContent;

public:
		editor_world			( xray::editor::engine& engine );
								~editor_world			( );

public:
	virtual	void				load					( );
	virtual	void				run						( );
	virtual	void				clear_resources			( );
	virtual	HWND				main_handle				( );
	virtual	HWND				view_handle				( );
	virtual HWND				camera_view_handle		( );

			void				on_fs_iterator_render_options_ready( fs_new::virtual_path_string const & options_path, xray::vfs::vfs_locked_iterator const & fs_it );
			
			void				on_active_control_changed( editor_base::editor_control_base^ c );
	virtual	void				tick					( );
			void				render					( u32 frame_id, xray::math::float4x4 view_matrix );
			void				set_default_layout		( );

	virtual int					exit_code				( ) const;
	virtual void				set_exit_code			( int const exit_code );
	virtual void				message_out				( pcstr message );
	inline	u32					frame_id				( ) const { return m_frame_id; };
			
			void				set_renderer_configuration	( pcstr in_render_config_name );
			
	inline	render::editor::renderer&		get_renderer( );
	inline	render::scene_ptr const&		scene		( ) { R_ASSERT( m_scene ); return *m_scene; }
	inline	render::scene_view_ptr const&	scene_view	( ) { R_ASSERT( m_scene_view ); return *m_scene_view; }
	inline	render::render_output_window_ptr const&	view_output_window( ) { R_ASSERT( m_view_output_window ); return *m_view_output_window; }

	inline	sound::sound_scene_ptr&			sound_scene	( )	{ R_ASSERT( m_sound_scene ); return *m_sound_scene; }

	IDockContent^				find_dock_content		( System::String^ type_name );
			float&				focus_distance			( ) { return m_object_focus_distance; }

public:
			void				on_idle_start			( );
			void				on_idle					( );
			void				on_idle_end				( );
			inline	xray::editor::engine&				engine			( );
	inline	gcroot<window_ide^>	ide						( ) const;

public:
			void				load_bkg_sound			( pcstr name );
			void				on_bkg_sound_loaded		( resources::queries_result& data );
			void				bkg_sound_play			( );
			void				bkg_sound_stop			( );

	xray::editor_base::scene_view_panel^ view_window		( ) {return m_view_window;}
	xray::editor_base::camera_view_panel^ camera_view_panel	( ) {return m_camera_view_window;}
	
			xray::ui::world*			get_ui_world		( ){ return m_ui_world; }
			level_editor^				get_level_editor	( );
			editor_base::gui_binder^	get_gui_binder		( );

public:
	xray::editor_base::tool_window^ create_model_editor		( );
			void				show_sound_editor			( );
			void				show_animation_editor		( );
			void				show_model_editor			( );
			void				show_dialog_editor			( );
			void				show_particle_editor		( );
			void				show_texture_editor			( );
			void				show_material_editor		( );
			void				show_material_instance_editor( );
			void				show_render_options_editor	( );
			
	editor_base::command_engine^	command_engine			( )			{ return m_command_engine; }
	gcroot<editor_base::input_engine^>	input_engine		( ) const	{ return m_input_engine; }
	
	System::String^				get_resources_path	( );

public:
			bool				undo_stack_empty			( );
			bool				redo_stack_empty			( );
			void				on_window_closing			( );

			int					rollback					( int steps );

			void				pause						( bool bpause);
			bool				paused						( ) const { return m_paused; };

	virtual	void				editor_mode					( bool beditor_mode );
			bool				editor_mode					( ) const { return m_editor_mode; }
			bool				close_query					( );
			void				toggle_console				( );

	collision::space_partitioning_tree*	get_collision_tree	( );

private:		
			bool				execute_input				( );
			void				register_actions			( );
			void 				set_active_control_impl		( editor_base::editor_control_base^ control );
			void				load_editors				( );
			void				load_settings				( );
			void				save_settings				( );
			void				init_sound_statistics		( );
public:
			void				set_soud_stats_mode			( u32 mode ) { m_sound_stats_mode = mode; }
private:
			void				query_scene					( );
			void			on_after_render_scene_created	( );

private:
			void				on_render_resources_created ( resources::queries_result& data );
			void				on_sound_scene_created		( resources::queries_result& data );


	gcroot<xray::editor_base::scene_view_panel^>	m_view_window;
	gcroot<xray::editor_base::camera_view_panel^>	m_camera_view_window;
	render::scene_ptr*					m_scene;
	render::scene_view_ptr*				m_scene_view;
	render::render_output_window_ptr*	m_view_output_window;
	render::render_output_window_ptr*	m_camera_view_output_window;
	editor::engine&						m_engine;
	sound::sound_scene_ptr*				m_sound_scene;

	gcroot<window_ide^>					m_window_ide;
	gcroot<level_editor^>				m_level_editor;
	float4x4							m_culling_view_matrix_inverted;

public:
	System::Windows::Forms::ImageList^	images16x16				( );
	gcroot<tool_window_storage^>		m_tool_windows;

	bool								m_b_draw_collision;
	u32									get_cells_flood_depth	( )	{ return m_cells_flood_depth; }
	void								set_cells_flood_depth	( u32 value );
	void								exit					( int exit_code );

	fs_new::virtual_path_string const&	get_selected_renderer_configuration() const { return m_selected_renderer_configuration; }
	void								updated_render_options_list();
	xray::physics::world&				get_physics_world		( ) { return *m_physics_world; }
	xray::ai::navigation::world&		get_ai_navigation_world	( ) { return *m_ai_navigation_world; }
	void								set_navmesh_info(pcstr) const { }
private:
	fs_new::virtual_path_string			m_selected_renderer_configuration;
	xray::ui::world*					m_ui_world;
	xray::physics::world*				m_physics_world;
	xray::ai::navigation::world*		m_ai_navigation_world;

	gcroot<xray::editor::controls::mode_strip_button^> m_render_options_button;
	
	gcroot<console_wrapper^>			m_console_wrapper;
	gcroot<editor_base::command_engine^> m_command_engine;
	gcroot<editor_base::input_engine^>	m_input_engine;			
	gcroot<editor_base::gui_binder^>	m_gui_binder;

	sound::sound_debug_stats*			m_sound_stats;
	u32									m_sound_stats_mode;

	float								m_object_focus_distance;

	u32									m_cells_flood_depth;
	u32									m_frame_id;
	bool								m_exit_query;
	bool								m_editor_mode;
	bool								m_paused;
}; // class editor_world

} // namespace editor
} // namespace xray

#include "editor_world_inline.h"

#endif // #ifndef EDITOR_WORLD_H_INCLUDED