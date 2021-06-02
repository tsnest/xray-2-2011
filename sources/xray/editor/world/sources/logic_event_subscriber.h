////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGIC_EVENT_SUBSCRIBER_H_INCLUDED
#define LOGIC_EVENT_SUBSCRIBER_H_INCLUDED

namespace xray {
	namespace editor {

ref class logic_event;

public interface class logic_event_subscriber {
public:
	void on_subscribed_logic_event_deletion		( logic_event^ event );
	void on_subscribed_logic_event_name_changed	( logic_event^ event );

}; // class logic_event_subscriber

} // namespace editor
} // namespace xray

#endif // #ifndef LOGIC_EVENT_SUBSCRIBER_H_INCLUDED