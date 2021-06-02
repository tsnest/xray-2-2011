////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SCROLLER_EVENT_ARGS_H_INCLUDED
#define SCROLLER_EVENT_ARGS_H_INCLUDED

using namespace System;

namespace xray
{
	namespace editor
	{
		namespace controls
		{
			public ref class scroller_event_args: public EventArgs
			{
			public:
				scroller_event_args(Single set_last_position, Single set_current_position, Single set_last_width, Single set_current_width):
					last_position		(set_last_position),
					current_position	(set_current_position),
					last_width			(set_last_width),
					current_width		(set_current_width){}

				Single last_position;
				Single current_position;
				Single last_width;
				Single current_width;
			}; // class scroller_event_args
		}//namespace controls
	}//namespace editor
}//namespace xray
#endif // #ifndef SCROLLER_EVENT_ARGS_H_INCLUDED