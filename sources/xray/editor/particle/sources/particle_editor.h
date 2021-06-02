////////////////////////////////////////////////////////////////////////////
//	Created		: 23.12.2009
//	Author		: Alexander Plichko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef PARTICLE_EDITOR_H_INCLUDED
#define PARTICLE_EDITOR_H_INCLUDED

#include <xray/render/facade/model.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace WeifenLuo::WinFormsUI::Docking;

using namespace xray::editor::controls;

namespace xray {
namespace ui {
	struct world;
	struct text;
}

namespace render { 
	class world; 
	namespace editor {
		class renderer;
	} // namespace editor
} // namespace render

	namespace particle_editor {

		ref class particle_groups_source;
		ref class particle_system_config_wrapper;
		ref class pe_transform_control_helper;

		typedef Generic::Dictionary<String^ , particle_system_config_wrapper^> configs_dictionary;
		typedef System::Collections::Generic::Dictionary<System::String^, System::String^>	data_source_types_dictioonary;

		ref class particle_graph_document;
		ref class particle_graph_node_style_mgr;
		ref class particle_sources_panel;
		ref class particle_time_based_layout_panel;
		ref class particle_data_sources_panel;
		ref class particle_graph_node;

		template<class t_type>
		interface class i_cloneable
		{
			t_type^ clone();
		};

