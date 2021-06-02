////////////////////////////////////////////////////////////////////////////
//	Created		: 04.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SENSORS_SUBSCRIBER_H_INCLUDED
#define SENSORS_SUBSCRIBER_H_INCLUDED

namespace xray {
namespace ai {

namespace sensors {
	struct sensed_object;
} // namespace sensors

namespace perceptors {

struct sensors_subscriber : private boost::noncopyable
{
	typedef boost::function< void ( sensors::sensed_object const& ) >	subscription_callback;

	inline sensors_subscriber		( subscription_callback const& enemy_callback )
		:	m_subscription_callback	( enemy_callback ),
			m_next					( 0 )
	{
	}

	subscription_callback			m_subscription_callback;
	sensors_subscriber*				m_next;
}; // struct sensors_subscriber

} // namespace perceptors
} // namespace ai
} // namespace xray

#endif // #ifndef SENSORS_SUBSCRIBER_H_INCLUDED