////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_ITEM_SELECTION_EVENT_ARGS_H_INCLUDED
#define SOUND_ITEM_SELECTION_EVENT_ARGS_H_INCLUDED

using namespace System;

namespace xray
{
	namespace editor
	{
		ref struct sound_item_struct;

		public ref class sound_item_selection_event_args:EventArgs
		{
		public:
			sound_item_selection_event_args(sound_item_struct^ file_struct)
			{
				m_struct = file_struct;
			}

		public:
			sound_item_struct^ m_struct;

		}; // class sound_item_selection_event_args
	}
}
#endif // #ifndef SOUND_ITEM_SELECTION_EVENT_ARGS_H_INCLUDED