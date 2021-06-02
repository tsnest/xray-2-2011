////////////////////////////////////////////////////////////////////////////
//	Created		: 17.01.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_setup_document.h"
#include "animation_setup_manager.h"
#include "animation_setup_channel_added_removed_commands.h"
#include "animation_setup_item_added_removed_commands.h"
#include "animation_setup_property_changed_commands.h"
#include "animations_list_panel.h"
#include "animation_node_clip_instance.h"

using xray::animation_editor::animation_setup_document;
using xray::animation_editor::animation_setup_manager;

void animation_setup_document::init_components()
{
	m_upper_panel = gcnew Panel();

	m_ts_and_fps_panel = gcnew Panel();
	m_time_scale_label = gcnew Label();
	m_time_scale_spin = gcnew NumericUpDown();
	m_fps_label = gcnew Label();
	m_fps_spin = gcnew NumericUpDown();

	m_max_time_panel = gcnew Panel();
	m_max_time_f_label = gcnew Label();
	m_max_time_s_label = gcnew Label();
	m_max_time_f_spin = gcnew NumericUpDown();
	m_max_time_s_spin = gcnew NumericUpDown();

	m_cur_time_panel = gcnew Panel();
	m_cur_time_f_label = gcnew Label();
	m_cur_time_s_label = gcnew Label();
	m_cur_time_f_spin = gcnew NumericUpDown();
	m_cur_time_s_spin = gcnew NumericUpDown();

	m_buttons_panel = gcnew Panel();
	m_home_button = gcnew Button();
	m_fbwd_button = gcnew Button();
	m_play_button = gcnew Button();
	m_stop_button = gcnew Button();
	m_ffwd_button = gcnew Button();
	m_end_button = gcnew Button();
	m_play_from_beginning_cb = gcnew CheckBox();
	m_follow_animation_thumb = gcnew CheckBox();
	m_snap_to_frames = gcnew CheckBox();
	m_save_first_as_last_cb = gcnew CheckBox();

	m_lower_panel = gcnew Panel();
	m_host = gcnew animation_setup_host();
	m_host->panel->follow_animation_thumb = follow_animation_thumb;
	m_host->panel->snap_to_frames = snap_to_frames;

	SuspendLayout();

	// m_upper_panel
	m_upper_panel->SuspendLayout();
	m_upper_panel->Controls->Add(m_cur_time_panel);
	m_upper_panel->Controls->Add(m_max_time_panel);
	m_upper_panel->Controls->Add(m_ts_and_fps_panel);
	m_upper_panel->Controls->Add(m_buttons_panel);
	m_upper_panel->BorderStyle = BorderStyle::FixedSingle;
	m_upper_panel->Dock = DockStyle::Top;
	m_upper_panel->Location = System::Drawing::Point(0, 0);
	m_upper_panel->Name = L"m_upper_panel";
	m_upper_panel->Size = System::Drawing::Size(284, 94);
	m_upper_panel->TabIndex = 0;

	// m_ts_and_fps_panel
	m_ts_and_fps_panel->SuspendLayout();
	m_ts_and_fps_panel->Controls->Add(m_time_scale_spin);
	m_ts_and_fps_panel->Controls->Add(m_time_scale_label);
	m_ts_and_fps_panel->Controls->Add(m_fps_spin);
	m_ts_and_fps_panel->Controls->Add(m_fps_label);
	m_ts_and_fps_panel->BorderStyle = BorderStyle::FixedSingle;
	m_ts_and_fps_panel->Dock = DockStyle::Left;
	m_ts_and_fps_panel->Location = System::Drawing::Point(0, 0);
	m_ts_and_fps_panel->Name = L"m_ts_and_fps_panel";
	m_ts_and_fps_panel->Size = System::Drawing::Size(120, 47);
	m_ts_and_fps_panel->TabIndex = 0;

	// m_time_scale_label
	m_time_scale_label->Location = System::Drawing::Point(0, 0);
	m_time_scale_label->Name = L"m_time_scale_spin";
	m_time_scale_label->Size = System::Drawing::Size(65, 23);
	m_time_scale_label->TabIndex = 0;
	m_time_scale_label->Text = L"Time scale:";
	m_time_scale_label->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;

	// m_fps_label
	m_fps_label->Location = System::Drawing::Point(0, 23);
	m_fps_label->Name = L"m_fps_label";
	m_fps_label->Size = System::Drawing::Size(65, 23);
	m_fps_label->TabIndex = 2;
	m_fps_label->Text = L"Panel fps:";
	m_fps_label->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;

	// m_time_scale_spin
	cli::safe_cast<System::ComponentModel::ISupportInitialize^>(m_time_scale_spin)->BeginInit();
	m_time_scale_spin->Location = System::Drawing::Point(65, 0);
	m_time_scale_spin->DecimalPlaces = 3;
	m_time_scale_spin->Increment = System::Decimal(0.01f);
	m_time_scale_spin->Maximum = System::Decimal(10.0f);
	m_time_scale_spin->Minimum = System::Decimal(-10.0f);
	m_time_scale_spin->Name = L"m_time_scale_spin";
	m_time_scale_spin->Size = System::Drawing::Size(55, 23);
	m_time_scale_spin->TabIndex = 1;
	m_time_scale_spin->Value = System::Decimal(time_scale);
	m_time_scale_spin->ValueChanged += gcnew System::EventHandler(this, &animation_setup_document::on_time_scale_spin_value_changed);
	cli::safe_cast<System::ComponentModel::ISupportInitialize^ >(m_time_scale_spin)->EndInit();

	// m_fps_spin
	cli::safe_cast<System::ComponentModel::ISupportInitialize^>(m_fps_spin)->BeginInit();
	m_fps_spin->Location = System::Drawing::Point(65, 23);
	m_fps_spin->DecimalPlaces = 0;
	m_fps_spin->Increment = System::Decimal(1);
	m_fps_spin->Maximum = System::Decimal(1000);
	m_fps_spin->Minimum = System::Decimal(1);
	m_fps_spin->Name = L"m_fps_spin";
	m_fps_spin->Size = System::Drawing::Size(55, 23);
	m_fps_spin->TabIndex = 3;
	m_fps_spin->Value = System::Decimal(panel_fps);
	m_fps_spin->ValueChanged += gcnew System::EventHandler(this, &animation_setup_document::on_fps_spin_value_changed);
	cli::safe_cast<System::ComponentModel::ISupportInitialize^ >(m_fps_spin)->EndInit();
	m_fps_spin->Enabled = false;

	// m_max_time_panel
	m_max_time_panel->SuspendLayout();
	m_max_time_panel->Controls->Add(m_max_time_f_label);
	m_max_time_panel->Controls->Add(m_max_time_s_label);
	m_max_time_panel->Controls->Add(m_max_time_f_spin);
	m_max_time_panel->Controls->Add(m_max_time_s_spin);
	m_max_time_panel->BorderStyle = BorderStyle::FixedSingle;
	m_max_time_panel->Dock = DockStyle::Left;
	m_max_time_panel->Location = System::Drawing::Point(0, 0);
	m_max_time_panel->Name = L"m_max_time_panel";
	m_max_time_panel->Size = System::Drawing::Size(175, 47);
	m_max_time_panel->TabIndex = 1;

	// m_max_time_f_label
	m_max_time_f_label->Location = System::Drawing::Point(0, 0);
	m_max_time_f_label->Name = L"m_max_time_label";
	m_max_time_f_label->Size = System::Drawing::Size(105, 23);
	m_max_time_f_label->TabIndex = 0;
	m_max_time_f_label->Text = L"Max time in frames:";
	m_max_time_f_label->TextAlign = System::Drawing::ContentAlignment::MiddleRight;

	// m_max_time_s_label
	m_max_time_s_label->Location = System::Drawing::Point(0, 23);
	m_max_time_s_label->Name = L"m_max_time_s_label";
	m_max_time_s_label->Size = System::Drawing::Size(105, 23);
	m_max_time_s_label->TabIndex = 1;
	m_max_time_s_label->Text = L"in seconds:";
	m_max_time_s_label->TextAlign = System::Drawing::ContentAlignment::MiddleRight;

	// m_max_time_f_spin
	cli::safe_cast<System::ComponentModel::ISupportInitialize^>(m_max_time_f_spin)->BeginInit();
	m_max_time_f_spin->Location = System::Drawing::Point(105, 0);
	m_max_time_f_spin->DecimalPlaces = 0;
	m_max_time_f_spin->Increment = System::Decimal(1);
	m_max_time_f_spin->Maximum = System::Decimal(3600)*m_fps_spin->Value;
	m_max_time_f_spin->Minimum = System::Decimal(1);
	m_max_time_f_spin->Name = L"m_max_time_f_spin";
	m_max_time_f_spin->Size = System::Drawing::Size(70, 23);
	m_max_time_f_spin->TabIndex = 2;
	m_max_time_f_spin->ValueChanged += gcnew System::EventHandler(this, &animation_setup_document::on_max_time_f_spin_value_changed);
	cli::safe_cast<System::ComponentModel::ISupportInitialize^ >(m_max_time_f_spin)->EndInit();

	// m_max_time_s_spin
	cli::safe_cast<System::ComponentModel::ISupportInitialize^>(m_max_time_s_spin)->BeginInit();
	m_max_time_s_spin->Location = System::Drawing::Point(105, 23);
	m_max_time_s_spin->DecimalPlaces = 3;
	m_max_time_s_spin->Increment = System::Decimal(0.01f);
	m_max_time_s_spin->Maximum = System::Decimal(3600.0f);
	m_max_time_s_spin->Minimum = System::Decimal(0.001f);
	m_max_time_s_spin->Name = L"m_max_time_s_spin";
	m_max_time_s_spin->Size = System::Drawing::Size(70, 23);
	m_max_time_s_spin->TabIndex = 3;
	m_max_time_s_spin->ValueChanged += gcnew System::EventHandler(this, &animation_setup_document::on_max_time_s_spin_value_changed);
	cli::safe_cast<System::ComponentModel::ISupportInitialize^ >(m_max_time_s_spin)->EndInit();

	// m_cur_time_panel
	m_cur_time_panel->SuspendLayout();
	m_cur_time_panel->Controls->Add(m_cur_time_f_label);
	m_cur_time_panel->Controls->Add(m_cur_time_s_label);
	m_cur_time_panel->Controls->Add(m_cur_time_f_spin);
	m_cur_time_panel->Controls->Add(m_cur_time_s_spin);
	m_cur_time_panel->BorderStyle = BorderStyle::FixedSingle;
	m_cur_time_panel->Dock = DockStyle::Left;
	m_cur_time_panel->Location = System::Drawing::Point(0, 0);
	m_cur_time_panel->Name = L"m_cur_time_panel";
	m_cur_time_panel->Size = System::Drawing::Size(200, 47);
	m_cur_time_panel->TabIndex = 2;

	// m_cur_time_f_label
	m_cur_time_f_label->Location = System::Drawing::Point(0, 0);
	m_cur_time_f_label->Name = L"m_cur_time_f_label";
	m_cur_time_f_label->Size = System::Drawing::Size(125, 23);
	m_cur_time_f_label->TabIndex = 0;
	m_cur_time_f_label->Text = L"Current time in frames:";
	m_cur_time_f_label->TextAlign = System::Drawing::ContentAlignment::MiddleRight;

	// m_cur_time_s_label
	m_cur_time_s_label->Location = System::Drawing::Point(0, 23);
	m_cur_time_s_label->Name = L"m_cur_time_s_label";
	m_cur_time_s_label->Size = System::Drawing::Size(125, 23);
	m_cur_time_s_label->TabIndex = 1;
	m_cur_time_s_label->Text = L"in seconds:";
	m_cur_time_s_label->TextAlign = System::Drawing::ContentAlignment::MiddleRight;

	// m_cur_time_f_spin
	cli::safe_cast<System::ComponentModel::ISupportInitialize^>(m_cur_time_f_spin)->BeginInit();
	m_cur_time_f_spin->Location = System::Drawing::Point(125, 0);
	m_cur_time_f_spin->DecimalPlaces = 3;
	m_cur_time_f_spin->Increment = System::Decimal(0.01f);
	m_cur_time_f_spin->Maximum = m_max_time_f_spin->Value;
	m_cur_time_f_spin->Minimum = System::Decimal(0.0f);
	m_cur_time_f_spin->Name = L"m_cur_time_f_spin";
	m_cur_time_f_spin->Size = System::Drawing::Size(75, 23);
	m_cur_time_f_spin->TabIndex = 2;
	m_cur_time_f_spin->Value = System::Decimal(current_time_in_ms/1000.0f)*m_fps_spin->Value;
	m_cur_time_f_spin->ValueChanged += gcnew System::EventHandler(this, &animation_setup_document::on_cur_time_f_spin_value_changed);
	cli::safe_cast<System::ComponentModel::ISupportInitialize^ >(m_cur_time_f_spin)->EndInit();

	// m_cur_time_s_spin
	cli::safe_cast<System::ComponentModel::ISupportInitialize^>(m_cur_time_s_spin)->BeginInit();
	m_cur_time_s_spin->Location = System::Drawing::Point(125, 23);
	m_cur_time_s_spin->DecimalPlaces = 3;
	m_cur_time_s_spin->Increment = System::Decimal(0.01f);
	m_cur_time_s_spin->Maximum = m_max_time_s_spin->Value;
	m_cur_time_s_spin->Minimum = System::Decimal(0.0f);
	m_cur_time_s_spin->Name = L"m_cur_time_s_spin";
	m_cur_time_s_spin->Size = System::Drawing::Size(75, 23);
	m_cur_time_s_spin->TabIndex = 3;
	m_cur_time_s_spin->Value = System::Decimal(current_time_in_ms/1000.0f);
	m_cur_time_s_spin->ValueChanged += gcnew System::EventHandler(this, &animation_setup_document::on_cur_time_s_spin_value_changed);
	cli::safe_cast<System::ComponentModel::ISupportInitialize^ >(m_cur_time_s_spin)->EndInit();

	// m_buttons_panel
	m_buttons_panel->SuspendLayout();
	m_buttons_panel->Controls->Add(m_home_button);
	m_buttons_panel->Controls->Add(m_fbwd_button);
	m_buttons_panel->Controls->Add(m_play_button);
	m_buttons_panel->Controls->Add(m_stop_button);
	m_buttons_panel->Controls->Add(m_ffwd_button);
	m_buttons_panel->Controls->Add(m_end_button);
	m_buttons_panel->Controls->Add(m_play_from_beginning_cb);
	m_buttons_panel->Controls->Add(m_follow_animation_thumb);
	m_buttons_panel->Controls->Add(m_snap_to_frames);
	m_buttons_panel->Controls->Add(m_save_first_as_last_cb);
	m_buttons_panel->Dock = DockStyle::Bottom;
	m_buttons_panel->Location = System::Drawing::Point(0, 0);
	m_buttons_panel->Name = L"m_buttons_panel";
	m_buttons_panel->Size = System::Drawing::Size(134, 47);
	m_buttons_panel->TabIndex = 2;

	// m_home_button
	m_home_button->Location = System::Drawing::Point(5, 1);
	m_home_button->Name = L"m_home_button";
	m_home_button->Size = System::Drawing::Size(50, 23);
	m_home_button->TabIndex = 0;
	m_home_button->Text = L"|<";
	m_home_button->UseVisualStyleBackColor = true;
	m_home_button->Click += gcnew System::EventHandler(this, &animation_setup_document::on_home_clicked);

	// m_fbwd_button
	m_fbwd_button->Location = System::Drawing::Point(56, 1);
	m_fbwd_button->Name = L"m_fbwd_button";
	m_fbwd_button->Size = System::Drawing::Size(50, 23);
	m_fbwd_button->TabIndex = 1;
	m_fbwd_button->Text = L"<";
	m_fbwd_button->UseVisualStyleBackColor = true;
	m_fbwd_button->Click += gcnew System::EventHandler(this, &animation_setup_document::on_fbwd_clicked);

	// m_play_button
	m_play_button->Location = System::Drawing::Point(107, 1);
	m_play_button->Name = L"m_play_button";
	m_play_button->Size = System::Drawing::Size(50, 23);
	m_play_button->TabIndex = 2;
	m_play_button->Text = L"Play";
	m_play_button->UseVisualStyleBackColor = true;
	m_play_button->Click += gcnew System::EventHandler(this, &animation_setup_document::on_play_clicked);

	// m_stop_button
	m_stop_button->Location = System::Drawing::Point(158, 1);
	m_stop_button->Name = L"m_stop_button";
	m_stop_button->Size = System::Drawing::Size(50, 23);
	m_stop_button->TabIndex = 3;
	m_stop_button->Text = L"Stop";
	m_stop_button->UseVisualStyleBackColor = true;
	m_stop_button->Click += gcnew System::EventHandler(this, &animation_setup_document::on_stop_clicked);

	// m_ffwd_button
	m_ffwd_button->Location = System::Drawing::Point(209, 1);
	m_ffwd_button->Name = L"m_ffwd_button";
	m_ffwd_button->Size = System::Drawing::Size(50, 23);
	m_ffwd_button->TabIndex = 4;
	m_ffwd_button->Text = L">";
	m_ffwd_button->UseVisualStyleBackColor = true;
	m_ffwd_button->Click += gcnew System::EventHandler(this, &animation_setup_document::on_ffwd_clicked);

	// m_end_button
	m_end_button->Location = System::Drawing::Point(260, 1);
	m_end_button->Name = L"m_end_button";
	m_end_button->Size = System::Drawing::Size(50, 23);
	m_end_button->TabIndex = 5;
	m_end_button->Text = L">|";
	m_end_button->UseVisualStyleBackColor = true;
	m_end_button->Click += gcnew System::EventHandler(this, &animation_setup_document::on_end_clicked);

	// m_play_from_beginning_cb
	m_play_from_beginning_cb->Checked = play_from_beginning;
	m_play_from_beginning_cb->Location = System::Drawing::Point(5, 23);
	m_play_from_beginning_cb->Name = L"m_play_from_beginning_cb";
	m_play_from_beginning_cb->Size = System::Drawing::Size(110, 23);
	m_play_from_beginning_cb->TabIndex = 6;
	m_play_from_beginning_cb->Text = L"Play from begin";
	m_play_from_beginning_cb->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
	m_play_from_beginning_cb->CheckedChanged += gcnew System::EventHandler(this, &animation_setup_document::on_play_from_beginning_check_changed);

	// m_follow_animation_thumb
	m_follow_animation_thumb->Checked = follow_animation_thumb;
	m_follow_animation_thumb->Location = System::Drawing::Point(115, 23);
	m_follow_animation_thumb->Name = L"m_follow_animation_thumb";
	m_follow_animation_thumb->Size = System::Drawing::Size(130, 23);
	m_follow_animation_thumb->TabIndex = 6;
	m_follow_animation_thumb->Text = L"Follow current time";
	m_follow_animation_thumb->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
	m_follow_animation_thumb->CheckedChanged += gcnew System::EventHandler(this, &animation_setup_document::on_follow_animation_thumb_check_changed);

	// m_snap_to_frames
	m_snap_to_frames->Checked = snap_to_frames;
	m_snap_to_frames->Location = System::Drawing::Point(245, 23);
	m_snap_to_frames->Name = L"m_snap_to_frames";
	m_snap_to_frames->Size = System::Drawing::Size(110, 23);
	m_snap_to_frames->TabIndex = 6;
	m_snap_to_frames->Text = L"Snap to frames";
	m_snap_to_frames->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
	m_snap_to_frames->CheckedChanged += gcnew System::EventHandler(this, &animation_setup_document::on_snap_to_frames_check_changed);

	// m_save_first_as_last_cb
	m_save_first_as_last_cb->Checked = snap_to_frames;
	m_save_first_as_last_cb->Location = System::Drawing::Point(355, 23);
	m_save_first_as_last_cb->Name = L"m_save_first_as_last_cb";
	m_save_first_as_last_cb->Size = System::Drawing::Size(150, 23);
	m_save_first_as_last_cb->TabIndex = 7;
	m_save_first_as_last_cb->Text = L"Save first interval as last";
	m_save_first_as_last_cb->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
	m_save_first_as_last_cb->CheckedChanged += gcnew System::EventHandler(this, &animation_setup_document::on_save_first_as_last_check_changed);
	
	// m_lower_panel
	m_lower_panel->SuspendLayout();
	m_lower_panel->Controls->Add(m_host);
	m_lower_panel->AutoScroll = true;
	m_lower_panel->BorderStyle = BorderStyle::FixedSingle;
	m_lower_panel->Dock = DockStyle::Fill;
	m_lower_panel->Location = System::Drawing::Point(0, 45);
	m_lower_panel->Name = L"m_lower_panel";
	m_lower_panel->Size = System::Drawing::Size(284, 150);
	m_lower_panel->TabIndex = 1;

	// m_host
	m_host->Dock = DockStyle::Fill;
	m_host->TabIndex = 0;
	m_host->panel->max_time = m_max_time;
	m_host->panel->fps = (float)m_panel_fps;
	m_host->panel->PropertyChanged += gcnew System::ComponentModel::PropertyChangedEventHandler(this, &animation_setup_document::on_setup_panel_property_changed);
	m_host->panel->ask_for_create_channel += gcnew System::EventHandler<setup_panel_event_args^>(this, &animation_setup_document::on_channel_added);
	m_host->panel->ask_for_remove_channel += gcnew System::EventHandler<setup_panel_event_args^>(this, &animation_setup_document::on_channel_removed);
	m_host->panel->ask_for_create_item += gcnew System::EventHandler<setup_panel_event_args^>(this, &animation_setup_document::on_item_added);
	m_host->panel->ask_for_remove_item += gcnew System::EventHandler<setup_panel_event_args^>(this, &animation_setup_document::on_item_removed);
	m_host->panel->ask_for_change_item_property += gcnew System::EventHandler<setup_panel_event_args^>(this, &animation_setup_document::on_item_property_changed);
	m_host->panel->ask_for_copy_channel += gcnew System::EventHandler<setup_panel_copy_event_args^>(this, &animation_setup_document::on_channel_copy);
	m_host->panel->ask_for_cut_channel += gcnew System::EventHandler<setup_panel_copy_event_args^>(this, &animation_setup_document::on_channel_cut);
	m_host->panel->ask_for_paste_channel += gcnew System::EventHandler(this, &animation_setup_document::on_channel_paste);

	AllowEndUserDocking = false;
	Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &animation_setup_document::on_paint);
	VisibleChanged += gcnew System::EventHandler(this, &animation_setup_document::on_document_activated);
	Controls->Add(m_lower_panel);
	Controls->Add(m_upper_panel);
	m_upper_panel->ResumeLayout(false);
	m_lower_panel->ResumeLayout(false);
	set_buttons_enable(false);
	ResumeLayout(false);
	PerformLayout();
}

