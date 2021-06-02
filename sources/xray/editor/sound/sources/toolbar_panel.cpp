////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "toolbar_panel.h"
#include "sound_document_editor_base.h"
#include "sound_editor.h"
#include "graph_view.h"
#include "single_sound_wrapper.h"

using xray::sound_editor::toolbar_panel;
using xray::sound_editor::sound_editor;
using xray::sound_editor::single_sound_wrapper;

toolbar_panel::toolbar_panel(sound_editor^ ed)
:super(ed->multidocument_base),
m_parent(ed)
{
	m_control_panel = gcnew Panel();
	m_button_stop = gcnew Button();
	m_button_play = gcnew Button();
	m_button_pause = gcnew Button();
	m_rms_graph_panel = gcnew Panel();
	m_rms_graph = gcnew graph_view(10);
	m_rms_scroll = gcnew scalable_scroll_bar();

	m_rms_graph_panel->SuspendLayout();
	m_control_panel->SuspendLayout();
	SuspendLayout();

	// m_rms_graph_panel
	m_rms_graph_panel->Dock = DockStyle::Fill;
	m_rms_graph_panel->Controls->Add(m_rms_graph);
	m_rms_graph_panel->Controls->Add(m_rms_scroll);

	// m_rms_graph
	m_rms_graph->Dock = DockStyle::Fill;

	// m_rms_scroll
	m_rms_scroll->Height = 22;
	m_rms_scroll->Dock = DockStyle::Bottom;
	m_rms_scroll->scroll_max = m_rms_graph->max_graph_x;
	m_rms_scroll->scroll_min = m_rms_graph->min_graph_x;
	m_rms_scroll->scroller_position = m_rms_graph->min_graph_x;
	m_rms_scroll->scroller_width = m_rms_scroll->scroll_max-m_rms_scroll->scroll_min;
	m_rms_scroll->scroller_moved += gcnew EventHandler<scroller_event_args^>(this, &toolbar_panel::rms_scroll_changed);
	m_rms_scroll->on_draw_background += gcnew PaintEventHandler(this, &toolbar_panel::rms_scroll_draw_background);

	// m_control_panel
	m_control_panel->Controls->Add(m_button_stop);
	m_control_panel->Controls->Add(m_button_pause);
	m_control_panel->Controls->Add(m_button_play);
	m_control_panel->Dock = DockStyle::Top;
	m_control_panel->Location = System::Drawing::Point(0, 0);
	m_control_panel->Name = L"m_control_panel";
	m_control_panel->Size = System::Drawing::Size(284, 23);
	m_control_panel->TabIndex = 0;

	// m_button_play
	m_button_play->Dock = DockStyle::Left;
	m_button_play->BackColor = System::Drawing::SystemColors::Control;
	m_button_play->Location = System::Drawing::Point(0, 0);
	m_button_play->Name = L"m_button_play";
	m_button_play->Size = System::Drawing::Size(50, 23);
	m_button_play->TabIndex = 0;
	m_button_play->Text = L"Play";
	m_button_play->Click += gcnew System::EventHandler(this, &toolbar_panel::on_play_click);

	// m_button_pause
	m_button_pause->Dock = DockStyle::Left;
	m_button_pause->BackColor = System::Drawing::SystemColors::Control;
	m_button_pause->Location = System::Drawing::Point(50, 0);
	m_button_pause->Name = L"m_button_pause";
	m_button_pause->Size = System::Drawing::Size(50, 23);
	m_button_pause->TabIndex = 1;
	m_button_pause->Text = L"Pause";
	m_button_pause->Click += gcnew System::EventHandler(this, &toolbar_panel::on_pause_click);

	// m_button_stop
	m_button_stop->Dock = DockStyle::Left;
	m_button_stop->BackColor = System::Drawing::SystemColors::Control;
	m_button_stop->Location = System::Drawing::Point(50, 0);
	m_button_stop->Name = L"m_button_stop";
	m_button_stop->Size = System::Drawing::Size(50, 23);
	m_button_stop->TabIndex = 1;
	m_button_stop->Text = L"Stop";
	m_button_stop->Click += gcnew System::EventHandler(this, &toolbar_panel::on_stop_click);

	// toolbar_panel
	Controls->Add(m_rms_graph_panel);
	Controls->Add(m_control_panel);
	BackColor = System::Drawing::SystemColors::ControlDark;
	Name = L"toolbar_panel";
	Text = "Toolbar";
	HideOnClose = true;
	m_rms_graph_panel->ResumeLayout(false);
	m_control_panel->ResumeLayout(false);
	enable_buttons(false);
	ResumeLayout(false);
}

toolbar_panel::~toolbar_panel()
{
	if(components)
		delete components;

	delete m_rms_scroll;
	delete m_rms_graph;
	delete m_rms_graph_panel;
}

void toolbar_panel::on_play_click(Object^, EventArgs^) 
{
	m_parent->play_selected();
}

void toolbar_panel::on_stop_click(Object^, EventArgs^) 
{
	m_parent->stop_selected();
}

void toolbar_panel::on_pause_click(Object^, EventArgs^) 
{
	m_parent->pause_selected();
}

void toolbar_panel::rms_scroll_changed(Object^, xray::editor::controls::scroller_event_args^)
{
	m_rms_graph->min_view_x = m_rms_scroll->scroller_position;
	m_rms_graph->max_view_x = m_rms_scroll->scroller_position + m_rms_scroll->scroller_width;
}

void toolbar_panel::rms_scroll_draw_background(Object^, PaintEventArgs^ e)
{
	m_rms_graph->draw_with_graphics(e->Graphics, false, false);
}

void toolbar_panel::show_rms(Object^ obj)
{
	m_rms_graph->clear();
	single_sound_wrapper^ s = dynamic_cast<single_sound_wrapper^>(obj);
	array<sound_object_wrapper^>^ col = dynamic_cast<array<sound_object_wrapper^>^>(obj);
	if(!s && !col)
		return;

	if(!col)
		col = gcnew array<sound_object_wrapper^>(1);

	if(s)
		col[0] = s;

	for each(sound_object_wrapper^ o in col)
	{
		single_sound_wrapper^ snd = dynamic_cast<single_sound_wrapper^>(o);
		if(!snd || !snd->is_loaded)
			continue;

		sound::sound_rms_ptr rms = snd->get_sound_rms();
		sound::sound_rms_pinned pinned_rms(rms);
		u32 size = pinned_rms->size();
		array<float>^ rms_data = gcnew array<float>(size);
		for(u32 i=0; i<size; ++i)
			rms_data[i] = pinned_rms->get_rms_by_index(i);

		m_rms_graph->add_graph_points(rms_data);
		m_rms_scroll->set_scrollbar(
			m_rms_graph->min_graph_x,
			m_rms_graph->max_graph_x,
			m_rms_graph->min_graph_x,
			m_rms_graph->max_graph_x-m_rms_graph->min_graph_x
		);
	}
}

void toolbar_panel::enable_buttons(bool enable)
{
	m_button_stop->Enabled = enable;
	m_button_play->Enabled = enable;
	m_button_pause->Enabled = enable;
}