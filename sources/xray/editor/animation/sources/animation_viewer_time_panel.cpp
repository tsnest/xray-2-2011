////////////////////////////////////////////////////////////////////////////
//	Created		: 27.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_viewer_time_panel.h"
#include "viewer_editor.h"
#include "viewer_document.h"
#include <xray/animation/anim_track_common.h>

using xray::animation_editor::animation_viewer_time_panel;
using xray::animation_editor::viewer_document;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using namespace xray::editor::wpf_controls;
using namespace xray::editor::wpf_controls::animation_playback;

animation_viewer_time_panel::animation_viewer_time_panel(xray::animation_editor::viewer_editor^ ed)
{
	m_editor = ed;
	disable_events_handling = false;
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
	m_lock_button = gcnew Button();
	m_play_from_beginning_cb = gcnew CheckBox();
	m_follow_animation_thumb = gcnew CheckBox();
	m_snap_to_frames = gcnew CheckBox();
	m_camera_follows_npc = gcnew CheckBox();

	m_lower_panel = gcnew Panel();
	m_host = gcnew animation_playback_host();

	SuspendLayout();

	// m_upper_panel
	m_upper_panel->SuspendLayout();
	m_upper_panel->Controls->Add(m_buttons_panel);
	m_upper_panel->Controls->Add(m_cur_time_panel);
	m_upper_panel->Controls->Add(m_max_time_panel);
	m_upper_panel->Controls->Add(m_ts_and_fps_panel);
	m_upper_panel->BorderStyle = BorderStyle::FixedSingle;
	m_upper_panel->Dock = DockStyle::Top;
	m_upper_panel->Location = System::Drawing::Point(0, 0);
	m_upper_panel->Name = L"m_upper_panel";
	m_upper_panel->Size = System::Drawing::Size(284, 47);
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
	m_time_scale_spin->Value = System::Decimal(1.0f);
	m_time_scale_spin->ValueChanged += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_time_scale_spin_value_changed);
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
	m_fps_spin->Value = System::Decimal(animation::default_fps);
	m_fps_spin->Enabled = false;
	cli::safe_cast<System::ComponentModel::ISupportInitialize^ >(m_fps_spin)->EndInit();

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
	m_max_time_f_spin->Maximum = System::Decimal(30*3600);
	m_max_time_f_spin->Minimum = System::Decimal(1);
	m_max_time_f_spin->Name = L"m_max_time_f_spin";
	m_max_time_f_spin->Size = System::Drawing::Size(70, 23);
	m_max_time_f_spin->TabIndex = 2;
	m_max_time_f_spin->Value = System::Decimal(1);
	m_max_time_f_spin->ValueChanged += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_max_time_f_spin_value_changed);
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
	m_max_time_s_spin->ValueChanged += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_max_time_s_spin_value_changed);
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
	m_cur_time_f_spin->Minimum = System::Decimal(0.0f);
	m_cur_time_f_spin->Name = L"m_cur_time_f_spin";
	m_cur_time_f_spin->Size = System::Drawing::Size(75, 23);
	m_cur_time_f_spin->TabIndex = 2;
	m_cur_time_f_spin->Value = System::Decimal(0.0f);
	m_cur_time_f_spin->ValueChanged += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_cur_time_f_spin_value_changed);
	cli::safe_cast<System::ComponentModel::ISupportInitialize^ >(m_cur_time_f_spin)->EndInit();

	// m_cur_time_s_spin
	cli::safe_cast<System::ComponentModel::ISupportInitialize^>(m_cur_time_s_spin)->BeginInit();
	m_cur_time_s_spin->Location = System::Drawing::Point(125, 23);
	m_cur_time_s_spin->DecimalPlaces = 3;
	m_cur_time_s_spin->Increment = System::Decimal(0.01f);
	m_cur_time_s_spin->Minimum = System::Decimal(0.0f);
	m_cur_time_s_spin->Name = L"m_cur_time_s_spin";
	m_cur_time_s_spin->Size = System::Drawing::Size(75, 23);
	m_cur_time_s_spin->TabIndex = 3;
	m_cur_time_s_spin->Value = System::Decimal(0.0f);
	m_cur_time_s_spin->ValueChanged += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_cur_time_s_spin_value_changed);
	cli::safe_cast<System::ComponentModel::ISupportInitialize^ >(m_cur_time_s_spin)->EndInit();

	// m_buttons_panel
	m_buttons_panel->SuspendLayout();
	m_buttons_panel->Controls->Add(m_home_button);
	m_buttons_panel->Controls->Add(m_fbwd_button);
	m_buttons_panel->Controls->Add(m_play_button);
	m_buttons_panel->Controls->Add(m_stop_button);
	m_buttons_panel->Controls->Add(m_ffwd_button);
	m_buttons_panel->Controls->Add(m_end_button);
	m_buttons_panel->Controls->Add(m_lock_button);
	m_buttons_panel->Controls->Add(m_play_from_beginning_cb);
	m_buttons_panel->Controls->Add(m_follow_animation_thumb);
	m_buttons_panel->Controls->Add(m_snap_to_frames);
	m_buttons_panel->Controls->Add(m_camera_follows_npc);
	m_buttons_panel->BorderStyle = BorderStyle::FixedSingle;
	m_buttons_panel->Dock = DockStyle::Fill;
	m_buttons_panel->Location = System::Drawing::Point(0, 0);
	m_buttons_panel->Name = L"m_buttons_panel";
	m_buttons_panel->Size = System::Drawing::Size(134, 23);
	m_buttons_panel->TabIndex = 2;

	// m_home_button
	m_home_button->Location = System::Drawing::Point(5, 1);
	m_home_button->Name = L"m_home_button";
	m_home_button->Size = System::Drawing::Size(50, 23);
	m_home_button->TabIndex = 0;
	m_home_button->Text = L"|<";
	m_home_button->UseVisualStyleBackColor = true;
	m_home_button->Click += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_home_clicked);

	// m_fbwd_button
	m_fbwd_button->Location = System::Drawing::Point(56, 1);
	m_fbwd_button->Name = L"m_fbwd_button";
	m_fbwd_button->Size = System::Drawing::Size(50, 23);
	m_fbwd_button->TabIndex = 1;
	m_fbwd_button->Text = L"<";
	m_fbwd_button->UseVisualStyleBackColor = true;
	m_fbwd_button->Click += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_fbwd_clicked);

	// m_play_button
	m_play_button->Location = System::Drawing::Point(107, 1);
	m_play_button->Name = L"m_play_button";
	m_play_button->Size = System::Drawing::Size(50, 23);
	m_play_button->TabIndex = 2;
	m_play_button->Text = L"Play";
	m_play_button->UseVisualStyleBackColor = true;
	m_play_button->Click += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_play_clicked);

	// m_stop_button
	m_stop_button->Location = System::Drawing::Point(158, 1);
	m_stop_button->Name = L"m_stop_button";
	m_stop_button->Size = System::Drawing::Size(50, 23);
	m_stop_button->TabIndex = 3;
	m_stop_button->Text = L"Stop";
	m_stop_button->UseVisualStyleBackColor = true;
	m_stop_button->Click += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_stop_clicked);

	// m_ffwd_button
	m_ffwd_button->Location = System::Drawing::Point(209, 1);
	m_ffwd_button->Name = L"m_ffwd_button";
	m_ffwd_button->Size = System::Drawing::Size(50, 23);
	m_ffwd_button->TabIndex = 4;
	m_ffwd_button->Text = L">";
	m_ffwd_button->UseVisualStyleBackColor = true;
	m_ffwd_button->Click += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_ffwd_clicked);

	// m_end_button
	m_end_button->Location = System::Drawing::Point(260, 1);
	m_end_button->Name = L"m_end_button";
	m_end_button->Size = System::Drawing::Size(50, 23);
	m_end_button->TabIndex = 5;
	m_end_button->Text = L">|";
	m_end_button->UseVisualStyleBackColor = true;
	m_end_button->Click += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_end_clicked);

	// m_lock_button
	m_lock_button->Location = System::Drawing::Point(311, 1);
	m_lock_button->Name = L"m_lock_button";
	m_lock_button->Size = System::Drawing::Size(50, 23);
	m_lock_button->TabIndex = 5;
	m_lock_button->Text = L"Lock";
	m_lock_button->UseVisualStyleBackColor = true;
	m_lock_button->Click += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_lock_clicked);
	
	// m_play_from_beginning_cb
	m_play_from_beginning_cb->Checked = m_editor->play_from_beginning;
	m_play_from_beginning_cb->Location = System::Drawing::Point(5, 23);
	m_play_from_beginning_cb->Name = L"m_play_from_beginning_cb";
	m_play_from_beginning_cb->Size = System::Drawing::Size(110, 23);
	m_play_from_beginning_cb->TabIndex = 6;
	m_play_from_beginning_cb->Text = L"Play from begin";
	m_play_from_beginning_cb->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
	m_play_from_beginning_cb->CheckedChanged += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_play_from_beginning_check_changed);

	// m_follow_animation_thumb
	m_follow_animation_thumb->Checked = m_editor->follow_animation_thumb;
	m_follow_animation_thumb->Location = System::Drawing::Point(115, 23);
	m_follow_animation_thumb->Name = L"m_follow_animation_thumb";
	m_follow_animation_thumb->Size = System::Drawing::Size(130, 23);
	m_follow_animation_thumb->TabIndex = 6;
	m_follow_animation_thumb->Text = L"Follow current time";
	m_follow_animation_thumb->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
	m_follow_animation_thumb->CheckedChanged += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_follow_animation_thumb_check_changed);

	// m_snap_to_frames
	m_snap_to_frames->Checked = m_editor->snap_to_frames;
	m_snap_to_frames->Location = System::Drawing::Point(245, 23);
	m_snap_to_frames->Name = L"m_snap_to_frames";
	m_snap_to_frames->Size = System::Drawing::Size(110, 23);
	m_snap_to_frames->TabIndex = 6;
	m_snap_to_frames->Text = L"Snap to frames";
	m_snap_to_frames->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
	m_snap_to_frames->CheckedChanged += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_snap_to_frames_check_changed);

	// m_camera_follows_npc
	m_camera_follows_npc->Checked = m_editor->camera_follows_npc;
	m_camera_follows_npc->Location = System::Drawing::Point(355, 23);
	m_camera_follows_npc->Name = L"m_camera_follows_npc";
	m_camera_follows_npc->Size = System::Drawing::Size(130, 23);
	m_camera_follows_npc->TabIndex = 6;
	m_camera_follows_npc->Text = L"Camera follows NPC";
	m_camera_follows_npc->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
	m_camera_follows_npc->CheckedChanged += gcnew System::EventHandler(this, &animation_viewer_time_panel::on_camera_follows_npc_check_changed);

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
	m_host->panel->PropertyChanged += gcnew System::ComponentModel::PropertyChangedEventHandler(this, &animation_viewer_time_panel::on_animation_playback_panel_property_changed);

	// form
	AutoScaleDimensions = System::Drawing::SizeF(6, 13);
	AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	AutoScroll = true;
	BackColor = SystemColors::Control;
	ClientSize = System::Drawing::Size(284, 262);
	DockAreas = static_cast<WeifenLuo::WinFormsUI::Docking::DockAreas>(
										WeifenLuo::WinFormsUI::Docking::DockAreas::DockTop | 
										WeifenLuo::WinFormsUI::Docking::DockAreas::DockBottom);
	Font = gcnew System::Drawing::Font(	L"Microsoft Sans Serif", 
										8.25F, 
										System::Drawing::FontStyle::Regular, 
										System::Drawing::GraphicsUnit::Point, 
										static_cast<System::Byte>(204));
	FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
	MaximizeBox = false;
	MinimizeBox = false;
	Name = L"animation_viewer_time_panel";
	Text = L"Time Layout";

	Controls->Add(m_lower_panel);
	Controls->Add(m_upper_panel);
	m_upper_panel->ResumeLayout(false);
	m_lower_panel->ResumeLayout(false);

	set_buttons_enable(false);
	update();
	ResumeLayout(false);
	PerformLayout();
}

