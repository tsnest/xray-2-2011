////////////////////////////////////////////////////////////////////////////
//	Created		: 06.09.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_ANIMATION_EDITOR_VIEWER_EDITOR_H_INCLUDED
#define XRAY_ANIMATION_EDITOR_VIEWER_EDITOR_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace WeifenLuo::WinFormsUI::Docking;
using namespace xray::editor::controls;
using namespace xray::editor::wpf_controls;
using xray::editor::wpf_controls::property_grid::value_changed_event_args;

namespace xray {
namespace animation_editor {

	ref class animation_editor;
	ref class animation_viewer_graph_nodes_style_mgr;
	ref class animation_viewer_toolbar_panel;
	ref class animation_viewer_properties_panel;
	ref class animation_editor_viewport_panel;
	ref class animation_viewer_time_panel;
	ref class animation_lexeme_editor;
	ref class animation_viewer_detailed_options_panel;
	ref class animation_viewer_graph_node_base;
	ref class animation_node_clip;
	ref class animation_node_interval;

	public ref class viewer_editor : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
								viewer_editor							(animation_editor^ ed);
								~viewer_editor							();
		void					tick									();
		void					clear_resources							();
		bool					close_query								();
		animation_editor^		get_animation_editor					() {return m_animation_editor;};
		document_base^			on_document_creating					();
		IDockContent^			find_dock_content						(System::String^ persist_string);
		void					change_recent_list						(System::String^ path, bool b_add);
		void					update_options_items					(System::Object^ , System::EventArgs^);
		void					on_scene_created						( render::scene_ptr const& scene );

		bool					predicate_save_scene					();
		bool					predicate_save_scene_as					();
		bool					predicate_has_target					();
		void					save_document_as						();
		void					add_models_to_render					();
		void					remove_models_from_render				();
		void					change_locked							();
		void					save_node_as_clip						();
		bool					anim_node_selected						();

		void					run_lexeme_editor						(animation_viewer_graph_node_base^ n);
		void					set_animation_node_new_intervals_list	(animation_viewer_graph_node_base^ n, List<animation_node_interval^>^ lst, u32 cycle_index);
		void					show_properties							(Object^ obj);

		void					on_node_property_changed				(System::Object^ sender, value_changed_event_args^ e);
		void					on_open_click							();
		void					on_editor_key_down						(Object^, KeyEventArgs^ e);
		inline render::scene_ptr const& scene							( ) { R_ASSERT( m_scene ); return *m_scene; }

		property u32	detalization_delta;
		property bool	show_detailed_weights;
		property bool	show_detailed_scales;
		property bool	play_from_beginning;
		property bool	snap_to_frames;
		property bool	follow_animation_thumb;
		property System::String^ source_path
		{
			System::String^ get( );
		}
		property animation_viewer_graph_nodes_style_mgr^ node_style_manager 
		{	
			animation_viewer_graph_nodes_style_mgr^ get() {return m_node_style_mgr;};
		}
		property xray::editor::controls::document_editor_base^ multidocument_base
		{
			xray::editor::controls::document_editor_base^ get() {return m_multidocument_base;}
		}
		property animation_viewer_toolbar_panel^ toolbar_panel
		{
			animation_viewer_toolbar_panel^ get() {return m_toolbar_panel;};
		};
		property animation_viewer_properties_panel^ properties_panel
		{
			animation_viewer_properties_panel^ get() {return m_properties_panel;};
		};
		property animation_viewer_time_panel^ time_panel
		{
			animation_viewer_time_panel^ get() {return m_time_panel;};
		};
		property bool camera_follows_npc
		{
			bool get() {return m_camera_follows_npc;};
			void set(bool val);
		};
		property bool use_detailed_weights_and_scales
		{
			bool get() {return m_use_detailed_weights_and_scales;};
			void set(bool val);
		};

