////////////////////////////////////////////////////////////////////////////
//	Created		: 06.09.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "viewer_editor.h"
#include "animation_editor.h"
#include "animation_editor_form.h"
#include "viewer_document.h"
#include "animation_viewer_graph_node_style.h"
#include "animation_viewer_properties_panel.h"
#include "animation_viewer_toolbar_panel.h"
#include "animation_viewer_time_panel.h"
#include "animation_viewer_detailed_options_panel.h"
#include "animation_lexeme_editor.h"
#include "animation_node_interval.h"
#include <xray/animation/api.h>

using xray::animation_editor::viewer_editor;
using xray::animation_editor::animation_editor;
using namespace xray::editor::wpf_controls::animation_playback;

viewer_editor::viewer_editor(animation_editor^ ed) :
	m_animation_editor	( ed ),
	m_scene				( NEW( render::scene_ptr) )
{
	detalization_delta = 33;
	play_from_beginning = true;
	snap_to_frames = false;
	follow_animation_thumb = true;
	show_detailed_weights = true;
	show_detailed_scales = true;
	m_camera_follows_npc = true;
	m_use_detailed_weights_and_scales = false;
	init_controls();
	load_settings();

//	m_multidocument_base->new_document ( );
}

System::String^ viewer_editor::source_path::get( )
{
	return animation_editor::animation_viewer_scenes_path;
}

void viewer_editor::clear_resources()
{
	delete m_detailed_options_panel;
	delete m_lexeme_editor;
	delete m_time_panel;
	delete m_properties_panel;
	delete m_toolbar_panel;
	delete m_multidocument_base;
	delete m_node_style_mgr;

	DELETE	( m_scene );
}

void viewer_editor::on_scene_created( xray::render::scene_ptr const& scene )
{
	*m_scene				= scene;

	u32 const buffer_size	= 256;
	pstr const buffer		= static_cast<pstr>(ALLOCA(buffer_size*sizeof(char)));
	buffer_string str		( buffer, buffer_size );
	if ( s_animation_editor_scene_key.is_set_as_string(&str) )
	{
		System::String^ path = gcnew System::String(str.c_str());
		m_multidocument_base->load_document(path);
	}
}

void viewer_editor::tick()
{
	if(m_multidocument_base->active_document!=nullptr)
		safe_cast<viewer_document^>(m_multidocument_base->active_document)->tick();
}

void viewer_editor::camera_follows_npc::set(bool new_val)
{
	m_camera_follows_npc = new_val;
	if(m_multidocument_base->active_document!=nullptr)
		safe_cast<viewer_document^>(m_multidocument_base->active_document)->set_camera_follower(new_val);
}

void viewer_editor::use_detailed_weights_and_scales::set(bool new_val)
{
	m_use_detailed_weights_and_scales = new_val;
	if(m_multidocument_base->active_document!=nullptr)
		safe_cast<viewer_document^>(m_multidocument_base->active_document)->reset_target();
}

void viewer_editor::add_models_to_render()
{
	if(m_multidocument_base->active_document!=nullptr && m_animation_editor->form->active_content==this)
		safe_cast<viewer_document^>(m_multidocument_base->active_document)->add_models_to_render();
}

void viewer_editor::remove_models_from_render()
{
	if(m_multidocument_base->active_document!=nullptr && m_animation_editor->form->active_content==this)
		safe_cast<viewer_document^>(m_multidocument_base->active_document)->remove_models_from_render();
}

void viewer_editor::on_add_model_click(System::Object^, System::EventArgs^)
{
	System::String^ selected = nullptr;
	bool result = editor_base::resource_chooser::choose("solid_visual", nullptr, nullptr, selected, true);
	if(result)
	{
		if(m_multidocument_base->active_document!=nullptr)
			safe_cast<viewer_document^>(m_multidocument_base->active_document)->add_model(selected);
	}
}

void viewer_editor::on_remove_model_click(System::Object^, System::EventArgs^)
{
	if(m_multidocument_base->active_document!=nullptr)
		safe_cast<viewer_document^>(m_multidocument_base->active_document)->remove_model();
}

void viewer_editor::on_select_model_click(System::Object^, System::EventArgs^)
{
	if(m_multidocument_base->active_document!=nullptr)
		safe_cast<viewer_document^>(m_multidocument_base->active_document)->select_active_model();
}
