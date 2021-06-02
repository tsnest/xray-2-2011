////////////////////////////////////////////////////////////////////////////
//	Created 	: 04.02.2008
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SCHEDULER_H_INCLUDED
#define SCHEDULER_H_INCLUDED

#include "step_counter.h"

namespace xray {
namespace physics {
	class	physics_world;
} // namespace physics
} // namespace xray

class	step_object;
extern	float integration_step;

class scheduler:
	private boost::noncopyable
{
	step_counter						m_step_counter;
	const physics::physics_world&		m_world;
	vectora<step_object*>				m_active_objects;
	xray::timing::timer					m_timer;
	float								m_time_left;

public:
								scheduler	( memory::base_allocator& allocator, const physics::physics_world& w	);
		void					activate	( step_object* o );
		bool					deactivate	( step_object* o );
		void					tick		( );

inline	float					interpolation_factor( ){ return m_time_left/integration_step; }

inline	const physics::physics_world& world	( )const	{return m_world ;}
inline	const step_counter_ref	step_counter( )const	{ return step_counter_ref( m_step_counter ); }
private:
	void	step		( float time_delta );
	


}; // class scheduler

#endif // #ifndef PHYSICS_SCHEDULER_H_INCLUDED