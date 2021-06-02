////////////////////////////////////////////////////////////////////////////
//	Created		: 15.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_lexeme_editor.h"
#include "viewer_editor.h"
#include "animation_editor.h"
#include "animation_viewer_graph_node_animation.h"
#include "animations_view_panel.h"
#include "animation_node_clip_instance.h"
#include "animation_node_clip.h"
#include "animation_node_interval.h"
#include <xray/animation/mixing_animation_lexeme.h>

using xray::animation_editor::animation_lexeme_editor;
using xray::animation_editor::viewer_editor;
using xray::animation_editor::animation_viewer_graph_node_base;
using xray::animation_editor::animation_viewer_graph_node_animation;
using xray::editor::wpf_controls::animation_setup::animation_channel_partition_type;

namespace xray {
namespace animation_editor {

animation_lexeme_editor::animation_lexeme_editor(viewer_editor^ ed)
{
	m_editor = ed;
	m_lexeme_modified = false;
	m_dont_handle_events = false;
	m_presaved_mode = false;
	m_request = gcnew intervals_request_type();

	InitializeComponent();

	m_animation_lexeme_panel = gcnew DockContent( );
	m_animation_lexeme_panel->DockAreas = WeifenLuo::WinFormsUI::Docking::DockAreas::Document;
	m_lexeme_panel_host = gcnew animation_lexeme_panel_host();
	m_lexeme_panel_host->Dock = DockStyle::Fill;
	m_lexeme_panel_host->TabIndex = 0;
	m_lexeme_panel_host->panel->lexeme_modified += gcnew System::EventHandler(this, &animation_lexeme_editor::on_lexeme_modified);
	m_animation_lexeme_panel->Controls->Add(m_lexeme_panel_host);
	m_layout_panel = gcnew animations_view_panel(this);

	//Load panels settings
	if( !xray::editor::controls::serializer::deserialize_dock_panel_root( this, m_main_dock_panel, "animation_lexeme_editor",
		gcnew DeserializeDockContent( this, &animation_lexeme_editor::find_dock_content ), false ) )
	{
		m_layout_panel->Show			( m_main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockLeft );
		m_animation_lexeme_panel->Show	( m_main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::Document );
	}
}

animation_lexeme_editor::~animation_lexeme_editor()
{
	if(components)
		delete components;
}

IDockContent^ animation_lexeme_editor::find_dock_content	( String^ persist_string )
{
	if( persist_string == "xray.animation_editor.animations_view_panel" )
		return m_layout_panel;

	if( persist_string == "WeifenLuo.WinFormsUI.Docking.DockContent" )
		return m_animation_lexeme_panel;

	return ( nullptr );
}

void animation_lexeme_editor::show(WeifenLuo::WinFormsUI::Docking::DockPanel^ form, animation_viewer_graph_node_base^ n)
{
	m_dont_handle_events = true;
	m_node = safe_cast<animation_viewer_graph_node_animation^>(n);
	Dictionary<String^,animation_item^>^ clip_names_lst = gcnew Dictionary<String^,animation_item^>();
	for(int i=0; i<m_node->intervals->Count; ++i)
	{
		animation_node_interval^ interval = m_node->intervals[i];
		System::String^ clip_name = gcnew System::String(interval->parent->name());
		animation_item^ new_item = nullptr;
		if(!clip_names_lst->ContainsKey(clip_name))
		{
			new_item = gcnew animation_item(clip_name, interval->parent->length()*1000.0f);
			new_item->panel = lexeme_panel;
			lexeme_panel->animations->Add(new_item);
			clip_names_lst->Add(clip_name, new_item);
			for each(animation_node_interval^ child_interval in interval->parent->intervals)
			{
				gcnew animation_interval_item(
					new_item, 
					child_interval->offset*1000.0f,
					child_interval->length*1000.0f, 
					UInt32::MaxValue, 
					(animation_channel_partition_type)child_interval->type
				);
			}
		}
		else
			new_item = clip_names_lst[clip_name];

		if(interval->id!=u32(-1))
		{
			if((int)interval->id<new_item->intervals->Count)
				lexeme_panel->lexeme->insert_interval(new_item->intervals[interval->id], i);
		}
		else
		{
			animation_interval_item^ itm = gcnew animation_interval_item(
					new_item, 
					interval->offset*1000.0f,
					interval->length*1000.0f, 
					UInt32::MaxValue, 
					(animation_channel_partition_type)interval->type
				);
			lexeme_panel->lexeme->insert_interval(itm, i);
		}
	}

	lexeme_panel->lexeme->cycle_from_item_index = m_node->cycle_from_interval_id;
	if(m_node->intervals->Count>0)
	{
		System::String^ path = gcnew System::String(m_node->intervals[0]->parent->name());
		m_layout_panel->track_active_item(path);
	}
	else if(m_layout_panel->last_selected_node!=nullptr)
	{
		System::String^ path = m_layout_panel->last_selected_node->FullPath;
		if(path!="animations")
			path = path->Remove(0, 11);

		m_layout_panel->track_active_item(path);
	}

	m_dont_handle_events = false;
	this->ShowDialog(form);
}

void animation_lexeme_editor::on_ok_clicked(System::Object^, System::EventArgs^)
{
	if(m_lexeme_modified)
	{
		if(lexeme_panel->lexeme->intervals->Count==0)
		{
			System::Windows::Forms::MessageBox::Show(
				this, "Lexeme must contains at least one interval!", 
				"Animation lexeme editor", 
				System::Windows::Forms::MessageBoxButtons::OK, 
				System::Windows::Forms::MessageBoxIcon::Error
			);
			return;
		}

		List<animation_node_interval^>^ new_intervals_list = gcnew List<animation_node_interval^>();
		for(int i=0; i<lexeme_panel->lexeme->intervals->Count; ++i)
		{
			animation_interval_item^ interval_item = lexeme_panel->lexeme->intervals[i];
			animation_node_clip^ clip = m_editor->get_animation_editor()->clip_by_name(interval_item->parent->name);
			R_ASSERT(clip!=nullptr);
			int index = interval_item->parent->intervals->IndexOf(interval_item);
			new_intervals_list->Add(clip->intervals[index]);
		}

		m_editor->set_animation_node_new_intervals_list(m_node, new_intervals_list, (u32)lexeme_panel->lexeme->cycle_from_item_index);
	}

	this->Close();
}

void animation_lexeme_editor::on_cancel_clicked(System::Object^, System::EventArgs^)
{
	this->Close();
}

void animation_lexeme_editor::on_form_closing(System::Object^, System::Windows::Forms::FormClosingEventArgs^ e)
{
	xray::editor::controls::serializer::serialize_dock_panel_root( this, m_main_dock_panel, "animation_lexeme_editor" );

	if(e->CloseReason==System::Windows::Forms::CloseReason::UserClosing)
	{
		m_dont_handle_events = true;
		lexeme_panel->lexeme->intervals->Clear();
		lexeme_panel->animations->Clear();
		m_node = nullptr;
		m_lexeme_modified = false;
		m_dont_handle_events = false;
		this->Hide();
	}

	e->Cancel = true;
}

void animation_lexeme_editor::add_animation(System::String^ anim_name, bool presaved)
{
	if(presaved)
	{
		m_presaved_mode = true;
		lexeme_panel->animations->Clear();
		int lexeme_intervals_count = lexeme_panel->lexeme->intervals->Count;
		for(int i=lexeme_intervals_count-1; i>=0; --i)
			lexeme_panel->lexeme->remove_interval(i);

		xray::fs_new::virtual_path_string animation_path;
		animation_path += unmanaged_string( animation_editor::single_animations_path + "/" ).c_str( );
		animation_path += unmanaged_string(anim_name).c_str();
		animation_path += ".anim_clip";

 		query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &animation_lexeme_editor::on_presaved_clip_loaded), g_allocator);
		xray::resources::query_resource(
			animation_path.c_str(),
			xray::resources::lua_config_class,
			boost::bind(&query_result_delegate::callback, q, _1),
			g_allocator);
	}
	else
	{
		m_presaved_mode = false;
		animation_clip_request_callback^ cb = gcnew animation_clip_request_callback(this, &animation_lexeme_editor::on_animation_loaded);
		m_editor->get_animation_editor()->request_animation_clip(anim_name, cb);
	}
}

