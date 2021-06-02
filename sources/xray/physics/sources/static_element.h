////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.02.2008
//	Author		: Konstantin Slipchenko
//	Description : element - rigid physics bone
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_STATIC_ELEMENT_H_INCLUDED
#define XRAY_PHYSICS_STATIC_ELEMENT_H_INCLUDED

#include "collide_element.h"

class static_element:
	public	collide_element
{
	
private:
	virtual	bool	collide			( island& i, const collide_element_interface& o, bool reverted_order )const;	
	virtual	bool	collide			( island& i, const xray::collision::geometry_instance& og, body& body )const;
	virtual void	on_step_start	( island& , float  ){  ASSERT( false, "statics does not steps" ); };

private:
	virtual	bool	collide			( island& i, const xray::collision::geometry_instance &gi, bool reverted_order )const;
	virtual	bool	integrate		( const step_counter_ref& sr, float time_delta );

	virtual	void	interpolate_world_transform	( float, float4x4 & ) const {  ASSERT( false, "statics does not return positions" ); }

private:
	virtual	void	fill_island		( island & )			{ UNREACHABLE_CODE(); }
	virtual	body	*get_body		( )						{ return 0; }

public:
	static_element		( xray::collision::geometry_instance& cf ):	collide_element( cf )		{;}

}; // class static_element

#endif