		public ref class particle_editor : public xray::editor_base::tool_window
		{
		public:
										particle_editor		( System::String^ res_path, xray::render::world& render_world, xray::ui::world& ui_world, xray::editor_base::tool_window_holder^ h) ;
			virtual						~particle_editor	( );

					void							register_actions		( );
			virtual void							clear_resources			( );
			virtual bool							close_query				( ){return true;}
			virtual	void							tick					( );
			virtual void							load_settings			( RegistryKey^ product_key );
			virtual void							save_settings			( RegistryKey^ product_key );
					IDockContent^					find_dock_content		( System::String^ );
			virtual System::String^					name					( )					{ return m_name; };
 			virtual	void							Show					( System::String^ , System::String^ )	{ m_form->Show(); m_form->BringToFront(); }
			virtual System::Windows::Forms::Form^	main_form				( ) { return m_form; }

					void							on_render_resources_ready(resources::queries_result& data);
					void							query_create_render_resources();
			inline	render::scene_ptr const&		scene					() { R_ASSERT( m_scene ); return *m_scene;}
			inline	render::scene_view_ptr const&	scene_view				() { R_ASSERT( m_scene_view ); return *m_scene_view;}
			inline	render::render_output_window_ptr const&	render_output_window() { R_ASSERT( m_output_window ); return *m_output_window;}

		private:
			void									initialize				( );
			void									close_internal			( );
			void									setup_preview_model		( System::String^ library_item_path );
			void									on_preview_model_loaded	( resources::queries_result& data );			
			void									draw_statistics			( );
		private:
			System::String^								m_name;
			System::Windows::Forms::Form^				m_form;
			render::editor::renderer*					m_editor_renderer;
			xray::editor_base::tool_window_holder^		m_holder;
			bool										m_closed;
			
			xray::ui::world&							m_ui_world;
			xray::render::world&						m_render_world;
			render::static_model_ptr*					m_preview_model_instance;
			math::float4x4*								m_preview_model_root_matrix;
			System::String^								m_preview_model_path;
			configs::lua_config_value*					m_particles_library_config;
			render::scene_ptr*							m_scene;
			render::scene_view_ptr*						m_scene_view;
			render::render_output_window_ptr*			m_output_window;
			
			static String^								s_default_file_name = "new_particle";
			static String^								s_default_folder_name = "new_folder";
			static String^								s_file_extension	= ".particle";
			
			document_editor_base^						m_multidocument_base;
			particle_graph_node_style_mgr^				m_node_style_mgr;
			particle_sources_panel^						m_nodes_panel;
			particle_time_based_layout_panel^			m_time_layout_panel;
			particle_data_sources_panel^				m_data_sources_panel;
			data_source_types_dictioonary^				m_available_data_source_types;
			particle_groups_source^						m_particle_groups_source;

			pe_transform_control_helper^				m_transform_control_helper;
			xray::editor_base::scene_view_panel^		m_view_window;
			xray::editor_base::input_engine^			m_input_engine;
			xray::editor_base::gui_binder^				m_gui_binder;
			xray::editor_base::options_dialog^			m_options_dialog;


			xray::editor::controls::item_properties_panel_base^	m_properties_panel;
			xray::editor::controls::file_view_panel_base^		m_particles_panel;
			xray::editor::controls::toolbar_panel_base^			m_toolbar_panel;
			
			System::Windows::Forms::ToolStripMenuItem^  view_menu_item;
			System::Windows::Forms::ToolStripMenuItem^  toolbar_panel_menu_item;
			System::Windows::Forms::ToolStripMenuItem^  file_view_panel_menu_item;
			System::Windows::Forms::ToolStripMenuItem^  properties_panel_menu_item;
			System::Windows::Forms::ToolStripMenuItem^  data_sources_menu_item;
			System::Windows::Forms::ToolStripMenuItem^  preview_panel_menu_item;
			System::Windows::Forms::ToolStripMenuItem^  time_layout_menu_item;

			System::Windows::Forms::ToolStripMenuItem^  add_preview_model_menu_item;
			System::Windows::Forms::ToolStripMenuItem^  clear_preview_model_menu_item;

private:	System::Windows::Forms::StatusStrip^		statusStrip;
private:	System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel0;
private:	System::Windows::Forms::ToolStripMenuItem^  ToolsMenuItem;
private:	System::Windows::Forms::ToolStripMenuItem^  optionsToolStripMenuItem;
			




		public:
			static String^									particle_resources_path;
			static String^									absolute_particle_resources_path;

			configs_dictionary^								particle_configs;
			property xray::editor_base::editor_control_base^		active_control;
			
			property pe_transform_control_helper^		transform_helper{
				pe_transform_control_helper^		get() {return m_transform_control_helper;}
			}

			property xray::editor_base::scene_view_panel^		view_window{
				xray::editor_base::scene_view_panel^			get() {return m_view_window;}
			}
			property xray::editor::controls::item_properties_panel_base^	properties_panel{
				xray::editor::controls::item_properties_panel_base^		get(){return m_properties_panel;}
			}

			property xray::editor::controls::file_view_panel_base^		particles_panel{
				xray::editor::controls::file_view_panel_base^				get(){return m_particles_panel;}
			}

			property xray::editor::controls::toolbar_panel_base^			tollbar_panel{
				xray::editor::controls::toolbar_panel_base^				get(){return m_toolbar_panel;}
			}

			property document_editor_base^					multidocument_base{
				document_editor_base^						get(){return m_multidocument_base;}
			}
			property particle_graph_node_style_mgr^			node_style_mgr{
				particle_graph_node_style_mgr^				get(){return m_node_style_mgr;}
			}
			property particle_time_based_layout_panel^		time_based_layout_panel{
				particle_time_based_layout_panel^			get(){return m_time_layout_panel;}
			}
			property particle_sources_panel^				node_sources_panel{
				particle_sources_panel^						get(){return m_nodes_panel;}
			}
			property particle_data_sources_panel^			data_sources_panel{
				particle_data_sources_panel^				get(){return m_data_sources_panel;}
			}
			property render::editor::renderer*				editor_renderer{
				render::editor::renderer*					get(){return m_editor_renderer;}
			}
			property xray::editor_base::gui_binder^			gui_binder{
				xray::editor_base::gui_binder^				get(){return m_gui_binder;}
			}

			property data_source_types_dictioonary^			available_data_source_types
			{
				data_source_types_dictioonary^				get( )
				{
					return m_available_data_source_types;
				}				
			}

			property configs::lua_config_value				particles_library_config
			{
				configs::lua_config_value					get(){ return *m_particles_library_config; }
			}


		private:
			bool			dock_panels						();
			void			particle_editor_FormClosing		(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
			void			exitToolStripMenuItem_Click		( ) ;


			document_base^	on_create_document				();
			void 			on_document_add_new_lod			(Object^ sender, EventArgs^ e);
			void			on_document_delete_lod			();
			void			on_tree_view_expand_node		(Object^, TreeViewCancelEventArgs^ e);
			void			load_document					(xray::editor::controls::tree_node^ node);
			void			load_particles_library			( );
			void			on_particles_library_loaded		( xray::resources::queries_result& data );


		public:

			void			on_tree_view_paste_from_clipboard	( );
			void			on_tree_view_copy_to_clipboard		( );
			void			on_tree_view_cut_to_clipboard		( );
			void 			on_tree_view_remove_document		( );
			void			on_tree_view_file_double_click		(Object^ sender, TreeNodeMouseClickEventArgs^ e);
			void			on_tree_view_label_edit				(Object^ sender, NodeLabelEditEventArgs^ e);
			void 			on_tree_view_new_document			(Object^ sender, EventArgs^ e);
			void 			on_tree_view_new_folder				(Object^ sender, EventArgs^ e);
			void 			on_tree_view_folder_new_document	(Object^ sender, EventArgs^ e);
			void 			on_tree_view_rename_item			(Object^ , EventArgs^ );
			void 			on_tree_view_folder_new_folder		(Object^ sender, EventArgs^ e);
			void			on_create_new_file_from_tool_strip	( );
			void			on_restart_particle_system			(Object^ sender, EventArgs^ e);
			void			restart_particle_system				( );
			
			void			on_looping_particle_system			(Object^ sender, EventArgs^ e);
			void			on_toggle_post_process				(Object^ sender, EventArgs^ e);
			void			on_toggle_skybox					(Object^ sender, EventArgs^ e);
			
			void			tree_loaded							(Object^ sender, EventArgs^ e);
			void			show_properties						(Object^ obj);

			void			add_context_menu_to_tree_view_node(xray::editor::controls::tree_node^ node);
			particle_graph_node^ deserialize_particle_node_from_config( configs::lua_config_value& node_config );


			void			on_view_menu_item_select		(Object^ , EventArgs^ );
			void			on_toolbar_panel_menu_select	(Object^ , EventArgs^ );
			void			on_file_view_panel_menu_select	(Object^ , EventArgs^ );
			void			on_properties_panel_menu_select	(Object^ , EventArgs^ );
			void			on_data_sources_menu_select		(Object^ , EventArgs^ );
			void			on_time_layout_menu_select		(Object^ , EventArgs^ );
			void			on_preview_panel_menu_select	(Object^ , EventArgs^ );
			
			void			on_add_preview_model_menu_item_click	(Object^ , EventArgs^ );
			void			on_clear_preview_model_menu_item_click	(Object^ , EventArgs^ );
			void			on_scene_view_mode_click		(Object^ , EventArgs^ );
			void			on_render_mode_click			(Object^ , EventArgs^ );
			void			on_play_speed_click				(Object^ , EventArgs^ );
			void			on_playing_click				(Object^ , EventArgs^ );

			float			m_current_play_speed;
			bool			m_is_playing;
			bool			m_is_looping_particle_system;
			bool			m_is_use_post_process;
			bool			m_is_use_skybox;
			

			void			on_main_form_deactivate			( Object^ , EventArgs^ e);
			void			keys_combination_changed		( System::String^ combination );
			void			disable_selected_nodes			( );
			void			show_options					( Object^ , EventArgs^ );
			void			reset_transform					( );
		}; //class particle_editor
	} // namespace particle_editor  
} // namespace xray
#endif // #ifndef PARTICLE_EDITOR_H_INCLUDED
