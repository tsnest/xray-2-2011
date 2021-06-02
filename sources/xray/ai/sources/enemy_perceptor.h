////////////////////////////////////////////////////////////////////////////
//	Created		: 04.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ENEMY_PERCEPTOR_H_INCLUDED
#define ENEMY_PERCEPTOR_H_INCLUDED

#include "perceptor_base.h"
#include "sensors_subscriber.h"

namespace xray {
namespace ai {
namespace perceptors {

class enemy_perceptor : public perceptor_base
{
public:
							enemy_perceptor		(
								npc& npc,
								brain_unit& brain,
								working_memory& memory
							);
	virtual					~enemy_perceptor	( );
	
	virtual	void	on_sensed_object_retrieval	( sensors::sensed_object const& memory_object );

private:
	inline enemy_perceptor* return_this			( );

private:
	sensors_subscriber		m_subscription;
}; // class enemy_perceptor

} // namespace perceptors
} // namespace ai
} // namespace xray

#endif // #ifndef ENEMY_PERCEPTOR_H_INCLUDED