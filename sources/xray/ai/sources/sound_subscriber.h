////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_SUBSCRIBER_H_INCLUDED
#define SOUND_SUBSCRIBER_H_INCLUDED

namespace xray {
namespace ai {

struct sensed_sound_object;

namespace sensors {

struct sound_subscriber : private boost::noncopyable
{
	typedef boost::function< void ( sensed_sound_object const& ) >	subscription_callback;

	inline sound_subscriber			( subscription_callback const& sound_callback )
		:	m_subscription_callback	( sound_callback ),
			m_next					( 0 )
	{
	}

	subscription_callback			m_subscription_callback;
	sound_subscriber*				m_next;
}; // struct sound_subscriber

} // namespace sensors
} // namespace ai
} // namespace xray

#endif // #ifndef SOUND_SUBSCRIBER_H_INCLUDED