void animation_setup_document::on_form_closing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e)
{
	m_setup_manager->show_properties(nullptr);
	super::on_form_closing(sender, e);
}

void animation_setup_document::set_buttons_enable(bool val)
{
	m_home_button->Enabled = val;
	m_fbwd_button->Enabled = val;
	m_play_button->Enabled = val;
	m_stop_button->Enabled = val;
	m_ffwd_button->Enabled = val;
	m_end_button->Enabled = val;
}

void animation_setup_document::on_document_activated(System::Object^, System::EventArgs^)
{
	if(Visible && m_clip!=nullptr)
	{
		set_target();
		m_setup_manager->animations_list->track_active_item(this->Name);
		m_setup_manager->show_properties(m_clip->container());
	}
}

void animation_setup_document::paused::set(bool new_val)
{
	m_paused = new_val;
	(m_paused) ? m_play_button->Text = L"Play" : m_play_button->Text = L"Pause";
	if(!m_paused)
		m_timer->start();
}

void animation_setup_document::time_scale::set(float new_val)
{
	m_time_scale = new_val;
	m_ms_in_frame = 1000.0f/m_panel_fps*m_time_scale;
}

void animation_setup_document::panel_fps::set(u32 new_val)
{
	R_ASSERT(new_val>0);
	m_panel_fps = new_val;
	m_ms_in_frame = 1000.0f/m_panel_fps*m_time_scale;
}

