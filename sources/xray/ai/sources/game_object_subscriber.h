////////////////////////////////////////////////////////////////////////////
//	Created		: 15.12.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_OBJECT_SUBSCRIBER_H_INCLUDED
#define GAME_OBJECT_SUBSCRIBER_H_INCLUDED

namespace xray {
namespace ai {

struct game_object;

struct game_object_subscriber : private boost::noncopyable
{
	typedef boost::function< void ( game_object const& ) >	subscription_callback;

	inline game_object_subscriber			( subscription_callback const& callback )
		:	m_subscription_callback			( callback ),
			m_next							( 0 )
	{
	}

	subscription_callback					m_subscription_callback;
	game_object_subscriber*					m_next;
}; // struct game_object_subscriber

} // namespace ai
} // namespace xray

#endif // #ifndef GAME_OBJECT_SUBSCRIBER_H_INCLUDED