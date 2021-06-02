////////////////////////////////////////////////////////////////////////////
//	Created		: 10.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PICKUP_ITEM_PERCEPTOR_H_INCLUDED
#define PICKUP_ITEM_PERCEPTOR_H_INCLUDED

#include "perceptor_base.h"
#include "sensors_subscriber.h"

namespace xray {
namespace ai {
namespace perceptors {

class pickup_item_perceptor :
	public perceptor_base
{
public:						
						pickup_item_perceptor		(
							npc& npc,
							brain_unit& brain,
							working_memory& memory
						);
	virtual				~pickup_item_perceptor		( );
	
	virtual	void		on_sensed_object_retrieval	( sensors::sensed_object const& memory_object );

private:
	inline pickup_item_perceptor*	return_this		( );

private:
	sensors_subscriber				m_subscription;
}; // class pickup_item_perceptor

} // namespace perceptors
} // namespace ai
} // namespace xray

#endif // #ifndef PICKUP_ITEM_PERCEPTOR_H_INCLUDED