float animation_setup_document::current_time_in_ms::get()
{
	return m_current_time_in_ms;
}

void animation_setup_document::current_time_in_ms::set(float val)
{
	float const check_value = m_current_time_in_ms;
	if(xray::math::is_similar(val, check_value))
		return;

	if(play_from_beginning)
	{
		if(val>m_max_time)
			val = 0.0f;
		else if(val<0.f)
			val = m_max_time;
	}
	else
	{
		if(val>m_max_time)
		{
			val = m_max_time;
			paused = !paused;
		}
		else if(val<0.f)
		{
			val = 0.0f;
			paused = !paused;
		}
	}

	m_current_time_in_ms = val;
	R_ASSERT(m_current_time_in_ms>=0.0f && m_current_time_in_ms<=m_max_time);
	set_target();
	update_panel_values(m_current_time_in_ms);
}

void animation_setup_document::update_panel_values(float val)
{
	if(!m_spin_is_updating)
	{
		m_spin_is_updating = true;
		m_cur_time_s_spin->Value = System::Decimal(val/1000.0f);
		m_cur_time_f_spin->Value = System::Decimal(xray::math::clamp_r(
			val/1000.0f*(float)m_fps_spin->Value, 
			(float)m_cur_time_f_spin->Minimum, 
			(float)m_cur_time_f_spin->Maximum - xray::math::epsilon_5)
		);
		m_host->panel->animation_time = val;

		m_play_from_beginning_cb->Checked = play_from_beginning;
		m_follow_animation_thumb->Checked = follow_animation_thumb;
		m_snap_to_frames->Checked = snap_to_frames;
		m_host->panel->snap_to_frames = snap_to_frames;
		m_host->panel->follow_animation_thumb = follow_animation_thumb;
		m_fps_spin->Value = System::Decimal(panel_fps);
		m_time_scale_spin->Value = System::Decimal(time_scale);
		m_max_time_s_spin->Value = System::Decimal(m_max_time/1000.0f);
		m_max_time_f_spin->Value = System::Decimal(m_max_time/1000.0f)*m_fps_spin->Value;
		m_host->panel->max_time = m_max_time;
		m_host->panel->fps = (float)m_panel_fps;
		Invalidate(false);
		m_spin_is_updating = false;
	}
}

