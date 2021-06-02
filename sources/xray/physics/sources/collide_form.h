////////////////////////////////////////////////////////////////////////////
//	Created 	: 05.02.2008
//	Author		: Konstantin Slipchenko
//	Description : collision interface
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_COLLIDE_FORM_H_INCLUDED
#define XRAY_COLLIDE_FORM_H_INCLUDED
class contact_info_buffer;
class pose_anchor;
class geometry;
class collide_form
{
public:
	virtual	void	collide( contact_info_buffer& , const collide_form& )				const{}	;
	virtual	void	collide( contact_info_buffer& , pose_anchor &, const geometry& )	const{}	;
	virtual	void	move_to( const  pose_anchor & )											{}	;

public:
virtual	~collide_form	()	{}
	collide_form	()	{}
};
#endif
