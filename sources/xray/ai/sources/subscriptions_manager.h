////////////////////////////////////////////////////////////////////////////
//	Created		: 05.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SUBSCRIPTIONS_MANAGER_H_INCLUDED
#define SUBSCRIPTIONS_MANAGER_H_INCLUDED

namespace xray {
namespace ai {

template < typename SubscriberType, typename EventParameterType >
class subscriptions_manager
{
public:
	inline	void	subscribe	( SubscriberType& subscriber );
	inline	void	unsubscribe	( SubscriberType& subscriber );
	inline	void	on_event	( EventParameterType const& parameter ) const;
	

private:
	typedef intrusive_list<	SubscriberType,
							SubscriberType*,
							&SubscriberType::m_next >	subscribers_type;

private:
	subscribers_type		m_subscribers;
}; // class subscriptions_manager

} // namespace ai
} // namespace xray

#include "subscriptions_manager_inline.h"

#endif // #ifndef SUBSCRIPTIONS_MANAGER_H_INCLUDED