void animation_setup_document::on_home_clicked(System::Object^, System::EventArgs^)
{
	current_time_in_ms = 0.0f;
}

void animation_setup_document::on_fbwd_clicked(System::Object^, System::EventArgs^)
{
	if(m_snap_to_frames->Checked)
	{
		float const koef = m_panel_fps / 1000.0f;
		float const cur_frame = (float)System::Math::Round(m_current_time_in_ms * koef);
		float const prev_frame = cur_frame - 1.0f;
		paused = true;
		current_time_in_ms = prev_frame / koef;
	}
	else
	{
		paused = true;
		current_time_in_ms -= m_ms_in_frame;
	}
}

void animation_setup_document::on_play_clicked(System::Object^, System::EventArgs^)
{
	paused = !paused;
}

void animation_setup_document::on_stop_clicked(System::Object^, System::EventArgs^)
{
	paused = true;
	current_time_in_ms = 0.0f;
}

void animation_setup_document::on_ffwd_clicked(System::Object^, System::EventArgs^)
{
	if(m_snap_to_frames->Checked)
	{
		float const koef = m_panel_fps / 1000.0f;
		float const cur_frame = (float)System::Math::Round(current_time_in_ms * koef);
		float const next_frame = cur_frame + 1.0f;
		paused = true;
		current_time_in_ms = next_frame / koef;
	}
	else
	{
		paused = true;
		current_time_in_ms += m_ms_in_frame;
	}
}