void animation_lexeme_editor::on_presaved_clip_loaded(xray::resources::queries_result& data)
{
	xray::configs::lua_config_ptr cfg_ptr = static_cast_resource_ptr<xray::configs::lua_config_ptr>(data[0].get_unmanaged_resource());
	xray::configs::lua_config_value cfg = cfg_ptr->get_root()["clip"];
	if(cfg.value_exists("intervals"))
	{
		for(u32 i=0; i<cfg["intervals"].size(); ++i)
		{
			String^ clip_name = gcnew String((pcstr)cfg["intervals"][i]["animation"]);
			if(clip_name->Contains( animation_editor::single_animations_path ))
				clip_name = clip_name->Remove(0, animation_editor::single_animations_path->Length + 1 );

			intervals_request rq;
			rq.cfg_interval_id = u32(-1);
			if(cfg["intervals"][i].value_exists("interval_id"))
				rq.cfg_interval_id = (u32)cfg["intervals"][i]["interval_id"];
			else
			{
				rq.offset = (float)cfg["intervals"][i]["offset"];
				rq.length = (float)cfg["intervals"][i]["length"];
			}

			rq.interval_id = i;
			if(!m_request->ContainsKey(clip_name))
				m_request->Add(clip_name, gcnew List<intervals_request>());

			m_request[clip_name]->Add(rq);
		}

		animation_clip_request_callback^ cb = gcnew animation_clip_request_callback(this, &animation_lexeme_editor::on_animation_loaded);
		intervals_request_type^ r = gcnew intervals_request_type(m_request);
		for each(KeyValuePair<String^, List<intervals_request>^> p in r)
			m_editor->get_animation_editor()->request_animation_clip(p.Key, cb);
	}
}

