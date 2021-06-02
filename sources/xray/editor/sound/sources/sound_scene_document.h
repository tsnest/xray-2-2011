////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_SCENE_DOCUMENT_H_INCLUDED
#define SOUND_SCENE_DOCUMENT_H_INCLUDED

#include <xray/render/engine/base_classes.h>
#include <xray/sound/sound.h>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using namespace xray::editor_base;
using namespace WeifenLuo::WinFormsUI::Docking;

namespace xray {
namespace render { 
	class world; 
	namespace editor {
		class renderer;
	} // namespace editor
} // namespace render
namespace sound_editor {

	ref class sound_editor;
	ref class graph_view;
	ref class se_transform_control_helper;
	ref class sound_object_instance;

	public ref class sound_scene_document: public xray::editor::controls::document_base
	{
		typedef xray::editor::controls::document_base super;
		typedef List<sound_object_instance^> objects_list;
	public:
								sound_scene_document			(sound_editor^ ed);
								~sound_scene_document			();
		virtual	void			save							() override;
		virtual	void			load							() override;
		virtual	void			undo							() override;
		virtual	void			redo							() override;
		virtual	void			copy							(bool del) override;
		virtual	void			paste							() override;
		virtual	void			select_all						() override;
		virtual	void			del								() override;
				void			tick							();
				sound_editor^	get_editor						() {return m_editor;};
	render::editor::renderer*	get_renderer					() {return m_renderer;};
	sound::sound_scene_ptr&		get_sound_scene					();
				void			on_property_changed				(String^ prop_name, Object^ old_val, Object^ new_val);
		sound_object_instance^	get_object_by_id				(Guid obj_id);
				void			add_object						(sound_object_instance^ inst);
				void			remove_object					(Guid inst_id);
				bool			is_selected						(sound_object_instance^ inst);
				void			clear_selection					();
				void			select_object					(sound_object_instance^ inst);
				void			select_objects					(objects_list^ lst);
				void			change_object_transform			(sound_object_instance^ inst, float4x4 old_transform, float4x4 new_transform);
				void			play_selected					();
				void			pause_selected					();
				void			stop_selected					();
				void			play_all						();
				void			stop_all						();
				bool			all_playing						();
				bool			all_stoped						();

render::scene_ptr const&				scene					() {return *m_scene;};
render::scene_view_ptr const&			scene_view				() {return *m_scene_view;};
render::render_output_window_ptr const&	render_output_window	() {return *m_output_window;};

		property editor_control_base^ active_control;
		property scene_view_panel^ viewport
		{
			scene_view_panel^ get() {return m_view_window;};
		};

	protected:
		virtual void			on_form_closing					(Object^, System::Windows::Forms::FormClosingEventArgs^) override;

	private:
				void			initialize_components			();
				void			query_create_render_resources	();
				void			on_render_resources_ready		(xray::resources::queries_result& data);
				void 			query_sound_scene				();
				void 			on_sound_scene_created			(xray::resources::queries_result& data);
				void			register_actions				();
				void			unregister_actions				();
				void			on_resources_loaded				(xray::resources::queries_result& result);
				void			on_document_activated			(Object^, EventArgs^);
				void			view_window_drag_enter			(Object^, DragEventArgs^ e);
				void			view_window_drag_drop			(Object^, DragEventArgs^ e);
				void			add_objects						(u32 t, float3 position, List<String^>^ obj_list);
				void			active_object_matrix_modified	(bool apply);
				void			clear_helper					();

	private:
		System::ComponentModel::Container^			components;
		sound_editor^								m_editor;
		input_engine^								m_input_engine;
		gui_binder^									m_gui_binder;
		command_engine^								m_command_engine;
		scene_view_panel^							m_view_window;
		render::scene_ptr*							m_scene;
		render::scene_view_ptr*						m_scene_view;
		render::render_output_window_ptr*			m_output_window;
		render::editor::renderer*					m_renderer;
		render::world&								m_render_world;
		sound::sound_scene_ptr*						m_sound_scene;
		se_transform_control_helper^				m_transform_control_helper;
		transform_control_rotation^					m_transform_control_rotation;
		transform_control_translation^				m_transform_control_translation;
		bool										m_first_save;
		SaveFileDialog^								m_save_file_dialog;
		WeifenLuo::WinFormsUI::Docking::DockPanel^	m_main_dock_panel;
		objects_list^								m_objects;
		objects_list^								m_selected_objects;
	}; // ref class sound_scene_document
} // namespace sound_editor
} // namespace xray 
#endif // #ifndef SOUND_SCENE_DOCUMENT_H_INCLUDED