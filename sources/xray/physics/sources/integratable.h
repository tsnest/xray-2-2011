////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.02.2008
//	Author		: Konstantin Slipchenko
//	Description : integration interface
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_INTEGRATABLE_H_INCLUDED
#define XRAY_PHYSICS_INTEGRATABLE_H_INCLUDED

class integratable
{
public:	
	virtual	void force_integrate	( float time_delta ) =0;
	virtual	void position_integrate	( float time_delta ) =0;

protected:
	~integratable	(){}
	integratable	(){}
}; // class integratable

#endif