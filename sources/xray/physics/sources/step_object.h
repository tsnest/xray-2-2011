////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.02.2008
//	Author		: Konstantin Slipchenko
//	Description : step_object
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_STEP_OBJECT_H_INCLUDED
#define XRAY_PHYSICS_STEP_OBJECT_H_INCLUDED


class scheduler;

class step_object
{

public:
	virtual bool	step				( scheduler& w, float time_delta ) = 0;
protected:
					~step_object		( );
					step_object			( ) {};
}; // class step_object

#endif