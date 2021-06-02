////////////////////////////////////////////////////////////////////////////
//	Created 	: 02.02.2008
//	Author		: Konstantin Slipchenko
//	Description : element collision interface
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_SHELL_ELEMENT_H_INCLUDED
#define XRAY_PHYSICS_SHELL_ELEMENT_H_INCLUDED

#include "collide_element_interface.h"

class island;
class body;
class step_counter_ref;

class shell_element:
	public collide_element_interface
{

public:
	virtual	bool	integrate					( const step_counter_ref& sr, float time_delta )	=0;
	virtual	void	fill_island					( island &i )										=0;
	virtual	body	*get_body					( )													=0;
	virtual void	on_step_start				( island	&i, float time_delta )					=0;
	virtual	void	interpolate_world_transform	( float factor, float4x4 &transform )const			=0;
	virtual			~shell_element				( )													{;}

}; // class shell_element

#endif