////////////////////////////////////////////////////////////////////////////
//	Created		: 30.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "sound_object_instance.h"
#include "sound_object_wrapper.h"
#include "single_sound_wrapper.h"
#include "composite_sound_wrapper.h"
#include "sound_scene_document.h"
#include "sound_editor.h"
#include "sound_object_collision.h"
#include "wav_file_options.h"
#include <xray/render/facade/debug_renderer.h>
#include <xray/editor/base/collision_object_types.h>
#include <xray/sound/sound_instance_proxy.h>
#include <xray/collision/api.h>

using xray::sound_editor::sound_object_instance;
using xray::sound_editor::sound_object_wrapper;
using xray::sound_editor::sound_scene_document;
using xray::editor::wpf_controls::property_editors::attributes::combo_box_items_attribute;

sound_object_instance::sound_object_instance(sound_object_wrapper^ w)
:m_sound_object(w)
{
	play_looped = false;
	m_collision = nullptr;
	m_transform = NEW(math::float4x4)(math::float4x4().identity());
	m_proxy = NEW(sound::sound_instance_proxy_ptr)();
	id = Guid::NewGuid();
	m_container = gcnew property_container();
	m_container->owner = this;
	xray::editor_base::object_properties::initialize_property_container(this, m_container);
}

sound_object_instance::~sound_object_instance()
{
	if(m_collision->initialized())
		m_collision->destroy(g_allocator);

	(*m_proxy) = NULL;
	DELETE(m_proxy);
	DELETE(m_transform);
	delete m_collision;
}

void sound_object_instance::save(xray::configs::lua_config_value cfg)
{
	unmanaged_string name_str(m_sound_object->name);
	cfg["filename"] = name_str.c_str();
	unmanaged_string id_str(id.ToString());
	cfg["id"] = id_str.c_str();
	cfg["position"] = (*m_transform).c.xyz();
	cfg["rotation"] = (*m_transform).get_angles_xyz();
	cfg["looped"] = play_looped;
	if(dynamic_cast<single_sound_wrapper^>(m_sound_object))
		cfg["resource_type"] = (u32)sound_object_type::single;
	else if(dynamic_cast<composite_sound_wrapper^>(m_sound_object))
		cfg["resource_type"] = (u32)sound_object_type::composite;
	else
		cfg["resource_type"] = (u32)sound_object_type::collection;

}

void sound_object_instance::load(xray::configs::lua_config_value const& cfg)
{
	id = Guid::Guid(gcnew String((pcstr)cfg["id"]));
	play_looped = (bool)cfg["looped"];
	float3 pos = (float3)cfg["position"];
	float3 angles = (float3)cfg["rotation"];
	float4x4 m = create_translation(pos) * create_rotation(angles);
	set_transform(m);
}

void sound_object_instance::set_transform(float4x4 const& transform)
{
	float3 s = transform.get_scale();
	if(s.x<0.05f || s.y<0.05f || s.z<0.05f)
	{
		xray::math::float4x4 m = transform;
		xray::math::clamp(s.x, 0.05f, 1000.0f);
		xray::math::clamp(s.y, 0.05f, 1000.0f);
		xray::math::clamp(s.z, 0.05f, 1000.0f);
		m.set_scale(s);
		*m_transform = m;
	}
	else
		*m_transform = transform;
	
	if(m_collision->initialized())
		m_collision->set_matrix(m_transform);

	if(m_sound_object->is_loaded && m_proxy->c_ptr())
		(*m_proxy)->set_position((*m_transform).c.xyz());
}

void sound_object_instance::set_parent(sound_scene_document^ doc)
{
	m_parent = doc;
	m_collision = gcnew sound_object_collision(m_parent->viewport->m_collision_tree);
	float3 extents(0.3f, 0.3f, 0.3f);
	collision::geometry_instance* geom = &*collision::new_box_geometry_instance(g_allocator, math::create_scale(extents) );
	m_collision->create_from_geometry(true, this, geom, xray::editor_base::collision_object_type_dynamic | xray::editor_base::collision_object_type_touch);
	m_collision->insert();
	m_collision->set_matrix(m_transform);
}

bool sound_object_instance::is_selected()
{
	return m_parent->is_selected(this);
}

void sound_object_instance::render(xray::render::debug::renderer& r, xray::render::scene_ptr const& s)
{
	r.draw_origin(s, *m_transform, 2.0f);
	if(is_selected())
		r.draw_line_ellipsoid(s, *m_transform, color(255, 0, 0));
	else
		r.draw_line_ellipsoid(s, *m_transform, color(0, 255, 0));

	if(!m_proxy || !(*m_proxy).c_ptr())
		return;
}

bool sound_object_instance::is_playing()
{
	return m_proxy->c_ptr() && (*m_proxy)->is_playing();
}

bool sound_object_instance::is_paused()
{
	return m_proxy->c_ptr() && (*m_proxy)->is_producing_paused( );
}

void sound_object_instance::play()
{
//	if(!m_proxy->c_ptr())
	(*m_proxy) = m_sound_object->emit(m_parent->get_sound_scene(), m_parent->get_editor()->get_sound_world()->get_editor_world_user());

	if((*m_proxy)->is_producing_paused())
		return (*m_proxy)->resume();

	if((*m_proxy)->is_playing())
		return;

	if((*m_proxy)->is_playing())
	{
		LOG_ERROR("attempt to play propagating sound!");
		return;
	}

//	(*m_proxy)->play((*m_transform).c.xyz(), (play_looped) ? sound::looped : sound::once);
	(*m_proxy)->play((play_looped) ? sound::looped : sound::once);
}

void sound_object_instance::stop()
{
	if(!m_proxy->c_ptr())
		return;

	(*m_proxy)->stop();
}

void sound_object_instance::pause()
{
	if(!m_proxy->c_ptr())
		return;

	if((*m_proxy)->is_playing())
		(*m_proxy)->pause();
}

void sound_object_instance::stop_now()
{
	if(!m_proxy->c_ptr())
		return;
	
	(*m_proxy)->stop();
}

void sound_object_instance::set_position_revertible(float3 new_val)
{
	float4x4 transform = *m_transform;
	transform.c.xyz() = new_val;
	m_parent->change_object_transform(this, *m_transform, transform);
}

void sound_object_instance::set_rotation_revertible(float3 new_val)
{
	float4x4 transform = *m_transform;
	transform = create_scale(transform.get_scale()) * create_rotation(new_val) * create_translation(transform.c.xyz());
	m_parent->change_object_transform(this, *m_transform, transform);
}
