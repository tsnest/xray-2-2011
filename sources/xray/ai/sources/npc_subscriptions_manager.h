////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef NPC_SUBSCRIPTIONS_MANAGER_H_INCLUDED
#define NPC_SUBSCRIPTIONS_MANAGER_H_INCLUDED

namespace xray {
namespace ai {

struct npc;

template < typename SubscriberType, typename EventParameterType >
class npc_subscriptions_manager
{
public:
	inline	void	subscribe	( npc& npc, SubscriberType& subscriber );
	inline	void	unsubscribe	( npc& npc, SubscriberType& subscriber );
	inline	void	on_event	( npc& npc, EventParameterType const& parameter ) const;
	

private:
	typedef intrusive_list<	SubscriberType,
							SubscriberType*,
							&SubscriberType::m_next >	subscribers_list_type;
	typedef map< npc*, subscribers_list_type >			subscribers_type;

private:
	subscribers_type		m_subscribers;
}; // class npc_subscriptions_manager

} // namespace ai
} // namespace xray

#include "npc_subscriptions_manager_inline.h"

#endif // #ifndef SUBSCRIPTIONS_MANAGER_H_INCLUDED