void animation_setup_document::on_end_clicked(System::Object^, System::EventArgs^)
{
	current_time_in_ms = m_max_time;
}

void animation_setup_document::on_time_scale_spin_value_changed(System::Object^, System::EventArgs^)
{
	time_scale = (float)m_time_scale_spin->Value;
}

void animation_setup_document::on_fps_spin_value_changed(System::Object^, System::EventArgs^)
{
	if(!m_spin_is_updating)
	{
		m_spin_is_updating = true;
		panel_fps = (u32)m_fps_spin->Value;
		m_max_time_f_spin->Value = System::Decimal(m_max_time/1000.0f)*m_fps_spin->Value;
		m_cur_time_f_spin->Maximum = m_max_time_f_spin->Value;
		m_cur_time_f_spin->Value = System::Decimal(current_time_in_ms/1000.0f)*m_fps_spin->Value;
		m_host->panel->max_time = m_max_time;
		m_host->panel->fps = (float)m_panel_fps;
		m_spin_is_updating = false;
	}
}

void animation_setup_document::on_max_time_s_spin_value_changed(System::Object^, System::EventArgs^)
{
	if(!m_spin_is_updating)
	{
		m_spin_is_updating = true;
		m_max_time = (float)m_max_time_s_spin->Value*1000.0f;
		System::Decimal v = System::Decimal(m_max_time/1000.0f)*m_fps_spin->Value;
		m_max_time_f_spin->Value = v;
		m_cur_time_f_spin->Maximum = m_max_time_f_spin->Value;
		m_cur_time_s_spin->Maximum = m_max_time_s_spin->Value;
		m_host->panel->max_time = m_max_time;
		m_spin_is_updating = false;
	}
}

