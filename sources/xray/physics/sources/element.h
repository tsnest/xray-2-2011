////////////////////////////////////////////////////////////////////////////
//	Created 	: 02.02.2008
//	Author		: Konstantin Slipchenko
//	Description : element - rigid physics bone
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_ELEMENT_H_INCLUDED
#define XRAY_PHYSICS_ELEMENT_H_INCLUDED

#include "collide_element.h"

class body;
class collide_form;
class contact_ifo;
class element:
	public	collide_element,
	private boost::noncopyable
{

private:
	body			&m_body;
	float4x4		m_pose_in_body;
	typedef			collide_element	inherited;

public:
			element( body &b, xray::collision::geometry_instance &cf  );

protected:
			~element( );

public:
	virtual	void	set_mass				( float mass );

private:

	virtual	bool		collide			( island& i, const collide_element_interface& o, bool reverted_order )const;	
	virtual	bool		collide			( island& i, const xray::collision::geometry_instance &og, body& body )const;
	virtual	bool		collide			( island& i, const xray::collision::geometry_instance &og, bool reverted_order )const;
	virtual void		on_step_start	( island	&i, float time_delta );
	virtual	void		render			(  render::scene_ptr const& scene, render::debug::renderer& renderer ) const;
	virtual	bool		integrate		( const step_counter_ref& sr, float time_delta );
	
	virtual	void		fill_island		( island &i );
	
	virtual	body		*get_body		(){ return &m_body; }
	virtual	body const	*get_body		() const { return &m_body; }

	virtual	void		interpolate_world_transform	( float factor, float4x4 &transform ) const;

}; // class element

#endif