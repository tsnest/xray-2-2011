////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.02.2008
//	Author		: Konstantin Slipchenko
//	Description : integrator
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_INTEGRATOR_ODE_H_INCLUDED
#define XRAY_PHYSICS_INTEGRATOR_ODE_H_INCLUDED

struct dxJoint;
struct dxBody;

class integrator_ode:
	private boost::noncopyable
{
	vectora< dxJoint* >	&m_joints;
	vectora< dxBody* >	&m_bodies;

public:
inline		 integrator_ode	( vectora< dxJoint* >& joints, vectora< dxBody* >& bodies );
inline	void add_body		( dxBody *b );
inline	void add_joint		( dxJoint *j );
		void step			( float time_delta );

}; // class integrator_ode


inline	integrator_ode::integrator_ode( vectora< dxJoint* >& joints, vectora< dxBody* >& bodies ):
	m_joints( joints ),
	m_bodies( bodies )
{

}

inline	void integrator_ode::add_body( dxBody *b )
{
	ASSERT( b );
	m_bodies.push_back( b );
}

inline	void integrator_ode::add_joint( dxJoint *j )
{
	ASSERT( j );
	m_joints.push_back( j );
}

#endif