////////////////////////////////////////////////////////////////////////////
//	Created		: 12.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_FILE_SELECTION_EVENT_ARGS_H_INCLUDED
#define SOUND_FILE_SELECTION_EVENT_ARGS_H_INCLUDED

using namespace System;

namespace xray
{
	namespace editor
	{
		ref struct sound_file_struct;

		public ref class sound_file_selection_event_args: EventArgs {
		public:
			sound_file_selection_event_args(sound_file_struct^ file_struct)
			{
				m_struct = file_struct;
			}

		public:
			sound_file_struct^ m_struct;
		}; // class sound_file_selection_event_args
	}//namespace editor
}//namespace xray

#endif // #ifndef SOUND_FILE_SELECTION_EVENT_ARGS_H_INCLUDED