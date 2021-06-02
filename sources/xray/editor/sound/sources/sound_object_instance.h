////////////////////////////////////////////////////////////////////////////
//	Created		: 30.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_OBJECT_INSTANCE_H_INCLUDED
#define SOUND_OBJECT_INSTANCE_H_INCLUDED

#include <xray/sound/sound_instance_proxy.h>

using namespace System;
using namespace System::ComponentModel;
using xray::editor::wpf_controls::property_container;

namespace xray {
namespace render {
namespace debug {
	class renderer;
} // namespace debug
} // namespace render
namespace sound_editor {
	
	ref class sound_object_wrapper;
	ref class sound_scene_document;
	ref struct sound_object_collision;

	public ref class sound_object_instance
	{
		typedef xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute	value_range_and_format_attribute;
	public:
						sound_object_instance	(sound_object_wrapper^ w);
						~sound_object_instance	();
		void			save					(xray::configs::lua_config_value cfg);
		void			load					(xray::configs::lua_config_value const& cfg);
		void			set_transform			(float4x4 const& transform);
		float4x4*		get_transform			() {return m_transform;};
		void			set_parent				(sound_scene_document^ doc);
sound_object_wrapper^	get_sound_object		() {return m_sound_object;};
	property_container^	get_property_container	() {return m_container;};
		void			render					(xray::render::debug::renderer& r, xray::render::scene_ptr const& s);

		bool			is_selected				();
		bool			is_playing				();
		bool			is_paused				();
		void			play					();
		void			stop					();
		void			pause					();
		void			stop_now				();

	[BrowsableAttribute(false)]
	property Guid id;

	[CategoryAttribute("Instance properties"), DisplayNameAttribute("position"), ValueAttribute(ValueAttribute::value_type::e_def_val, 0.0f, 0.0f, 0.0f)]
	property Float3 position
	{
		Float3 get() {return Float3(m_transform->c.xyz());}
		void set(Float3 new_val) {set_position_revertible(float3(new_val));}
	}

	[CategoryAttribute("Instance properties"), DisplayNameAttribute("rotation"), ValueAttribute(ValueAttribute::value_type::e_def_val, 0.0f, 0.0f, 0.0f)]
	property Float3 rotation
	{
		Float3 get() {return Float3(rad2deg(m_transform->get_angles_xyz()));};
		void set(Float3 new_val) {set_rotation_revertible(deg2rad(float3(new_val)));};
	}

	[CategoryAttribute("Instance properties"), DisplayNameAttribute("play_looped")]
	property bool play_looped;

	private:
		void			set_position_revertible	(float3 new_val);
		void			set_rotation_revertible	(float3 new_val);
		float3			rad2deg					(float3 const& rad) {return rad*(180.0f/math::pi);};
		float3			deg2rad					(float3 const& deg) {return deg*(math::pi/180.0f);};

	private:
		sound_object_wrapper^				m_sound_object;
		sound_object_collision^				m_collision;
		sound::sound_instance_proxy_ptr*	m_proxy;
		sound_scene_document^				m_parent;
		float4x4*							m_transform;
		property_container^					m_container;
	}; // class sound_object_instance
}// namespace sound_editor
}// namespace xray
#endif // #ifndef SOUND_OBJECT_INSTANCE_H_INCLUDED