void animation_setup_document::on_max_time_f_spin_value_changed(System::Object^, System::EventArgs^)
{
	if(!m_spin_is_updating)
	{
		m_spin_is_updating = true;
		m_max_time = float(m_max_time_f_spin->Value/m_fps_spin->Value)*1000.0f;
		m_max_time_s_spin->Value = System::Decimal(m_max_time/1000.0f);
		m_cur_time_f_spin->Maximum = m_max_time_f_spin->Value;
		m_cur_time_s_spin->Maximum = m_max_time_s_spin->Value;
		m_host->panel->max_time = m_max_time;
		m_spin_is_updating = false;
	}
}

void animation_setup_document::on_cur_time_s_spin_value_changed(System::Object^, System::EventArgs^)
{
	if(!m_spin_is_updating)
	{
		m_spin_is_updating = true;
		current_time_in_ms = float(m_cur_time_s_spin->Value)*1000.0f;
		m_cur_time_f_spin->Value = System::Decimal(current_time_in_ms/1000.0f)*m_fps_spin->Value;
		m_host->panel->animation_time = float(m_cur_time_s_spin->Value)*1000.0f;
		m_spin_is_updating = false;
	}
}

void animation_setup_document::on_cur_time_f_spin_value_changed(System::Object^, System::EventArgs^)
{
	if(!m_spin_is_updating)
	{
		m_spin_is_updating = true;
		current_time_in_ms = float(m_cur_time_f_spin->Value/m_fps_spin->Value)*1000.0f;
		m_cur_time_s_spin->Value = System::Decimal(current_time_in_ms/1000.0f);
		m_host->panel->animation_time = float(m_cur_time_f_spin->Value/m_fps_spin->Value)*1000.0f;
		m_spin_is_updating = false;
	}
}