	private:
		void					init_controls							();
		void					get_recent_list							(ArrayList^ recent_list);
		void					load_settings							();
		void					save_settings							();
		void					file_on_drop_down_opening				(System::Object^, System::EventArgs^);
		void					recent_on_drop_down_opening				(System::Object^, System::EventArgs^);
		void					recent_item_click						(System::Object^ sender, System::EventArgs^);
		void					on_view_menu_item_select				(System::Object^, System::EventArgs^);
		void					show_toolbar_panel						(System::Object^, System::EventArgs^);
		void					show_properties_panel					(System::Object^, System::EventArgs^);
		void					show_time_panel							(System::Object^, System::EventArgs^);
		void					on_document_closing						(System::Object^, System::EventArgs^);
		void					on_detailed_weights_and_scales_click	(System::Object^, System::EventArgs^);
		void					model_on_drop_down_opening				(System::Object^, System::EventArgs^);
		void					on_add_model_click						(System::Object^, System::EventArgs^);
		void					on_remove_model_click					(System::Object^, System::EventArgs^);
		void					on_select_model_click					(System::Object^, System::EventArgs^);
		void					on_new_document_created					(System::Object^, document_event_args^ e);
		bool					get_locked								();

	public:
		ToolStripMenuItem^							newToolStripMenuItem;
		ToolStripMenuItem^							saveToolStripMenuItem;
		ToolStripMenuItem^							saveasToolStripMenuItem;
		ToolStripMenuItem^							openToolStripMenuItem;
		ToolStripMenuItem^							undoToolStripMenuItem;
		ToolStripMenuItem^							redoToolStripMenuItem;
		ToolStripMenuItem^							cutToolStripMenuItem;
		ToolStripMenuItem^							copyToolStripMenuItem;
		ToolStripMenuItem^							pasteToolStripMenuItem;
		ToolStripMenuItem^							selectAllToolStripMenuItem;
		ToolStripMenuItem^							delToolStripMenuItem;
		ToolStripMenuItem^							lockToolStripMenuItem;
		ToolStripMenuItem^							saveClipToolStripMenuItem;

	private:
		MenuStrip^									m_menu_strip;
		ToolStripMenuItem^							fileToolStripMenuItem;
		ToolStripMenuItem^							recentToolStripMenuItem;
		ToolStripMenuItem^							exitToolStripMenuItem;
		ToolStripMenuItem^							editToolStripMenuItem;
		ToolStripMenuItem^							viewToolStripMenuItem;
		ToolStripMenuItem^							toolbarToolStripMenuItem;
		ToolStripMenuItem^							propertiesToolStripMenuItem;
		ToolStripMenuItem^							timeToolStripMenuItem;
		ToolStripMenuItem^							timepanelToolStripMenuItem;
		ToolStripMenuItem^							playfrombeginToolStripMenuItem;
		ToolStripMenuItem^							followthumbToolStripMenuItem;
		ToolStripMenuItem^							snaptoframesToolStripMenuItem;
		ToolStripMenuItem^							follownpcToolStripMenuItem;
		ToolStripMenuItem^							detailedweightsandscalesToolStripMenuItem;
		ToolStripMenuItem^							modelToolStripMenuItem;
		ToolStripMenuItem^							addmodelToolStripMenuItem;
		ToolStripMenuItem^							removemodelToolStripMenuItem;
		ToolStripMenuItem^							selectmodelToolStripMenuItem;
		OpenFileDialog^								m_open_file_dialog;

		System::ComponentModel::Container^			components;
		animation_editor^							m_animation_editor;
		document_editor_base^						m_multidocument_base;
		animation_viewer_graph_nodes_style_mgr^		m_node_style_mgr;
		animation_viewer_toolbar_panel^				m_toolbar_panel;
		animation_viewer_properties_panel^			m_properties_panel;
		animation_viewer_time_panel^				m_time_panel;
		animation_viewer_detailed_options_panel^	m_detailed_options_panel;
		animation_lexeme_editor^					m_lexeme_editor;
		render::scene_ptr*							m_scene;
		bool										m_camera_follows_npc;
		bool										m_use_detailed_weights_and_scales;
	}; // ref class viewer_editor

	public enum class viewer_drag_drop_operation: int 
	{
		root_node = 0,
		animation_node = 1,
		operator_node = 2,
	};
} // namespace animation_editor
} // namespace xray
#endif // #ifndef XRAY_ANIMATION_EDITOR_VIEWER_EDITOR_H_INCLUDED