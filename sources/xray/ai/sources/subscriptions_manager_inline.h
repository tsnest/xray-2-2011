////////////////////////////////////////////////////////////////////////////
//	Created		: 05.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SUBSCRIPTIONS_MANAGER_INLINE_H_INCLUDED
#define SUBSCRIPTIONS_MANAGER_INLINE_H_INCLUDED

namespace xray {
namespace ai {

template < typename SubscriberType, typename EventParameterType >
inline void subscriptions_manager< SubscriberType, EventParameterType >::subscribe	( 
		SubscriberType& subscriber
	)
{
	m_subscribers.push_back_unique( &subscriber );
}

template < typename SubscriberType, typename EventParameterType >
inline void subscriptions_manager< SubscriberType, EventParameterType >::unsubscribe(
		SubscriberType& subscriber
	)
{
	m_subscribers.erase( &subscriber );
}

template < typename SubscriberType, typename EventParameterType >
struct subscribers_callback_predicate :
	private boost::noncopyable
{
	subscribers_callback_predicate		( EventParameterType const& parameter )
		:	m_parameter					( parameter )
	{
	}

	inline void operator()				( SubscriberType* subscriber ) const
	{
		subscriber->m_subscription_callback( m_parameter );
	}

	EventParameterType const&			m_parameter;
};

template < typename SubscriberType, typename EventParameterType >
inline void subscriptions_manager< SubscriberType, EventParameterType >::on_event	( 
		EventParameterType const& parameter
	) const
{	
	subscribers_callback_predicate< SubscriberType, EventParameterType > callback_predicate( parameter );
	m_subscribers.for_each( callback_predicate );
}

} // namespace ai
} // namespace xray

#endif // #ifndef SUBSCRIPTIONS_MANAGER_INLINE_H_INCLUDED