void animation_setup_document::on_setup_panel_property_changed(System::Object^, System::ComponentModel::PropertyChangedEventArgs^ e)
{
	if(!m_spin_is_updating)
	{
		if(e->PropertyName=="animation_time")
			current_time_in_ms = m_host->panel->animation_time;
	}
}

void animation_setup_document::on_play_from_beginning_check_changed(System::Object^ sender, System::EventArgs^)
{
	bool val = dynamic_cast<CheckBox^>(sender)?dynamic_cast<CheckBox^>(sender)->Checked:dynamic_cast<ToolStripMenuItem^>(sender)->Checked;
	play_from_beginning = val;
	update_panel_values(m_current_time_in_ms);
}

void animation_setup_document::on_follow_animation_thumb_check_changed(System::Object^ sender, System::EventArgs^)
{
	bool val = dynamic_cast<CheckBox^>(sender)?dynamic_cast<CheckBox^>(sender)->Checked:dynamic_cast<ToolStripMenuItem^>(sender)->Checked;
	follow_animation_thumb = val;
	m_host->panel->follow_animation_thumb = val;
	update_panel_values(m_current_time_in_ms);
}

void animation_setup_document::on_snap_to_frames_check_changed(System::Object^ sender, System::EventArgs^)
{
	bool val = dynamic_cast<CheckBox^>(sender)?dynamic_cast<CheckBox^>(sender)->Checked:dynamic_cast<ToolStripMenuItem^>(sender)->Checked;
	snap_to_frames = val;
	m_host->panel->snap_to_frames = val;
	update_panel_values(m_current_time_in_ms);
}

