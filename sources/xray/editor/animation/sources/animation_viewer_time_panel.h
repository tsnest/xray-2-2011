////////////////////////////////////////////////////////////////////////////
//	Created		: 27.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_VIEWER_TIME_PANEL_H_INCLUDED
#define ANIMATION_VIEWER_TIME_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace xray::editor::wpf_controls;

namespace xray {
namespace animation_editor {
	ref class viewer_editor;
	ref class viewer_document;

	public ref class animation_viewer_time_panel: public WeifenLuo::WinFormsUI::Docking::DockContent
	{
		typedef System::Collections::Generic::IEnumerable<animation_playback::animation_item^> animation_items_list;
	public:
				animation_viewer_time_panel	(viewer_editor^ ed);
				~animation_viewer_time_panel();

		void	set_buttons_enable								(bool val);
		void	update											();
		void	update_time										();
		void	set_items										(animation_items_list^ lst);
		animation_items_list^ get_items							() {return m_host->animation_items;};
		void	clear_items										();
		void	on_play_from_beginning_check_changed			(System::Object^ sender, System::EventArgs^);
		void	on_follow_animation_thumb_check_changed			(System::Object^ sender, System::EventArgs^);
		void	on_snap_to_frames_check_changed					(System::Object^ sender, System::EventArgs^);
		void	on_camera_follows_npc_check_changed				(System::Object^ sender, System::EventArgs^);
		bool	get_snapping									();
		bool	get_following									();
		float	get_time_layout_scale							();
		void	set_time_layout_scale							(float val);

	private:
		void	on_home_clicked									(System::Object^, System::EventArgs^);
		void	on_fbwd_clicked									(System::Object^, System::EventArgs^);
		void	on_play_clicked									(System::Object^, System::EventArgs^);
		void	on_stop_clicked									(System::Object^, System::EventArgs^);
		void	on_ffwd_clicked									(System::Object^, System::EventArgs^);
		void	on_end_clicked									(System::Object^, System::EventArgs^);
		void	on_lock_clicked									(System::Object^, System::EventArgs^);
		void	on_time_scale_spin_value_changed				(System::Object^, System::EventArgs^);
		void	on_max_time_s_spin_value_changed				(System::Object^, System::EventArgs^);
		void	on_max_time_f_spin_value_changed				(System::Object^, System::EventArgs^);
		void	on_cur_time_s_spin_value_changed				(System::Object^, System::EventArgs^);
		void	on_cur_time_f_spin_value_changed				(System::Object^, System::EventArgs^);
		void	on_animation_playback_panel_property_changed	(System::Object^, System::ComponentModel::PropertyChangedEventArgs^ e);
viewer_document^ get_active_doc									();

	private:
		viewer_editor^									m_editor;
		Panel^											m_upper_panel;

		Panel^											m_ts_and_fps_panel;
		Label^											m_time_scale_label;
		NumericUpDown^  								m_time_scale_spin;
		Label^											m_fps_label;
		NumericUpDown^ 									m_fps_spin;

		Panel^											m_max_time_panel;
		Label^											m_max_time_f_label;
		Label^											m_max_time_s_label;
		NumericUpDown^  								m_max_time_f_spin;
		NumericUpDown^ 									m_max_time_s_spin;

		Panel^											m_cur_time_panel;
		Label^											m_cur_time_f_label;
		Label^											m_cur_time_s_label;
		NumericUpDown^  								m_cur_time_f_spin;
		NumericUpDown^ 									m_cur_time_s_spin;

		Panel^											m_buttons_panel;
		Button^											m_home_button;
		Button^											m_fbwd_button;
		Button^											m_play_button;
		Button^											m_stop_button;
		Button^											m_ffwd_button;
		Button^											m_end_button;
		Button^											m_lock_button;
		CheckBox^										m_play_from_beginning_cb;
		CheckBox^										m_follow_animation_thumb;
		CheckBox^										m_snap_to_frames;
		CheckBox^										m_camera_follows_npc;

		Panel^											m_lower_panel;
		animation_playback::animation_playback_host^	m_host;
		bool											disable_events_handling;
	}; // class animation_viewer_time_panel
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_VIEWER_TIME_PANEL_H_INCLUDED