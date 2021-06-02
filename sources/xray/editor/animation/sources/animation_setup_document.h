////////////////////////////////////////////////////////////////////////////
//	Created		: 12.01.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_SETUP_DOCUMENT_H_INCLUDED
#define ANIMATION_SETUP_DOCUMENT_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace xray::editor::wpf_controls::animation_setup;

namespace xray {
namespace timing { 
	class timer;
} // namespace timing
namespace animation { 
	namespace mixing { 
		class animation_lexeme;
	} // namespace mixing
	//struct base_interpolator; 
	struct i_editor_animation;
} // namespace animation
namespace editor_base { 
	ref class command_engine; 
} // namespace editor_base
namespace animation_editor {

	ref class animation_setup_manager;
	//ref class animation_node_clip;
	ref class animation_node_clip_instance;

	public ref class animation_setup_document : public xray::editor::controls::document_base
	{
		typedef xray::editor::controls::document_base super;
		typedef xray::editor::wpf_controls::property_container			wpf_property_container;
	public:
							animation_setup_document				(animation_setup_manager^ m);
							~animation_setup_document				();

		virtual	void		save									() override;
		virtual	void		load									() override;
		virtual	void		undo									() override;
		virtual	void		redo									() override;
		virtual	void		copy									(bool del) override;
		virtual	void		paste									() override;
		virtual	void		select_all								() override;
		virtual	void		del										() override;
				void		set_target								();
				void		tick									();
				void		on_property_changed						(System::String^ l, Object^ new_val, Object^ old_val);
				void		update_properties						();
				void		set_buttons_enable						(bool val);
				void		on_play_from_beginning_check_changed	(System::Object^ sender, System::EventArgs^);
				void		on_follow_animation_thumb_check_changed	(System::Object^ sender, System::EventArgs^);
				void		on_snap_to_frames_check_changed			(System::Object^ sender, System::EventArgs^);
				void		on_save_first_as_last_check_changed		(System::Object^, System::EventArgs^);
				void		load_channel							(xray::configs::lua_config_value const& cfg);
				void		save_all_channels						();
				void		save_channel							(animation_channel^ ch, xray::configs::lua_config_value& cfg);
				void		save_channel							(System::String^ channel_name);
		animation_channel^	find_channel							(System::String^ name);
	animation_channel_item^	find_item								(animation_channel^ ch, System::Guid^ id);
	wpf_property_container^	interpolator							();

		property bool		disable_events_handling;
		property bool		play_from_beginning;
		property bool		follow_animation_thumb;
		property bool		snap_to_frames;
		property float		current_time_in_ms
		{
			float			get										();
			void			set										(float val);
		};
		property float		time_scale
		{
			float			get										() {return m_time_scale;};
			void			set										(float new_val);
		};
		property u32		panel_fps
		{
			u32				get										() {return m_panel_fps;};
			void			set										(u32 new_val);
		};
		property bool		paused
		{
			bool			get										() {return m_paused;};
			void			set										(bool new_val);
		};
		property bool		save_first_as_last
		{
			bool			get										() {return m_save_first_as_last;};
			void			set										(bool new_val) {m_save_first_as_last=new_val;};
		};
		property animation_setup_panel^	setup_panel
		{
			animation_setup_panel^	get								() {return m_host->panel;};
		};
		property wpf_property_container^ container
		{
			wpf_property_container^	get								();
		};

	protected:
		virtual System::Void on_form_closing						(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) override;

	private:
				void		init_components							();
				void		on_resources_loaded						(animation_node_clip_instance^ new_clip);
				void		on_document_activated					(System::Object^ obj, System::EventArgs^);
				void		on_home_clicked							(System::Object^, System::EventArgs^);
				void		on_fbwd_clicked							(System::Object^, System::EventArgs^);
				void		on_play_clicked							(System::Object^, System::EventArgs^);
				void		on_stop_clicked							(System::Object^, System::EventArgs^);
				void		on_ffwd_clicked							(System::Object^, System::EventArgs^);
				void		on_end_clicked							(System::Object^, System::EventArgs^);
				void		on_time_scale_spin_value_changed		(System::Object^, System::EventArgs^);
				void		on_fps_spin_value_changed				(System::Object^, System::EventArgs^);
				void		on_max_time_s_spin_value_changed		(System::Object^, System::EventArgs^);
				void		on_max_time_f_spin_value_changed		(System::Object^, System::EventArgs^);
				void		on_cur_time_s_spin_value_changed		(System::Object^, System::EventArgs^);
				void		on_cur_time_f_spin_value_changed		(System::Object^, System::EventArgs^);
				void		update_panel_values						(float val);
				void		on_paint								(System::Object^, System::Windows::Forms::PaintEventArgs^);
				void		on_setup_panel_property_changed			(System::Object^, System::ComponentModel::PropertyChangedEventArgs^ e);
				void		on_channel_added						(System::Object^, setup_panel_event_args^ e);
				void		on_channel_removed						(System::Object^, setup_panel_event_args^ e);
				void		on_item_added							(System::Object^, setup_panel_event_args^ e);
				void		on_item_removed							(System::Object^, setup_panel_event_args^ e);
				void		on_item_property_changed				(System::Object^, setup_panel_event_args^ e);
				void		on_channel_copy							(System::Object^, setup_panel_copy_event_args^ e);
				void		on_channel_cut							(System::Object^ obj, setup_panel_copy_event_args^ e);
				void		on_channel_paste						(System::Object^, System::EventArgs^);

	private:
		System::ComponentModel::Container^			components;
		animation_setup_manager^					m_setup_manager;
		animation_node_clip_instance^				m_clip;
		xray::editor_base::command_engine^			m_command_engine;

		Panel^										m_upper_panel;
		Panel^										m_ts_and_fps_panel;
		Label^										m_time_scale_label;
		NumericUpDown^  							m_time_scale_spin;
		Label^										m_fps_label;
		NumericUpDown^ 								m_fps_spin;

		Panel^										m_max_time_panel;
		Label^										m_max_time_f_label;
		Label^										m_max_time_s_label;
		NumericUpDown^  							m_max_time_f_spin;
		NumericUpDown^ 								m_max_time_s_spin;

		Panel^										m_cur_time_panel;
		Label^										m_cur_time_f_label;
		Label^										m_cur_time_s_label;
		NumericUpDown^  							m_cur_time_f_spin;
		NumericUpDown^ 								m_cur_time_s_spin;

		Panel^										m_buttons_panel;
		Button^										m_home_button;
		Button^										m_fbwd_button;
		Button^										m_play_button;
		Button^										m_stop_button;
		Button^										m_ffwd_button;
		Button^										m_end_button;
		CheckBox^									m_play_from_beginning_cb;
		CheckBox^									m_follow_animation_thumb;
		CheckBox^									m_snap_to_frames;
		CheckBox^									m_save_first_as_last_cb;

		Panel^										m_lower_panel;
		animation_setup_host^						m_host;

		bool										m_spin_is_updating;
		float										m_current_time_in_ms;
		float										m_ms_in_frame;
		float										m_time_scale;
		float										m_max_time;
		u32											m_panel_fps;
		bool										m_paused;
		bool										m_save_first_as_last;
		xray::timing::timer*						m_timer;
	}; // class animation_setup_document
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_SETUP_DOCUMENT_H_INCLUDED