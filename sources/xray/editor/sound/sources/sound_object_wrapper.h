////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_OBJECT_WRAPPER_H_INCLUDED
#define SOUND_OBJECT_WRAPPER_H_INCLUDED

#include <xray/sound/sound_emitter.h>

using namespace System;
using namespace System::ComponentModel;
using namespace xray::sound;

namespace xray {
namespace sound_editor {

	ref class sound_object_instance;
	ref class sound_collection_item;

	public ref class sound_object_wrapper abstract
	{
		typedef System::Collections::Generic::List<sound_collection_item^> items_list;
	public:
									sound_object_wrapper	(String^ name);
									~sound_object_wrapper	();
		sound_object_instance^		create_instance			();
		sound_instance_proxy_ptr	emit					(sound_scene_ptr& scene, world_user& user) {return (*m_sound)->emit(scene, user);};
		virtual void				apply_changes			(bool load_resources) = 0;
		virtual void				revert_changes			() = 0;
		virtual void				save					() = 0;
		virtual void				save					(xray::configs::lua_config_value& cfg) = 0;
		virtual void				load					(Action<sound_object_wrapper^>^ options_callback, Action<sound_object_wrapper^>^ sound_callback) = 0;
		virtual void				load					(xray::configs::lua_config_value const& cfg) = 0;
		virtual items_list^			items					() = 0;
		virtual void				add_item				(sound_collection_item^ itm) = 0;
		virtual void				remove_item				(sound_collection_item^ itm) = 0;

		[CategoryAttribute("Sound object"), DisplayNameAttribute("name"), DescriptionAttribute("name")]
		property String^ name
		{
			String^ get() {return m_name;};
		}

		[CategoryAttribute("Sound object"), DisplayNameAttribute("is_loaded"), DescriptionAttribute("is sound loaded")]
		property Boolean is_loaded
		{
			Boolean get() {return m_is_loaded;};
		}

		property Boolean is_exist
		{
			Boolean get() {return m_is_exist;}
		}

	protected:
		sound::sound_emitter_ptr*		m_sound;
		Action<sound_object_wrapper^>^	m_sound_loaded_callback;
		Action<sound_object_wrapper^>^	m_options_loaded_callback;
		String^							m_name;
		Boolean							m_is_loaded;
		Boolean							m_is_exist;
	}; // class sound_object_wrapper
} // namespace sound_editor 
} // namespace xray
#endif // #ifndef SOUND_OBJECT_WRAPPER_H_INCLUDED