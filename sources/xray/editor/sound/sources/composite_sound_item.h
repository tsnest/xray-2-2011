////////////////////////////////////////////////////////////////////////////
//	Created		: 05.08.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef COMPOSITE_SOUND_ITEM_H_INCLUDED
#define COMPOSITE_SOUND_ITEM_H_INCLUDED

#include "sound_collection_item.h"

using namespace System;
using namespace System::ComponentModel;

namespace xray {
namespace sound_editor {
	public ref class composite_sound_item: public sound_collection_item
	{
	public:
						composite_sound_item		();
						~composite_sound_item		();
		virtual void	save						(xray::configs::lua_config_value& cfg) override;
		virtual void	load						(xray::configs::lua_config_value const& cfg) override;

		typedef editor::wpf_controls::property_grid::refresh_grid_on_change_attribute	refresh_grid_on_change_attribute;

		[CategoryAttribute("Composite sound"), DisplayNameAttribute("playing_offset_min"), DescriptionAttribute("sound object as link")]
		[ refresh_grid_on_change_attribute ]
		property u32 playing_offset_min
		{
			u32		get		( )
			{ 
				return m_playing_offset_min; 
			}
			void	set		( u32 val )
			{
				m_playing_offset_min	= val;
				if ( m_playing_offset_min > m_playing_offset_max )
					playing_offset_max	= val;
			}
		}

		[CategoryAttribute("Composite sound"), DisplayNameAttribute("playing_offset_max"), DescriptionAttribute("sound object as link")]
		[ refresh_grid_on_change_attribute ]
		property u32 playing_offset_max
		{
			u32		get		( )
			{ 
				return m_playing_offset_max; 
			}
			void	set		( u32 val )
			{
				m_playing_offset_max	= val;
				if ( m_playing_offset_max < m_playing_offset_min )
					playing_offset_min	= val;
			}
		}

		[CategoryAttribute("Composite sound"), DisplayNameAttribute("offset_type"), DescriptionAttribute("sound object as link"), ReadOnlyAttribute(true)]
		property String^ offset_type
		{
			String^ get() 
			{
				if ( playing_offset_min == playing_offset_max )
					return "caonstant";
				else
					return "random";
			};
		}
		[CategoryAttribute("Composite sound"), DisplayNameAttribute("is_random"), DescriptionAttribute("sound object as link"), ReadOnlyAttribute(true)]
		property bool is_random; 
	private:
		u32		m_playing_offset_min;
		u32		m_playing_offset_max;
	}; // class composite_sound_item
} // namespace sound_editor 
} // namespace xray
#endif // #ifndef COMPOSITE_SOUND_ITEM_H_INCLUDED