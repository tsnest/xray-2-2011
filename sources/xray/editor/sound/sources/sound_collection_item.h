////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_COLLECTION_ITEM_H_INCLUDED
#define SOUND_COLLECTION_ITEM_H_INCLUDED

#include "sound_editor.h"

using namespace System;
using namespace System::ComponentModel;

namespace xray {
namespace sound_editor {

	ref class sound_object_wrapper;

	public ref class sound_collection_item
	{
	public:
						sound_collection_item		();
						~sound_collection_item		();
		virtual void	save						(xray::configs::lua_config_value& cfg);
		virtual void	load						(xray::configs::lua_config_value const& cfg);
		virtual String^ ToString					() override;

		[CategoryAttribute("Sound collection"), DisplayNameAttribute("is_link"), DescriptionAttribute("sound object as link"), ReadOnlyAttribute(true)]
		property bool is_link;

		[CategoryAttribute("Sound collection"), DisplayNameAttribute("sound_type"), DescriptionAttribute("item sound type"), ReadOnlyAttribute(true)]
		property sound_object_type sound_type;

		[CategoryAttribute("Sound collection"), DisplayNameAttribute("sound_object"), DescriptionAttribute("sound object"), ReadOnlyAttribute(true)]
		property sound_object_wrapper^ sound_object
		{
			sound_object_wrapper^ get() {return m_sound_object;};
			void set(sound_object_wrapper^ val) {m_sound_object = val;};
		};

		[BrowsableAttribute(false)]
		sound_object_wrapper^ parent;

	private:
				void	on_sound_loaded				(sound_object_wrapper^ obj);

	private:
		sound_object_wrapper^ m_sound_object;
	}; // class sound_collection_item
} // namespace sound_editor 
} // namespace xray
#endif // #ifndef SOUND_COLLECTION_ITEM_H_INCLUDED