animation_viewer_time_panel::~animation_viewer_time_panel()
{
}

viewer_document^ animation_viewer_time_panel::get_active_doc()
{
	if(m_editor->multidocument_base->active_document!=nullptr)
		return safe_cast<viewer_document^>(m_editor->multidocument_base->active_document);

	return nullptr;
}

void animation_viewer_time_panel::set_buttons_enable(bool val)
{
	m_home_button->Enabled = val;
	m_fbwd_button->Enabled = val;
	m_play_button->Enabled = val;
	m_stop_button->Enabled = val;
	m_ffwd_button->Enabled = val;
	m_end_button->Enabled = val;
	m_lock_button->Enabled = val;
}

void animation_viewer_time_panel::on_home_clicked(System::Object^, System::EventArgs^)
{
	viewer_document^ d = get_active_doc();
	if(d==nullptr)
		return;

	d->home();
}

void animation_viewer_time_panel::on_fbwd_clicked(System::Object^, System::EventArgs^)
{
	viewer_document^ d = get_active_doc();
	if(d==nullptr)
		return;

	if(m_editor->snap_to_frames)
	{

		float const koef = animation::default_fps / 1000.0f;
		float const cur_frame = (float)System::Math::Round(d->current_time * koef);
		float const prev_frame = cur_frame - 1.0f;
		d->current_time = prev_frame / koef;
	}
	else
		d->move_backward();
}

