////////////////////////////////////////////////////////////////////////////
//	Created		: 18.01.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef LEVEL_EDITOR_H_INCLUDED
#define LEVEL_EDITOR_H_INCLUDED

#include "project_defines.h"

namespace xray {

namespace collision { struct space_partitioning_tree; }
namespace render {
	namespace editor { class renderer; }
	class world;
} // namespace render

namespace editor {

class editor_world;
ref class project;
ref class tools_tab;
ref class project_tab;
ref class property_connection_editor;
ref class object_inspector_tab;
ref class animation_curve_editor_panel;
ref class triangulation_tester;
ref class scene_graph_tab;
ref class le_helper;
ref class object_painter_tool_tab;
ref class window_ide;

public ref class level_editor : public xray::editor_base::editor_base
{
	typedef xray::editor_base::editor_base super;
	typedef WeifenLuo::WinFormsUI::Docking::IDockContent IDockContent;
public:
							level_editor					( editor_world& world );
	virtual					~level_editor					( );
	virtual void			initialize						( );
	virtual void			clear_resources					( );
	virtual void			tick							( );
	virtual bool			close_query						( );

			IDockContent^	find_dock_content				( System::String^ type_name );

			bool			load							( System::String^ name );

			void			load_settings					( );
			void			save_settings					( );

			void			set_default_layout				( );
	virtual System::String^	name							( ) {return m_name;};
	virtual	void			show							( ) {};
			void			on_camera_move					( );
			void			on_editor_renderer_initialized	( );

	xray::editor_base::command_engine^	get_command_engine	( ) ;
	xray::editor_base::input_engine^	get_input_engine	( );
	xray::editor_base::gui_binder^		get_gui_binder		( );
	xray::editor_base::scene_view_panel^ view_window		( );
	xray::editor_base::camera_view_panel^ camera_view_panel	( );

	le_helper^			m_transform_control_helper;

	void					add_library_object				( float3 position, tool_base^ tool, System::String^ library_name );
			tool_base^		get_tool						( System::String^ tool_name );
			project^		get_project						( );
			void			tool_loaded						( tool_base^ tool);
			void			new_project						( );
			bool			save_project					( System::String^ name );
			void			close_project					( );
			bool			save_needed						( );
			bool			project_not_empty				( );
			void			group_selection					( );
			void			ungroup_selection				( );

		object_base_list^	selected_objects				( );
		object_base^		object_by_id					( u32 id);

			void			select_all						( );	
			void			delete_selected					( );
			void			delete_selected_in_logic_hypergraph( );
			void			copy_selected_to_clipboard		( );
			void			cut_selected_to_clipboard		( );
			void			paste_from_clipboard			( );
			void			duplicate_selected				( );
			u32				selected_items_count			( );
			bool			selection_not_empty				( );
			float			focus_distance					( );

			collision::space_partitioning_tree*		get_collision_tree					( );
			project_tab^							get_project_explorer				( );
			tools_tab^								get_tools_explorer					( );
			property_connection_editor^				get_connection_editor				( );
			object_inspector_tab^					get_object_inspector_tab			( );
			animation_curve_editor_panel^			get_animation_curve_editor_panel	( );
			triangulation_tester^					get_triangulation_tester			( );
			xray::editor_base::scene_view_panel^	get_camera_view_panel				( );


			render::scene_ptr const&				scene								( );
			render::scene_view_ptr const&			scene_view							( );
			render::render_output_window_ptr const&	render_output_window				( );

			void			switch_scene_graph				( );
			void			switch_object_inspector			( );
			void			switch_connection_editor		( );			
			void			show_project_explorer			( );
			void			show_tools_frame				( );
			void			switch_animation_curve_editor	( );
			void			switch_triangulation_tester		( );
			void			switch_camera_view_panel		( );

			void			on_active_control_changed		( xray::editor_base::editor_control_base^ c );
			void			on_active_tool_changed			( tool_base^ tool );
			void			on_project_clear				( );

			void			get_recent_list					( System::Collections::ArrayList^ recent_list );
			void			change_recent_list				( System::String^ path, bool b_add );
			void			add_onframe_delegate			( execute_delegate^ d, u32 frames );
			void			remove_onframe_delegate			( execute_delegate^ user_delegate );
			
			// editor_world wrappers
			window_ide^			ide							( );

			void				focus_camera_to				( float3 const& position, float distance );
			void				set_transform_object		( xray::editor_base::transform_control_object^ d );

			System::Windows::Forms::DialogResult ShowMessageBox( System::String^ message, 
																System::Windows::Forms::MessageBoxButtons buttons, 
																System::Windows::Forms::MessageBoxIcon icon );

			void			on_tree_view_node_double_click	( System::Windows::Forms::TreeNode^ node );

inline	render::editor::renderer&	get_editor_renderer		();
			editor_world&		get_editor_world			( ) {return m_editor_world;}

	xray::editor_base::editor_control_base^	get_active_control	( );
	xray::editor_base::editor_control_base^ get_editor_control	( System::String^ control_id );
			void 			set_active_control				( System::String^ control_id );
			void			register_editor_control			( xray::editor_base::editor_control_base^ c );
			void			unregister_editor_control		( xray::editor_base::editor_control_base^ c );
			
			void			set_active_tool					( tool_base^ tool );
			tool_base^		get_active_tool					( ){ return m_active_tool; }
			bool			context_fit						( System::String^ context );
			void			get_contexts_list				( System::Collections::Generic::List<System::String^>^ list );
			void			update_object_inspector			( );
			void			reset_object_inspector			( );
public:
			void			open_project_action				( ); //must be available for project class as well
	inline	void			toggle_culling_view				( ) { m_is_culling_view = !m_is_culling_view; }
	inline	bool			get_culling_view				( ) { return m_is_culling_view; }
			bool			has_uncommited_changes			( System::String^% reason );

private:
			void			new_project_action				( );
			void			save_project_action				( );
			void			save_as_project_action			( );
			void			close_project_action			( );
			void			toggle_console_action			( );
			void			export_project_as_obj_action	( );
			void			export_selection_as_obj_action	( );
			void			flush_selection					( );
			bool			save_confirmation				( );
			bool			clipboard_not_empty				( );
			bool			object_inspector_visible		( );
			bool			project_explorer_visible		( );
			bool			tools_frame_visible				( );
			bool			connection_editor_visible		( );
			bool			animation_curve_editor_visible	( );
			bool			camera_view_panel_visible		( );
			bool			scene_graph_visible				( );
			bool			triangulation_tester_visible	( );
			
			void			toggle_debug_renderer_enabled	( )		{ m_debug_renderer_enabled =!m_debug_renderer_enabled; }
			bool			debug_renderer_enabled			( )		{ return m_debug_renderer_enabled;}

private:
			editor_world&								m_editor_world;
			System::String^								m_name;
			project^									m_project;
			System::Collections::ArrayList				m_invoke_on_frame_delegates;
			System::Collections::ArrayList				m_object_tools;
			tool_base^									m_active_tool;
			tools_tab^									m_tools_tab;
			project_tab^								m_project_tab;
			property_connection_editor^					m_connection_editor;
			scene_graph_tab^							m_scene_graph_tab;
			object_inspector_tab^						m_object_inspector_tab;
			animation_curve_editor_panel^				m_animation_curve_editor_panel;
			triangulation_tester^						m_triangulation_tester;
			
			xray::editor_base::editor_control_base^		m_active_control;
			System::Collections::ArrayList				m_editor_controls;

			xray::editor_base::transform_control_scaling^			m_transform_control_scaling;
			xray::editor_base::transform_control_rotation^			m_transform_control_rotation;
			xray::editor_base::transform_control_translation^		m_transform_control_translation;


			bool								m_exit_query;
//			object_painter_tool_tab^			m_object_painter_tool_tab;


private:
			void			view_panel_drag_drop			( System::Object^ sender, System::Windows::Forms::DragEventArgs^ e );
			void			view_panel_drag_enter			( System::Object^ sender, System::Windows::Forms::DragEventArgs^ e );

			void			on_selection_changed			( );
			void			register_actions				( );

			bool			m_tools_loaded;
			bool			m_editor_initialized;
			bool			m_debug_renderer_enabled;

			bool			m_is_culling_view;
}; // class level_editor

}// namespace editor
} // namespace xray

#endif // #ifndef LEVEL_EDITOR_H_INCLUDED
