////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_GROUPS_DOCUMENT_H_INCLUDED
#define ANIMATION_GROUPS_DOCUMENT_H_INCLUDED

using namespace System::Collections::Generic;
using namespace xray::editor::wpf_controls::hypergraph;
using namespace xray::editor::wpf_controls::animation_setup;

namespace xray {
namespace animation_editor {

	ref class animation_groups_editor;
	ref class animation_group_object;
	ref class animation_groups_intervals_panel;
	ref class animation_node_clip_instance;
	ref class animation_node_interval;
	ref class animation_node_interval_instance;

	public ref class animation_groups_document: public xray::editor::controls::document_base
	{
		typedef xray::editor::controls::document_base super;
		typedef List<animation_group_object^> groups_type;
		typedef List<u32> query_ids_list;
	public:
								animation_groups_document		(animation_groups_editor^ ed);
								~animation_groups_document		();
		virtual	void			save							() override;
		virtual	void			load							() override;
		virtual	void			undo							() override;
		virtual	void			redo							() override;
		virtual	void			copy							(bool del) override;
		virtual	void			paste							() override;
		virtual	void			select_all						() override;
		virtual	void			del								() override;
				void			save_as							();
animation_groups_editor^		get_editor						() {return m_editor;};
hypergraph_control^				hypergraph						() {return m_hypergraph->hypergraph;};
groups_type^					groups							() {return m_groups;};
animation_group_object^			get_group						(System::Guid id);
				void			change_model					(System::String^ new_val);
				void			on_group_drag_over				(System::Object^, System::Windows::DragEventArgs^ e);
				void			on_group_drag_drop				(System::Object^ sender, System::Windows::DragEventArgs^ e);
				void			cancel_intervals_adding			();
				void			add_group_intervals				(node^ n, List<animation_node_interval^>^ lst);
				void			remove_group_intervals			(node^ n, System::Collections::IList^ lst);
				void			change_group_property			(System::String^ node_id, System::String^ property_name, System::Object^ new_val, System::Object^ old_val);
				void			change_interval_property		(System::String^ node_id, u32 index, System::String^ property_name, System::Object^ new_val, System::Object^ old_val);
				void			on_link_value_changed			(System::Object^ sender, System::Windows::RoutedPropertyChangedEventArgs<System::Object^>^ e);
				void			on_node_content_value_changed	(System::Object^ sender, System::Windows::RoutedPropertyChangedEventArgs<System::Object^>^ e);
				bool			can_learn_controller			();

		property System::String^ model
		{
			System::String^ get() {return m_model;};
			void set(System::String^ new_val);
		}
		property bool disable_events_handling;
		

	private:
				void			on_controller_loaded			(xray::resources::queries_result& result);
				void			on_groups_loaded				(xray::resources::queries_result& result);
				void			on_context_item_click			(System::Object^, menu_event_args^ e);
				void			on_link_creating				(System::Object^, link_event_args^ e);
				void			on_deleting_selection			(System::Object^, selection_event_args^ e);
				void			can_make_connection				(System::Object^, can_link_event_args^ e);
				void			on_node_moved					(System::Object^, node_move_event_args^ e);
				void			on_hypergraph_drag_over			(System::Object^, System::Windows::DragEventArgs^ e);
				void			on_hypergraph_drag_drop			(System::Object^, System::Windows::DragEventArgs^ e);
				void			on_animation_loaded				(animation_node_clip_instance^ new_clip);
				void			calculate_animations_count		(System::String^ folder_name, u32& nodes_counter);
				void			iterate_animation_folder		(System::String^ folder_name);
				void			on_document_activated			(System::Object^, System::EventArgs^);
				void			track_last_animation			();

	private:
		System::ComponentModel::Container^		components;
		animation_groups_editor^				m_editor;
		System::Windows::Forms::SaveFileDialog^	m_save_file_dialog;
		System::Windows::Forms::Panel^			m_model_panel;
		System::Windows::Forms::Label^			m_model_label;
		bool									m_first_save;
		hypergraph_host^						m_hypergraph;
		groups_type^							m_groups;
		xray::editor_base::command_engine^		m_command_engine;
		System::String^							m_model;
		animation_groups_intervals_panel^		m_intervals_panel;
		query_ids_list^							m_last_query_ids;
	};
} // namespace animation_editor
} // namespace xray 
#endif // #ifndef ANIMATION_GROUPS_DOCUMENT_H_INCLUDED