void animation_viewer_time_panel::on_play_clicked(System::Object^, System::EventArgs^)
{
	viewer_document^ d = get_active_doc();
	if(d==nullptr)
		return;

	d->play();
}

void animation_viewer_time_panel::on_stop_clicked(System::Object^, System::EventArgs^)
{
	viewer_document^ d = get_active_doc();
	if(d==nullptr)
		return;

	d->stop();
}

void animation_viewer_time_panel::on_ffwd_clicked(System::Object^, System::EventArgs^)
{
	viewer_document^ d = get_active_doc();
	if(d==nullptr)
		return;

	if(m_editor->snap_to_frames)
	{
		float const koef = animation::default_fps / 1000.0f;
		float const cur_frame = (float)System::Math::Round(d->current_time * koef);
		float const next_frame = cur_frame + 1.0f;
		d->current_time = next_frame / koef;
	}
	else
		d->move_forward();
}

void animation_viewer_time_panel::on_end_clicked(System::Object^, System::EventArgs^)
{
	viewer_document^ d = get_active_doc();
	if(d==nullptr)
		return;

	d->end();
}

void animation_viewer_time_panel::on_lock_clicked(System::Object^, System::EventArgs^)
{
	m_editor->change_locked();
}

void animation_viewer_time_panel::on_time_scale_spin_value_changed(System::Object^, System::EventArgs^)
{
	if(disable_events_handling)
		return;

	viewer_document^ d = get_active_doc();
	if(d==nullptr)
		return;

	d->time_scale = (float)m_time_scale_spin->Value;
	update();
}

