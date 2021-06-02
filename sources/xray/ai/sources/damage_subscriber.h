////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DAMAGE_SUBSCRIBER_H_INCLUDED
#define DAMAGE_SUBSCRIBER_H_INCLUDED

namespace xray {
namespace ai {

struct sensed_hit_object;

namespace sensors {

struct damage_subscriber : private boost::noncopyable
{
	typedef boost::function< void ( sensed_hit_object const& ) >	subscription_callback;

	inline damage_subscriber		( subscription_callback const& damage_callback )
		:	m_subscription_callback	( damage_callback ),
			m_next					( 0 )
	{
	}

	subscription_callback			m_subscription_callback;
	damage_subscriber*				m_next;
}; // struct damage_subscriber

} // namespace sensors
} // namespace ai
} // namespace xray

#endif // #ifndef DAMAGE_SUBSCRIBER_H_INCLUDED