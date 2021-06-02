////////////////////////////////////////////////////////////////////////////
//	Created		: 18.08.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_EDITOR_H_INCLUDED
#define ANIMATION_EDITOR_H_INCLUDED

using namespace WeifenLuo::WinFormsUI::Docking;
using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using namespace xray::editor_base;

#include <xray/rtp/base_controller.h>
#include <xray/render/engine/base_classes.h>

namespace xray {
namespace rtp { 
	class base_controller; 
} // namespace rtp
namespace animation { 
	namespace mixing { 
		class expression; 
	} // namespace mixing
} // namespace animation
namespace ui { 
	struct world; 
	struct window; 
} // namespace ui
namespace render { 
	class world; 
	namespace editor {
		class renderer;
	} // namespace editor
} // namespace render
namespace animation_editor {

	ref class animation_editor_form;
	ref class animation_editor_viewport_panel;
	ref class channels_editor;
	ref class viewer_editor;
	ref class animation_setup_manager;
	ref class animation_groups_editor;
	ref class animation_collections_editor;
	ref class view_window_control;
	ref class ae_transform_control_helper;
	ref class animation_node_clip;
	ref class animation_node_clip_instance;
	
	public delegate void navigation_controller_delegate		(xray::animation::mixing::expression const& expr, u32 current_time_in_ms);
	public delegate void animation_clip_request_callback	(animation_node_clip_instance^ clip);
	public delegate void request_delegate					(xray::resources::queries_result& data, animation_clip_request_callback^ callback);

	public value class intervals_request
	{
	public:
		float max_weight;
		float min_weight;
		float length;
		float offset;
		u32 interval_id;
		u32 cfg_interval_id;
	};

	struct navigation_controller_query
	{
		gcroot<navigation_controller_delegate^> m_delegate;

		navigation_controller_query(navigation_controller_delegate^ dd):m_delegate(dd) {};
		void callback(xray::animation::mixing::expression const& expr, u32 current_time_in_ms)
		{
			navigation_controller_delegate^ d = m_delegate;
			d(expr, current_time_in_ms);
		}
	};

	struct animation_clip_query
	{
		gcroot<animation_clip_request_callback^> m_callback;
		gcroot<request_delegate^> m_delegate;

		animation_clip_query(request_delegate^ dd, animation_clip_request_callback^ cb):m_delegate(dd), m_callback(cb) {};
		void callback(xray::resources::queries_result& data)
		{
			request_delegate^ d = m_delegate;
			d(data, m_callback);
			DELETE(this);
		}
	};