void animation_viewer_time_panel::on_max_time_s_spin_value_changed(System::Object^, System::EventArgs^)
{
	if(disable_events_handling)
		return;

	viewer_document^ d = get_active_doc();
	if(d==nullptr)
		return;

	d->max_time = (float)m_max_time_s_spin->Value*1000.0f;
	d->reset_target();
	update();
}

void animation_viewer_time_panel::on_max_time_f_spin_value_changed(System::Object^, System::EventArgs^)
{
	if(disable_events_handling)
		return;

	viewer_document^ d = get_active_doc();
	if(d==nullptr)
		return;

	d->max_time = float(m_max_time_f_spin->Value/m_fps_spin->Value)*1000.0f;
	d->reset_target();
	update();
}

void animation_viewer_time_panel::on_cur_time_s_spin_value_changed(System::Object^, System::EventArgs^)
{
	if(disable_events_handling)
		return;

	viewer_document^ d = get_active_doc();
	if(d==nullptr)
		return;

	d->current_time = float(m_cur_time_s_spin->Value)*1000.0f;
	update();
}

void animation_viewer_time_panel::on_cur_time_f_spin_value_changed(System::Object^, System::EventArgs^)
{
	if(disable_events_handling)
		return;

	viewer_document^ d = get_active_doc();
	if(d==nullptr)
		return;

	d->current_time = float(m_cur_time_f_spin->Value/m_fps_spin->Value)*1000.0f;
	update();
}

void animation_viewer_time_panel::set_items(animation_items_list^ lst)
{
	m_host->animation_items = lst;
}

void animation_viewer_time_panel::clear_items()
{
	m_host->animation_items = nullptr;
}

void animation_viewer_time_panel::on_animation_playback_panel_property_changed(System::Object^, System::ComponentModel::PropertyChangedEventArgs^ e)
{
	if(disable_events_handling)
		return;

	viewer_document^ d = get_active_doc();
	if(d==nullptr)
		return;

	if(e->PropertyName=="animation_time")
		d->current_time = m_host->panel->animation_time;
}

void animation_viewer_time_panel::on_play_from_beginning_check_changed(System::Object^ sender, System::EventArgs^)
{
	if(disable_events_handling)
		return;

	bool val = dynamic_cast<CheckBox^>(sender)?dynamic_cast<CheckBox^>(sender)->Checked:dynamic_cast<ToolStripMenuItem^>(sender)->Checked;
	m_editor->play_from_beginning = val;
	update();
}

