////////////////////////////////////////////////////////////////////////////
//	Created 	: 05.02.2008
//	Author		: Konstantin Slipchenko
//	Description : dynamic_object
////////////////////////////////////////////////////////////////////////////
#include "pch.h"

#include "dynamic_object.h"
#include "island.h"

bool	dynamic_object::step( scheduler& w, float time_delta )
{
	
	if( w.step_counter().is_equal( m_step_counter ) )	// object is olready steped
		return awake();

	island	island( w, time_delta );
	ASSERT( get_space_item(), "is not in space" );

	island_connect( island );
	
	island.step( );
	move( );
	on_step( w );
	return awake();
}

void	dynamic_object::island_connect( island	&i )
{
	ASSERT( get_space_item(), "is not in space" );

	if( i.step_counter().is_equal( m_step_mark_recursion ) )//prevent recursion, prevent resteping
		return;

	m_step_mark_recursion = i.step_counter();

	i.connect( this );

	m_collision_recursing = true;
	get_space_item()->collide( i ); //recurse ahead
	m_collision_recursing = false;

}

void	dynamic_object::island_step_end( island	&i, float time_delta )
{
	XRAY_UNREFERENCED_PARAMETER(time_delta);
	m_step_counter = i.step_counter();
}

void	dynamic_object::collide( island& i, space_object& o, bool reverted_order )
{
	if( m_collision_recursing || i.step_counter().is_equal( m_step_counter ) )	// not collide objects that already steped
		return;

	if( o.collide_detail( i, *this, !reverted_order ) )
				island_connect( i );
}

void	dynamic_object::wake_up( scheduler &s )
{
	ASSERT( !m_awake );
	s.activate( this );
	m_awake = true;
}


