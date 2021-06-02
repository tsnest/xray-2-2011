////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MODEL_EDITOR_H_INCLUDED
#define MODEL_EDITOR_H_INCLUDED

#include <xray/render/engine/base_classes.h>

namespace xray {
namespace model_editor {

ref class ide;
ref class model_editor_object;
ref class me_transform_control_helper;
ref class model_editor : public xray::editor_base::tool_window
{

public:
											model_editor			(	render::world& render_world,
																		xray::editor_base::tool_window_holder^ holder,
																		pcstr resources_path);
	virtual									~model_editor			( );

	virtual void							clear_resources			( );
	virtual bool							close_query				( );
	virtual	void							tick					( );
	virtual	void							Show					( System::String^ context1, System::String^ context2 );
	virtual	System::String^					name					( ) { return "model_editor"; }
	virtual System::Windows::Forms::Form^	main_form				( );
			
	virtual void							load_settings			( RegistryKey^ product_key );
	virtual void							save_settings			( RegistryKey^ product_key );

			void							refresh_caption			( );
			void							action_focus_to_selection( );

	property collision::space_partitioning_tree* collision_tree
	{
		collision::space_partitioning_tree* get( );
	}
	
	property bool draw_collision_tree_;

	editor_base::scene_view_panel^			m_view_window;
	model_editor_object^					m_edit_object;
	me_transform_control_helper^			m_transform_control_helper;

	inline editor_base::editor_control_base^ get_active_control				( )											{ return m_active_control; }
	void									set_active_control				( editor_base::editor_control_base^ c );
	inline ide^								get_ide							( ) { return m_ide; }
	inline editor_base::input_engine^		get_input_engine				( ) { return m_input_engine; }
	inline editor_base::gui_binder^			get_gui_binder					( ) { return m_gui_binder; }



	inline render::scene_ptr const&			scene							( ) { R_ASSERT( m_scene ); return *m_scene; }
	inline render::scene_view_ptr const&	scene_view						( ) { R_ASSERT( m_scene ); return *m_scene_view; }
	inline render::render_output_window_ptr const&	render_output_window	( ) { R_ASSERT( m_scene ); return *m_output_window; }
	inline render::editor::renderer*		editor_renderer					( ) { R_ASSERT( m_editor_renderer ); return m_editor_renderer; }
	System::String^							get_resources_path				( ) { return m_resources_path; }

	void									execute_action					( System::String^ action_name );
private: // actions
	editor_base::editor_control_base^		m_active_control;
	editor_base::tool_window_holder^		m_holder;
	ide^									m_ide;

	void									action_touch					( );
	void									register_actions				( );
	void									action_open_solid_model			( );
	void									action_open_tree_model			( );
	void									action_new_composite_model		( );

	void									apply_button_Click				( System::Object^  sender, System::EventArgs^  e);
	void									cancel_button_Click				( System::Object^  sender, System::EventArgs^  e);
	void									ide_FormClosing					( System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
	void									close_internal					( );

	void									on_render_resources_ready		( resources::queries_result& data );
	void									query_create_render_resources	( );

	void									set_default_layout				( );
	IDockContent^							find_dock_content				( System::String^ );
	System::String^							registry_key_name				( );
private:
	render::world&							m_render_world;
	editor_base::input_engine^				m_input_engine;
	editor_base::gui_binder^				m_gui_binder;
	render::scene_ptr*						m_scene;
	render::scene_view_ptr*					m_scene_view;
	render::render_output_window_ptr*		m_output_window;
	render::editor::renderer*				m_editor_renderer;
	System::String^							m_resources_path;
}; // ref class model_editor


} // namespace model_editor
} // namespace xray

#endif // #ifndef MODEL_EDITOR_H_INCLUDED
