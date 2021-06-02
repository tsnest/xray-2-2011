////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef NPC_SUBSCRIPTIONS_MANAGER_INLINE_H_INCLUDED
#define NPC_SUBSCRIPTIONS_MANAGER_INLINE_H_INCLUDED

namespace xray {
namespace ai {

template < typename SubscriberType, typename EventParameterType >
inline void npc_subscriptions_manager< SubscriberType, EventParameterType >::subscribe	(
		npc& npc,
		SubscriberType& subscriber
	)
{
	typename subscribers_type::iterator it_subscriber = m_subscribers.find( &npc );
	if ( it_subscriber == m_subscribers.end() )
	{
 		std::pair< typename subscribers_type::iterator, bool > result =
 			m_subscribers.insert( std::make_pair( &npc, subscribers_list_type() ) );
		
		it_subscriber = result.first;
	}
	it_subscriber->second.push_back( &subscriber );
}

template < typename SubscriberType, typename EventParameterType >
inline void npc_subscriptions_manager< SubscriberType, EventParameterType >::unsubscribe(
		npc& npc,
		SubscriberType& subscriber
	)
{
	typename subscribers_type::iterator it_subscriber = m_subscribers.find( &npc );
	R_ASSERT ( it_subscriber != m_subscribers.end() );
	it_subscriber->second.erase( &subscriber );
	
	if ( it_subscriber->second.empty() )
	{
		m_subscribers.erase( it_subscriber );
	}
}

template < typename SubscriberType, typename EventParameterType >
struct npc_subscribers_callback_predicate :
	private boost::noncopyable
{
	npc_subscribers_callback_predicate	( EventParameterType const& parameter )
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
inline void npc_subscriptions_manager< SubscriberType, EventParameterType >::on_event	(
		npc& npc,
		EventParameterType const& parameter
	) const
{
	typename subscribers_type::const_iterator it_subscriber = m_subscribers.find( &npc );
	R_ASSERT ( it_subscriber != m_subscribers.end() );
	
	npc_subscribers_callback_predicate< SubscriberType, EventParameterType > callback_predicate( parameter );
	it_subscriber->second.for_each( callback_predicate );
}

} // namespace ai
} // namespace xray

#endif // #ifndef NPC_SUBSCRIPTIONS_MANAGER_INLINE_H_INCLUDED
