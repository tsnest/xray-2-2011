////////////////////////////////////////////////////////////////////////////
//	Created		: 14.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_groups_intervals_panel.h"
#include "animation_groups_document.h"
#include "animation_node_clip_instance.h"
#include "animation_node_interval.h"

using xray::animation_editor::animation_groups_intervals_panel;
using xray::animation_editor::animation_groups_document;
using xray::animation_editor::animation_node_clip;
using xray::animation_editor::animation_node_interval;

animation_groups_intervals_panel::animation_groups_intervals_panel(animation_groups_document^ d)
{
	InitializeComponent();
	m_intervals = gcnew intervals_list();
	m_document = d;
	m_node = nullptr;
}

animation_groups_intervals_panel::~animation_groups_intervals_panel()
{
	m_intervals->Clear();
	delete m_intervals;
	if(components)
		delete components;
}

void animation_groups_intervals_panel::show(node^ n)
{
	m_node = n;
	m_intervals->Clear();
	checkedListBox1->Items->Clear();
	progressBar1->Value = 0;
	elements_enabled(false);
	this->Show(m_document);
}

void animation_groups_intervals_panel::elements_enabled(bool val)
{
	progressBar1->Enabled = !val;
	label1->Enabled = !val;
	checkedListBox1->Enabled = val;
	label2->Enabled = val;
	button2->Enabled = val;
}

void animation_groups_intervals_panel::animations_count(u32 val)
{
	progressBar1->Maximum = val*100;
}

void animation_groups_intervals_panel::animation_loaded(xray::animation_editor::animation_node_clip_instance^ new_clip)
{
	for each(animation_node_interval^ item in new_clip->intervals)
	{
		m_intervals->Add(item);
		if(!checkedListBox1->Items->Contains(item->type.ToString()))
			checkedListBox1->Items->Add(item->type.ToString(), false);
	}

	progressBar1->Value += 100;
	if(progressBar1->Value==progressBar1->Maximum)
		elements_enabled(true);
}

void animation_groups_intervals_panel::on_form_closing(System::Object^, System::Windows::Forms::FormClosingEventArgs^ e)
{
	if(e->CloseReason==System::Windows::Forms::CloseReason::UserClosing)
	{
		if(progressBar1->Value!=progressBar1->Maximum)
			m_document->cancel_intervals_adding();

		e->Cancel = true;
		this->Hide();
	}
}

void animation_groups_intervals_panel::on_ok_clicked(System::Object^, System::EventArgs^)
{
	if(checkedListBox1->CheckedItems->Count>0)
	{
		intervals_list^ lst = gcnew intervals_list();
		for each(System::String^ item in checkedListBox1->CheckedItems)
		{
			animation_channel_partition_type t = safe_cast<animation_channel_partition_type>(System::Enum::Parse(animation_channel_partition_type::typeid, item));
			for each(animation_node_interval^ interval in m_intervals)
			{
				if(interval->type==t)
					lst->Add(interval);
			}
		}

		m_document->add_group_intervals(m_node, lst);
	}

	this->Hide();
}

void animation_groups_intervals_panel::on_cancel_clicked(System::Object^, System::EventArgs^)
{
	if(progressBar1->Value!=progressBar1->Maximum)
		m_document->cancel_intervals_adding();

	this->Hide();
}
