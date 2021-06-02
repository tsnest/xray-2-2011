////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.02.2008
//	Author		: Konstantin Slipchenko
//	Description : joint_ode_base
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_JOINT_ODE_BASE_H_INCLUDED
#define XRAY_PHYSICS_JOINT_ODE_BASE_H_INCLUDED

//struct dxJoint;
//class body;
//class island;
//class joint_ode_base
//{
//
//protected:
//	virtual			dxJoint* dx_joint()					= 0;
//	virtual	const	dxJoint* dx_joint()const			= 0;
//public:
//
//	virtual			~joint_ode_base(){ };
//public:
//	virtual	void	connect		( body	*b0, body	*b1 )	;
//	virtual	void	connect		( body	*b )				;
//	virtual	void	disconect	( )							;
//	virtual	void	integrate	( float	integration_step )	;
//	virtual	void	fill_island ( island	&i )			;
//
//private:
//	void			detach		( )							;
//};

struct dxJoint;
class body;
class island;
void	connect		( dxJoint* dx_joint, body	*b );
void	connect		( dxJoint* dx_joint, body	*b0, body	*b1 );
void	fill_island ( dxJoint* dx_joint, island	&i );
void	detach		( dxJoint* dx_joint );

#endif