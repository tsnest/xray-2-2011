////////////////////////////////////////////////////////////////////////////
//	Created		: 06.09.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_ANIMATION_EDITOR_VIEWER_DOCUMENT_H_INCLUDED
#define XRAY_ANIMATION_EDITOR_VIEWER_DOCUMENT_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using namespace xray::editor::wpf_controls;

#include <xray/animation/i_editor_mixer.h>
#include <xray/animation/mixing_animation_lexeme.h>

namespace xray {
namespace timing { 
	class timer;
} // namespace timing
namespace animation_editor {

	using xray::animation::editor_animation_state;
	using xray::animation::mixing::expression;
	using xray::animation::mixing::animation_lexeme;
	using xray::animation::mixing::animation_lexeme_ptr;

	ref class viewer_editor;
	ref class animation_viewer_hypergraph;
	ref class animation_viewer_graph_link;
	ref class animation_viewer_graph_link_weighted;
	ref class animation_viewer_graph_node_base;
	ref class animation_viewer_graph_node_animation;
	ref class animation_node_interval;
	ref class animation_model;

	public ref class viewer_document : public xray::editor::controls::document_base
	{
		typedef xray::editor::controls::document_base super;
		typedef List<xray::editor::controls::hypergraph::node^ > nodes;
		typedef List<xray::editor::controls::hypergraph::link^ > links;
		typedef xray::editor::wpf_controls::property_container wpf_property_container;
		typedef map<u32, xray::animation::mixing::animation_lexeme_ptr> lexemes_collection_type;
		typedef List<animation_playback::animation_item^> animation_items_list;
		typedef Dictionary<String^, animation_model^> models_list;
		typedef Dictionary<String^, List<u32>^> ids_by_anim_name_type;

	public:
								viewer_document					(viewer_editor^ ed);
								~viewer_document				();

		virtual	void			save							() override;
		virtual	void			load							() override;
		virtual	void			undo							() override;
		virtual	void			redo							() override;
		virtual	void			copy							(bool del) override;
		virtual	void			paste							() override;
		virtual	void			select_all						() override;
		virtual	void			del								() override;
				void			save_as							();
				void			on_property_changed				(wpf_property_container^ pc, System::String^ l, Object^ new_val, Object^ old_val);

				void			set_target						(animation_viewer_graph_node_base^ n);
				void			reset_target					();
				bool			has_target						() { return m_target != nullptr; };
				void			tick							();
				void			home							();
				void			move_backward					();
				void			play							();
				void			stop							();
				void			move_forward					();
				void			end								();
				void			add_models_to_render			();
				void			remove_models_from_render		();
				void			set_camera_follower				(bool val);
				void			add_model						(System::String^ model_path);
				void			add_model						(System::String^ model_path, System::String^ model_name);
				void			remove_model					();
				void			select_active_model				();
				bool			has_loaded_models				();
				void			save_node_as_clip				();
				bool			anim_node_selected				();
				void			set_saved_flag					();

				void			add_connection					(xray::editor::controls::hypergraph::node^ source, xray::editor::controls::hypergraph::node^ destination);
animation_viewer_graph_link^	create_connection				(xray::editor::controls::hypergraph::node^ source, xray::editor::controls::hypergraph::node^ destination);
		viewer_editor^			get_editor						() {return m_editor;};
				bool			is_loaded						() {return m_is_loaded;};
				void			set_animation_node_new_intervals_list(animation_viewer_graph_node_base^ n, List<animation_node_interval^>^ lst, u32 cycle_index);
				void			update_panel_values				();

		property float	max_time
		{
			float		get							() {return m_max_time;};
			void		set							(float new_val);
		};
		property float	current_time
		{
			float		get							() {return m_current_time;};
			void		set							(float new_val);
		};
		property float	time_scale
		{
			float		get							() {return m_time_scale;};
			void		set							(float new_val);
		};
		property bool	paused
		{
			bool		get							() {return m_paused;};
			void		set							(bool new_val);
		};
		property bool	models_changed
		{
			bool		get							() {return m_models_changed;};
			void		set							(bool new_val) {m_models_changed = new_val;};
		};
		property bool	target_locked
		{
			bool		get							() {return m_target_locked;};
			void		set							(bool new_val);
		};

	private:
				void		on_resources_loaded				(xray::resources::queries_result& result);
				void		on_new_model_loaded				(xray::resources::queries_result& result);
				void		on_hypergraph_drag_over			(System::Object^ sender, System::Windows::Forms::DragEventArgs^ e);
				void		on_hypergraph_drag_drop			(System::Object^ sender, System::Windows::Forms::DragEventArgs^ e);
				void		on_hypergraph_click				(System::Object^ sender, System::EventArgs^ e);
				void		on_hypergraph_selection_changed	();
				void		on_drop_animation_node			(System::Windows::Forms::DragEventArgs^ e);
				void		on_drop_operator_node			(System::Windows::Forms::DragEventArgs^ e);
				void		on_drop_root_node				(System::Windows::Forms::DragEventArgs^ e);
				void		on_focus_losted					(System::Object^, System::EventArgs^);
				void		on_document_activated			(System::Object^ obj, System::EventArgs^);

animation_playback::animation_item^	item_by_user_data		(u32 ud);
				void		create_animation_item			(xray::animation::mixing::animation_lexeme_ptr l);
				void		update_animation_items			(vectora<xray::animation::editor_animation_state> const& cur_anim_states);

				void		recalc_mixer_events				( );
				bool		can_create_expression_from_node	( animation_viewer_graph_node_base^ n );
				expression	create_expression_from_node		( animation_viewer_graph_node_base^ n, mutable_buffer& buffer, bool create_anim_items );
				animation_lexeme_ptr create_animation_lexeme( animation_viewer_graph_node_animation^ anim, mutable_buffer& buffer, bool create_anim_items );
				animation_lexeme_ptr create_synk_animation_lexeme( animation_viewer_graph_node_animation^ anim, mutable_buffer& buffer, bool create_anim_items );
				void		active_model_matrix_modified	( cli::array<System::String^>^ );
				void		set_active_model				( System::String^ name );
				u32			get_user_data					( System::String^ anim_name );
				void		set_target_impl					( );
				void		swap_target_ids					( );

	private:
		System::ComponentModel::Container^		components;
		viewer_editor^							m_editor;
		animation_viewer_hypergraph^			m_hypergraph;
		System::Windows::Forms::SaveFileDialog^	m_save_file_dialog;
		bool									m_first_save;
		bool									m_is_loaded;
		bool									m_models_changed;
		xray::timing::timer*					m_timer;
		models_list^							m_models;
		animation_model^							m_active_model;
		animation_viewer_graph_node_base^		m_target;
		lexemes_collection_type*				m_lexemes;
		animation_items_list^					m_animation_items;
		System::String^							m_error_message;
		float									m_max_time;
		float									m_current_time;
		float									m_time_scale;
		bool									m_paused;
		bool									m_target_locked;

		ids_by_anim_name_type^					m_assigned_ids;
		List<u32>^								m_prev_target_ids;
		List<u32>^								m_cur_target_ids;
	}; // ref class viewer_document
} // namespace animation_editor
} // namespace xray
#endif // #ifndef XRAY_ANIMATION_EDITOR_VIEWER_DOCUMENT_H_INCLUDED