void animation_viewer_time_panel::on_follow_animation_thumb_check_changed(System::Object^ sender, System::EventArgs^)
{
	if(disable_events_handling)
		return;

	bool val = dynamic_cast<CheckBox^>(sender)?dynamic_cast<CheckBox^>(sender)->Checked:dynamic_cast<ToolStripMenuItem^>(sender)->Checked;
	m_editor->follow_animation_thumb = val;
	update();
}

void animation_viewer_time_panel::on_snap_to_frames_check_changed(System::Object^ sender, System::EventArgs^)
{
	if(disable_events_handling)
		return;

	bool val = dynamic_cast<CheckBox^>(sender)?dynamic_cast<CheckBox^>(sender)->Checked:dynamic_cast<ToolStripMenuItem^>(sender)->Checked;
	m_editor->snap_to_frames = val;
	update();
}

void animation_viewer_time_panel::on_camera_follows_npc_check_changed(System::Object^ sender, System::EventArgs^)
{
	if(disable_events_handling)
		return;

	bool val = dynamic_cast<CheckBox^>(sender)?dynamic_cast<CheckBox^>(sender)->Checked:dynamic_cast<ToolStripMenuItem^>(sender)->Checked;
	m_editor->camera_follows_npc = val;
	update();
}

void animation_viewer_time_panel::update()
{
	disable_events_handling = true;

	m_host->panel->follow_animation_thumb = m_editor->follow_animation_thumb;
	m_host->panel->snap_to_frames = m_editor->snap_to_frames;
	m_play_from_beginning_cb->Checked = m_editor->play_from_beginning;
	m_follow_animation_thumb->Checked = m_editor->follow_animation_thumb;
	m_snap_to_frames->Checked = m_editor->snap_to_frames;
	m_camera_follows_npc->Checked = m_editor->camera_follows_npc;
	viewer_document^ d = get_active_doc();
	if(d==nullptr)
		return;

	m_lock_button->Text = (d->target_locked)?"Unlock":"Lock";
	float doc_max_time = d->max_time;
	float doc_cur_time = d->current_time;
	m_host->panel->max_time = doc_max_time;
	m_host->panel->animation_time = doc_cur_time;
	m_host->panel->editor_is_paused = d->paused;
	(d->paused) ? m_play_button->Text = L"Play" : m_play_button->Text = L"Pause";
	m_time_scale_spin->Value = System::Decimal(d->time_scale);
	m_max_time_f_spin->Value = System::Decimal(doc_max_time/1000.0f*animation::default_fps);
	m_max_time_s_spin->Value = System::Decimal(doc_max_time/1000.0f);
	m_cur_time_f_spin->Value = System::Decimal(doc_cur_time/1000.0f*animation::default_fps);
	m_cur_time_f_spin->Maximum = System::Decimal(doc_max_time/1000.0f*animation::default_fps);
	m_cur_time_s_spin->Value = System::Decimal(doc_cur_time/1000.0f);
	m_cur_time_s_spin->Maximum = System::Decimal(doc_max_time/1000.0f);

	disable_events_handling = false;
}

void animation_viewer_time_panel::update_time()
{
	viewer_document^ d = get_active_doc();
	if(d==nullptr)
		return;

	disable_events_handling = true;

	m_host->panel->editor_is_paused = d->paused;
	(d->paused) ? m_play_button->Text = L"Play" : m_play_button->Text = L"Pause";
	float doc_cur_time = d->current_time;
	m_host->panel->animation_time = doc_cur_time;
	m_cur_time_f_spin->Value = System::Decimal(doc_cur_time/1000.0f*animation::default_fps);
	m_cur_time_s_spin->Value = System::Decimal(doc_cur_time/1000.0f);

	disable_events_handling = false;
}

bool animation_viewer_time_panel::get_snapping()
{
	return m_host->panel->snap_to_frames;
}

bool animation_viewer_time_panel::get_following()
{
	return m_host->panel->follow_animation_thumb;
}

float animation_viewer_time_panel::get_time_layout_scale()
{
	return m_host->panel->time_layout_scale;
}

void animation_viewer_time_panel::set_time_layout_scale(float val)
{
	m_host->panel->time_layout_scale = val;
}
