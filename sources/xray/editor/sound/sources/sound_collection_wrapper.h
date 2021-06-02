////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_COLLECTION_WRAPPER_H_INCLUDED
#define SOUND_COLLECTION_WRAPPER_H_INCLUDED

#include "sound_object_wrapper.h"

using namespace System;
using namespace System::ComponentModel;
using namespace xray::sound;

namespace xray {
namespace sound_editor {

	ref class sound_object_wrapper;
	ref class sound_collection_item;
	public enum class collection_play_type
	{
		random = 1,
		sequential = 2,
	}; // enum class collection_play_type

	public ref class sound_collection_wrapper: public sound_object_wrapper
	{
		typedef System::Collections::Generic::List<sound_collection_item^> items_list;
	public:
							sound_collection_wrapper	(String^ name);
							~sound_collection_wrapper	();
		virtual void		apply_changes				(bool load_resources) override;
		virtual void		revert_changes				() override;
		virtual void		save						() override;
		virtual void		save						(xray::configs::lua_config_value& cfg) override;
		virtual void		load						(Action<sound_object_wrapper^>^ options_callback, Action<sound_object_wrapper^>^ sound_callback) override;
		virtual void		load						(xray::configs::lua_config_value const& cfg) override;
		virtual items_list^	items						() override {return m_items;};
		virtual void		add_item					(sound_collection_item^ itm) override;
		virtual void		remove_item					(sound_collection_item^ itm) override;

		[CategoryAttribute("Sound collection"), DisplayNameAttribute("play_type"), DescriptionAttribute("collection playing type")]
		property collection_play_type play_type;

		[CategoryAttribute("Sound collection"), DisplayNameAttribute("can_repeat_last"), DescriptionAttribute("collection can repeat last played sound")]
		property bool can_repeat_last;

		[CategoryAttribute("Sound collection"), DisplayNameAttribute("cycle_from"), DescriptionAttribute("collection cycle from sound")]
		property u32 cycle_from;

		[CategoryAttribute("Sound collection"), DisplayNameAttribute("min_delay"), DescriptionAttribute("min sound delay value")]
		property float min_delay;

		[CategoryAttribute("Sound collection"), DisplayNameAttribute("max_delay"), DescriptionAttribute("max sound delay value")]
		property float max_delay;

		EventHandler^	on_loaded;

	private:
				void		on_sound_loaded				(xray::resources::queries_result& data);
				void		on_options_loaded			(xray::resources::queries_result& data);

	private:
		items_list^		m_items;
	}; // class sound_collection_wrapper
} // namespace sound_editor 
} // namespace xray
#endif // #ifndef SOUND_COLLECTION_WRAPPER_H_INCLUDED