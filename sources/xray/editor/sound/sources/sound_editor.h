////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_EDITOR_H_INCLUDED
#define SOUND_EDITOR_H_INCLUDED

#include "sound_editor_form.h"
#include <xray/sound/sound.h>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace WeifenLuo::WinFormsUI::Docking;
using namespace xray::sound;
using namespace xray::editor_base;
using namespace xray::editor::controls;
using xray::editor::wpf_controls::property_grid::value_changed_event_args;

namespace xray {
namespace editor {
	namespace controls {
		ref class scalable_scroll_bar;
	} // namespace controls
} // namespace editor
namespace sound {
	struct world;
} // namespace sound 
namespace render {
	class world;
	namespace editor {
		class renderer;
	} // namespace editor
} // namespace render 
namespace sound_editor {
	
	ref class sound_editor_form;
	ref class toolbar_panel;
	ref class instance_properties_panel;
	ref class object_properties_panel;
	ref class tree_view_panel;
	ref class sound_object_wrapper;
	ref class single_sound_wrapper;
	ref class composite_sound_wrapper;
	ref class sound_collection_wrapper;
	ref class sound_document_editor_base;
	public enum class sound_object_type
	{
		single = 256,
		composite = 257,
		collection = 258
	}; // enum class sound_object_type

	public ref class sound_editor: public tool_window
	{
		typedef Dictionary<String^, single_sound_wrapper^> single_sounds_list;
		typedef Dictionary<String^, composite_sound_wrapper^> composite_sounds_list;
		typedef Dictionary<String^, sound_collection_wrapper^> sound_collections_list;
		typedef List<String^> modified_list;
	public:
								sound_editor				(
																String^ res_path, 
																sound::world& sound_world, 
																render::world& render_world, 
																tool_window_holder^ holder
															);

		virtual					~sound_editor				();
		virtual void			clear_resources				();
		virtual bool			close_query					();
		virtual	void			tick						();
		virtual void			load_settings				(RegistryKey^) {};
		virtual void			save_settings				(RegistryKey^) {};
		virtual String^			name						() {return m_name;};
		virtual Form^			main_form					() {return m_form;};
		virtual	void			Show						(String^, String^);
				IDockContent^	find_dock_content			(String^ persist_string);
				bool			predicate_active_scene		();
				bool			predicate_save_scene		();
				bool			predicate_save_all			();
				sound::world*	get_sound_world				() {return m_sound_world;};
		sound::sound_scene_ptr&	get_sound_scene				() {return *m_sound_scene;};
				void			on_instance_property_changed(Object^ sender, value_changed_event_args^ e);
				void			on_object_property_changed	(Object^ sender, value_changed_event_args^ e);
				void			clear_properties			();
				void			show_properties				(Object^ obj, bool is_instance);
	sound_object_wrapper^		get_sound					(String^ sound_name, sound_object_type t);
	single_sound_wrapper^		get_single_sound			(String^ sound_name);
	composite_sound_wrapper^	get_composite_sound			(String^ sound_name);
	sound_collection_wrapper^	get_sound_collection		(String^ sound_name);
				void			remove_sound				(String^ sound_name, sound_object_type t);
				void			track_selected				(List<String^>^ paths);
				void			play_selected				();
				void			pause_selected				();
				void			stop_selected				();
				bool			check_modified				(ReadOnlyCollection<tree_node^>^ nodes);
				bool			is_active_collection		();
				void			apply_changes				(modified_list^ names);
				void			revert_changes				(modified_list^ names);
				void			add_items_to_collection		(modified_list^ names, tree_node^ dest_node, sound_object_type t);

		property render::world& render_world
		{
			render::world& get() {return m_render_world;};
		};
		property sound_editor_form^ form
		{
			sound_editor_form^ get() {return m_form;};
		}
		property sound_document_editor_base^ multidocument_base
		{
			sound_document_editor_base^ get() {return m_form->multidocument_base;};
		}
		property toolbar_panel^ toolbar
		{
			toolbar_panel^ get() {return m_toolbar_panel;};
		};
		property instance_properties_panel^ instance_properties
		{
			instance_properties_panel^ get() {return m_instance_properties_panel;};
		};
		property object_properties_panel^ object_properties
		{
			object_properties_panel^ get() {return m_object_properties_panel;};
		};
		property tree_view_panel^ files_view
		{
			tree_view_panel^ get() {return m_tree_view_panel;};
		};
		property sound::calculation_type calculation
		{
			sound::calculation_type get();
			void set(sound::calculation_type new_val);
		}

	private:
				void			initialize					();
				void			on_editor_closing			(Object^, FormClosingEventArgs^ e);
				void			close_internal				();
				void			options_loaded_callback		(sound_object_wrapper^ w);
				void			sound_loaded_callback		(sound_object_wrapper^ w);
				void			set_nodes_images			(tree_node^ node, images16x16 img, images16x16 fo_img, images16x16 fc_img);
				void			add_items_to_collection_impl(tree_node^ n, modified_list^ names_list);
				void			on_single_sound_spl_curve_changed(Object^, EventArgs^);
				void			on_form_activated			(Object^, EventArgs^);

	public:
		static String^			absolute_sound_resources_path;
		static String^			sound_resources_path;
	private:
		enum class editor_state
		{
			none,
			closing,
			closed
		};

	private: 
		String^						m_name;
		sound_editor_form^			m_form;
		sound::world*				m_sound_world;
		sound::sound_scene_ptr*		m_sound_scene;
		render::world&				m_render_world;
		tool_window_holder^			m_holder;
		editor_state				m_state;

		input_engine^				m_input_engine;
		gui_binder^					m_gui_binder;

		toolbar_panel^				m_toolbar_panel;
		instance_properties_panel^	m_instance_properties_panel;
		object_properties_panel^	m_object_properties_panel;
		tree_view_panel^			m_tree_view_panel;

		single_sounds_list^			m_single_sounds;
		composite_sounds_list^		m_composite_sounds;
		sound_collections_list^		m_sound_collections;
		modified_list^				m_modified_sounds;
	}; // class sound_editor

	public enum class sound_editor_drag_drop_operation: int
	{
		tree_view_file = 0,
		tree_view_folder = 1,
	};
}// namespace sound_editor
}// namespace xray
#endif // #ifndef SOUND_EDITOR_H_INCLUDED