void animation_setup_document::on_save_first_as_last_check_changed(System::Object^, System::EventArgs^)
{
	save_first_as_last = m_save_first_as_last_cb->Checked;
}

void animation_setup_document::on_paint(System::Object^, System::Windows::Forms::PaintEventArgs^)
{
	if(m_command_engine->saved_flag())
		is_saved = true;
	else
		is_saved = false;
}

void animation_setup_document::on_channel_added(System::Object^, setup_panel_event_args^ e)
{
	Invalidate(false);
	is_saved &= !m_command_engine->run(gcnew animation_setup_channel_added_command(this, e));
}

void animation_setup_document::on_channel_removed(System::Object^, setup_panel_event_args^ e)
{
	Invalidate(false);
	is_saved &= !m_command_engine->run(gcnew animation_setup_channel_removed_command(this, e));
}

void animation_setup_document::on_item_added(System::Object^, setup_panel_event_args^ e)
{
	Invalidate(false);
	is_saved &= !m_command_engine->run(gcnew animation_setup_item_added_command(this, e));
}

void animation_setup_document::on_item_removed(System::Object^, setup_panel_event_args^ e)
{
	Invalidate(false);
	is_saved &= !m_command_engine->run(gcnew animation_setup_item_removed_command(this, e));
}

void animation_setup_document::on_item_property_changed(System::Object^, setup_panel_event_args^ e)
{
	Invalidate(false);
	is_saved &= !m_command_engine->run(gcnew animation_setup_item_property_changed_command(this, e));
}

void animation_setup_document::on_property_changed(System::String^ l, Object^ new_val, Object^ old_val)
{
	if(!disable_events_handling)
		is_saved &= !m_command_engine->run(gcnew animation_setup_property_changed_command(this, l, new_val, old_val));
}

animation_setup_document::wpf_property_container^	animation_setup_document::interpolator()
{
	return m_clip->interpolator;
}

animation_setup_document::wpf_property_container^	animation_setup_document::container::get() 
{
	return m_clip->container();
}
