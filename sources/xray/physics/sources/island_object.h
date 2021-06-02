////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.02.2008
//	Author		: Konstantin Slipchenko
//	Description : shell
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_ISLAND_OBJECT_H_INCLUDED
#define XRAY_PHYSICS_ISLAND_OBJECT_H_INCLUDED

#include "integratable.h"

class island_object
{
public:

	virtual	void	on_island_connect	( island	&i )					=0;
	virtual void	island_step_start	( island	&i, float time_delta )	=0;
	virtual void	island_step_end		( island	&i, float time_delta )	=0;
	virtual	void	render				( render::scene_ptr const& scene, render::debug::renderer& renderer ) const =0;

protected:
	island_object	( ){};
	~island_object	( ){};

}; // class island_object

#endif