void animation_lexeme_editor::on_animation_loaded(xray::animation_editor::animation_node_clip_instance^ new_clip)
{
	System::String^ clip_name = gcnew System::String(new_clip->name());
	if(m_presaved_mode)
	{
		animation_item^ new_item = nullptr;
		for each(animation_item^ itm in lexeme_panel->animations)
		{
			if(itm->name==clip_name)
			{
				new_item = itm;
				break;
			}
		}

		if(new_item==nullptr)
		{
			new_item = gcnew animation_item(clip_name, new_clip->base->length()*1000.0f);
			new_item->panel = lexeme_panel;
			lexeme_panel->animations->Add(new_item);
			for each(animation_node_interval^ child_interval in new_clip->intervals)
			{
				gcnew animation_interval_item(
					new_item, 
					child_interval->offset*1000.0f,
					child_interval->length*1000.0f, 
					UInt32::MaxValue, 
					(animation_channel_partition_type)child_interval->type
				);
			}
		}

		for each(intervals_request p in m_request[clip_name])
		{
			animation_node_interval^ interval = nullptr;
			if(p.cfg_interval_id==u32(-1))
				interval = new_clip->get_interval_by_offset_and_length(p.offset, p.length);
			else
			{
				R_ASSERT((u32)new_clip->intervals->Count>p.cfg_interval_id);
				interval = new_clip->intervals[p.cfg_interval_id];
			}

			u32 index = lexeme_panel->lexeme->intervals->Count;
			if(p.interval_id<index)
				index = p.interval_id;

			if(interval->id!=u32(-1))
				lexeme_panel->lexeme->insert_interval(new_item->intervals[interval->id], index);
			else
			{
				animation_interval_item^ itm = gcnew animation_interval_item(
						new_item, 
						interval->offset*1000.0f,
						interval->length*1000.0f, 
						UInt32::MaxValue, 
						(animation_channel_partition_type)interval->type
					);
				lexeme_panel->lexeme->insert_interval(itm, index);
			}
		}

		m_request->Remove(clip_name);
		if(m_request->Count==0)
			m_presaved_mode = false;
	}
	else
	{
		for each(animation_item^ item in lexeme_panel->animations)
		{
			if(item->name==clip_name)
				return;
		}
		
		bool lexeme_has_intervals = lexeme_panel->lexeme->intervals->Count>0;
		animation_item^ new_item = gcnew animation_item(clip_name, new_clip->animation_length()*1000.0f);
		new_item->panel = lexeme_panel;
		for(int i=0; i<new_clip->intervals->Count; ++i)
		{
			animation_node_interval^ node_interval = new_clip->intervals[i];
			animation_interval_item^ interval_item = gcnew animation_interval_item(
				new_item, 
				node_interval->offset*1000.0f, 
				node_interval->length*1000.0f, 
				UInt32::MaxValue, 
				(animation_channel_partition_type)node_interval->type
			);
			if(!lexeme_has_intervals)
				lexeme_panel->lexeme->insert_interval(interval_item, i);
		}

		lexeme_panel->animations->Add(new_item);
	}
}

void animation_lexeme_editor::on_lexeme_modified(System::Object^, System::EventArgs^)
{
	if(m_dont_handle_events)
		return;

	m_lexeme_modified = true;
}










/*



void animation_viewer_graph_node_animation::on_animation_loaded(xray::animation_editor::animation_node_clip_instance^ new_clip)
{
	if(new_clip==nullptr)
		return;

	m_clips->Add(new_clip);
	String^ clip_name = gcnew String(new_clip->name());
	R_ASSERT(m_request->ContainsKey(clip_name));
	for each(intervals_request p in m_request[clip_name])
	{
		animation_node_interval^ interval = nullptr;
		if(p.cfg_interval_id==u32(-1))
			interval = new_clip->get_interval_by_offset_and_length(p.offset, p.length);
		else
		{
			R_ASSERT((u32)new_clip->intervals->Count>p.cfg_interval_id);
			interval = new_clip->intervals[p.cfg_interval_id];
		}

		if(interval!=nullptr)
		{
			if(p.interval_id>(u32)m_intervals->Count)
				m_intervals->Add(interval);
			else
				m_intervals->Insert(p.interval_id, interval);
		}
	}
	
	m_request->Remove(clip_name);
	Invalidate(false);
}
*/

} // namespace animation_editor
} // namespace xray
