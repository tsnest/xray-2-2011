////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.02.2008
//	Author		: Konstantin Slipchenko
//	Description : integrator
////////////////////////////////////////////////////////////////////////////
#include "pch.h"

#include "integrator_ode.h"
#include "ode_include.h"


void integrator_ode::step( float time_delta )
{
	//void dInternalStepIsland (dxWorld *world,
	//		  dxBody * const *body, int nb,
	//		  dxJoint **joint, int nj,
	//		  dReal stepsize);//dWorldStep
	//if( m_joints.size() < 10 )
		dInternalStepIsland( 0, &m_bodies.front(), m_bodies.size(), &m_joints.front(), m_joints.size(), time_delta );
	//else
		//dxQuickStepper( 0, &m_bodies.front(), m_bodies.size(), &m_joints.front(), m_joints.size(), time_delta );

	m_bodies.clear();
	m_joints.clear();
}
