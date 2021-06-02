////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.02.2008
//	Author		: Konstantin Slipchenko
//	Description : gen_contact
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_GENERATE_CONTACT_H_INCLUDED
#define XRAY_PHYSICS_GENERATE_CONTACT_H_INCLUDED

#include "generate_contact_base.h"
class body;

struct on_two_body_contact:
	public generate_contact_base
{
	body	&body_0, &body_1	;

public:
	inline	on_two_body_contact(  body	&b_0,  body	&b_1, island &i ): generate_contact_base( i ),
		body_0( b_0 ), body_1( b_1 ) {}

public:
	virtual void	operator()	( const xray::collision::contact_info& c ) ;

}; // struct on_two_body_contact


struct on_one_body_contact:
	public generate_contact_base
{
	body	&b;

public:
	on_one_body_contact(  body	&_b, island	&i ): b(_b),  generate_contact_base( i ){}

public:
	virtual void operator()	( const xray::collision::contact_info& c ) ;
}; // struct on_one_body_contact

#endif
