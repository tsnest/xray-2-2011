////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef COMPOSITE_SOUND_WRAPPER_H_INCLUDED
#define COMPOSITE_SOUND_WRAPPER_H_INCLUDED

#include "sound_object_wrapper.h"
#include "composite_sound_item.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace xray::sound;

namespace xray {
namespace sound_editor {

	ref class sound_object_wrapper;
	ref class composite_sound_item;
	ref class sound_collection_item;

	public ref class composite_sound_wrapper: public sound_object_wrapper
	{
		typedef List<sound_collection_item^> items_list;
	public:
							composite_sound_wrapper		(String^ name);
							~composite_sound_wrapper	();
		virtual void		apply_changes				(bool load_resources) override;
		virtual void		revert_changes				() override;
		virtual void		save						() override;
		virtual void		save						(xray::configs::lua_config_value& cfg) override;
		virtual void		load						(Action<sound_object_wrapper^>^ options_callback, Action<sound_object_wrapper^>^ sound_callback) override;
		virtual void		load						(xray::configs::lua_config_value const& cfg) override;
		virtual items_list^	items						() override;
		virtual void		add_item					(sound_collection_item^ itm) override;
		virtual void		remove_item					(sound_collection_item^ itm) override;

		[CategoryAttribute("Composite sound"), DisplayNameAttribute("after_playing_pause"), DescriptionAttribute("pause after sounds playing")]
		property u32 after_playing_pause;

		[CategoryAttribute("Composite sound"), DisplayNameAttribute("before_playing_pause"), DescriptionAttribute("pause before sounds playing")]
		property u32 before_playing_pause;

		EventHandler^	on_loaded;

	private:
				void		on_sound_loaded				(xray::resources::queries_result& data);
				void		on_options_loaded			(xray::resources::queries_result& data);

	private:
		List<composite_sound_item^>^	m_items;
	}; // class composite_sound_wrapper
} // namespace sound_editor 
} // namespace xray
#endif // #ifndef COMPOSITE_SOUND_WRAPPER_H_INCLUDED