	public ref class animation_editor : public tool_window
	{
		typedef Dictionary<String^, animation_node_clip^> clips_collection_type;
		static									animation_editor		( ); 
	public:
												animation_editor		(	System::String^ resources_path, 
																			render::world& render_world,
																			ui::world& ui_wrld,
																			xray::editor_base::tool_window_holder^ h );
		virtual									~animation_editor		( );

	public:
		static const		u32					expression_alloca_buffer_size = 64 * xray::Kb;

	public:
		virtual void							clear_resources			( );
		virtual bool							close_query				( );
		virtual	void							tick					( );
		virtual void							load_settings			( RegistryKey^ /*product_key*/ ) {};
		virtual void							save_settings			( RegistryKey^ /*product_key*/ ) {};
		virtual String^							name					( ) {return m_name;};
		virtual	void							Show					( String^ context1, String^ context2 );
		virtual System::Windows::Forms::Form^	main_form				( );

				IDockContent^					find_dock_content		( String^ );
				render::editor::renderer&		get_renderer			( ) {return *m_renderer;};
				bool							is_in_navigation_mode	( ) {return m_in_navigation_mode;};
				bool							is_navigation_paused	( ) {return m_navigation_paused;};
				void							switch_navigation_mode	( );
				void							pause_navigation		( );
				void							move_controller			( );
				void							start_learning_controller( String^ controller_name );

		inline	render::scene_ptr const&				scene				( ) { return *m_scene; }
		inline	render::scene_view_ptr const&			scene_view			( ) { return *m_scene_view; }
		inline	render::render_output_window_ptr const&	render_output_window( ) { return *m_output_window; }


				void							start_saving_controller_scene	( );
				void							stop_saving_controller_scene	( );
				bool							is_saving_controller_scene		( );
				
				long							request_animation_clip			( String^ name, animation_clip_request_callback^ callback );
				void							refresh_animation_node_clip		( String^ name );
				animation_node_clip^			clip_by_name					( String^ clip_name );
 
	public:
		static property String^						models_path
		{
			String^		get(){ return m_model_path; };
		};
		property String^						resources_path
		{
			String^		get(){ return m_resources_path; };
		};
		static property String^					single_animations_path
		{
			String^ get( )
			{
				return "resources/" + m_single_animations_path;
			}
		}
		static property String^					animation_collections_path
		{
			String^ get( )
			{
				return "resources/" + m_animation_collections_path;
			}
		}
		static property String^					animation_clips_path
		{
			String^ get( )
			{
				return "resources/" + m_animation_clips_path;
			}
		}
		static property String^					animation_controllers_path
		{
			String^ get( )
			{
				return "resources/" + m_animation_controllers_path;
			}
		}
		static property String^					animation_viewer_scenes_path
		{
			String^ get( )
			{
				return "resources/" + m_animation_viewer_scenes_path;
			}
		}
		static property String^					single_animations_absolute_path
		{
			String^ get( )
			{
				return m_resources_path + "/sources/" + m_single_animations_path;
			}
		}
		static property String^					animation_collections_absolute_path
		{
			String^ get( )
			{
				return m_resources_path + "/sources/" + m_animation_collections_path;
			}
		}
		static property String^					animation_clips_absolute_path
		{
			String^ get( )
			{
				return m_resources_path + "/sources/" + m_animation_clips_path;
			}
		}
		static property String^					animation_controllers_absolute_path
		{
			String^ get( )
			{
				return m_resources_path + "/sources/" + m_animation_controllers_path;
			}
		}

		property editor_control_base^			active_control;
		property ae_transform_control_helper^	transform_helper
		{
			ae_transform_control_helper^ get() {return m_transform_control_helper;};
		}

		property animation_editor_form^			form
		{
			inline animation_editor_form^ get();
		};
		property scene_view_panel^				viewport
		{
			scene_view_panel^ get() {return m_view_window;};
		};
		property channels_editor^				channels
		{
			channels_editor^ get() {return m_channels_editor;};
		};
		property viewer_editor^					viewer
		{
			viewer_editor^ get() {return m_viewer_editor;};
		};
		property animation_setup_manager^		setup_manager
		{
			animation_setup_manager^ get() {return m_setup_manager;};
		};
		property animation_groups_editor^		groups
		{
			animation_groups_editor^ get() {return m_groups_editor;};
		};
		property animation_collections_editor^	collections_editor
		{
			animation_collections_editor^ get() {return m_collections_editor;};
		};

		property float2							control_view_dir
		{
			float2 get() {return *m_control_view_dir;};
			void set(float2 val) {*m_control_view_dir = val;};
		};
		property float2							control_path
		{
			float2 get() {return *m_control_path;};
			void set(float2 val) {*m_control_path = val;};
		};
		property bool							control_jump
		{
			bool get() {return m_control_jump;};
			void set(bool val) {m_control_jump = val;};
		};
		property bool							render_debug_info;
		property bool							learning_mode;
		property float							learning_max_residual;
		property u32							learning_max_iterations_count;

	private:
				void							action_focus_to_origin	();
				void							on_editor_closing		(Object^ sender, FormClosingEventArgs^ e);
				void							on_form_active_document_changed(System::Object^, System::EventArgs^);
				void							on_controller_loaded	(xray::resources::queries_result& result);
				void							tick_navigation_mode	();
				void							on_render_resources_ready(xray::resources::queries_result& data);
				void							query_create_render_resources();
				void							controller_set_target_callback(xray::animation::mixing::expression const& expr, u32 current_time_in_ms);
				void							inc_time_scale			();
				void							dec_time_scale			();
				void							reset_time_scale		();
				void							on_anim_clip_loaded		(xray::resources::queries_result& result, animation_clip_request_callback^ cb);
				void							view_on_drop_down_opening(System::Object^, System::EventArgs^);
				void							show_channels			(System::Object^, System::EventArgs^);
				void							show_viewer				(System::Object^, System::EventArgs^);
				void							show_setup				(System::Object^, System::EventArgs^);
				void							show_groups				(System::Object^, System::EventArgs^);
				void							show_collections		(System::Object^, System::EventArgs^);
				void							show_viewport			(System::Object^, System::EventArgs^);
				void							initialize				();
				void							close_internal			();

	private:
		static String^							m_single_animations_path;
		static String^							m_animation_collections_path;
		static String^							m_animation_clips_path;
		static String^							m_animation_controllers_path;
		static String^							m_animation_viewer_scenes_path;
		static String^							m_resources_path;
		static String^							m_model_path;

		String^									m_name;
		animation_editor_form^					m_form;
		scene_view_panel^						m_view_window;
		channels_editor^						m_channels_editor;
		viewer_editor^							m_viewer_editor;
		animation_setup_manager^				m_setup_manager;
		animation_groups_editor^				m_groups_editor;
		animation_collections_editor^			m_collections_editor;
		render::editor::renderer*				m_renderer;
		render::scene_ptr*						m_scene;
		render::scene_view_ptr*					m_scene_view;
		render::render_output_window_ptr*		m_output_window;
		render::world&							m_render_world;
		input_engine^							m_input_engine;
		gui_binder^								m_gui_binder;
		ae_transform_control_helper^			m_transform_control_helper;
		DockState								m_viewport_old_dock_state;
		xray::rtp::animation_controller_ptr*	m_controller;
		xray::timing::timer*					m_timer;
		float									m_time_scale;
		bool									m_navigation_model_added;
		bool									m_navigation_model_followed;
		bool									m_in_navigation_mode;
		bool									m_navigation_paused;
		float2*									m_control_path;
		float2*									m_control_view_dir;
		bool									m_control_jump;
		resources::user_data_variant*			m_pv;
		navigation_controller_query*			m_query_callback;
		SaveFileDialog^							m_save_file_dialog;
		OpenFileDialog^							m_open_file_dialog;
		xray::configs::lua_config_ptr*			m_controller_config;
		u32										m_previous_target_time;
		u32										m_expressions_counter;
		float									m_last_target_pos_y;
		clips_collection_type^					m_clips;
		System::String^							m_learning_controller_name;
		transform_control_rotation^				m_transform_control_rotation;
		transform_control_translation^			m_transform_control_translation;
		MenuStrip^								m_menu;
		ToolStripMenuItem^						m_menu_view;
		ToolStripMenuItem^						m_menu_view_channels;
		ToolStripMenuItem^						m_menu_view_viewer;
		ToolStripMenuItem^						m_menu_view_setup;
		ToolStripMenuItem^						m_menu_view_groups;
		ToolStripMenuItem^						m_menu_view_collections;
		ToolStripMenuItem^						m_menu_view_viewport;
		
		xray::editor_base::tool_window_holder^	m_holder;
		bool									m_closed;

// for test	
		void						show_statistics();
		ui::world&					m_ui_world;
		//strings::text_tree_view*	m_text_tree_view;
		ui::window*					m_text_wnd;
	}; // ref class animation_editor
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_EDITOR_H_INCLUDED
