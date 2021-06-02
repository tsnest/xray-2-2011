////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fermi_dirac_interpolator.h"
#include "interpolator_comparer.h"

using xray::animation::fermi_dirac_interpolator;
using xray::animation::interpolator_comparer;
using xray::animation::interpolator_visitor;
using xray::animation::instant_interpolator;
using xray::animation::linear_interpolator;

fermi_dirac_interpolator::fermi_dirac_interpolator			( float const transition_time, float const epsilon ) :
	m_total_transition_time	( transition_time ),
	m_epsilon				( epsilon )
{
}

float fermi_dirac_interpolator::interpolated_value			( float current_transition_time ) const
{
	R_ASSERT				( current_transition_time < m_total_transition_time );

	float const d			= m_total_transition_time / ( 2.f*math::log(1.f/m_epsilon + 1.f) );
	float const magnitude	= 1.f + 2.f*m_epsilon;
	float const t0			= m_total_transition_time/2.f;
	float const value		= magnitude /( 1.f + math::exp( -(current_transition_time - t0)/d ) );

	return					math::clamp_r( value - m_epsilon, 0.f, 1.f );
}

float fermi_dirac_interpolator::transition_time				( ) const
{
	return					m_total_transition_time;
}

fermi_dirac_interpolator* fermi_dirac_interpolator::clone	( xray::mutable_buffer& buffer ) const
{
	return					clone_impl< fermi_dirac_interpolator >( buffer );
}

void fermi_dirac_interpolator::accept						( interpolator_visitor& visitor ) const
{
	visitor.visit			( *this );
}

void fermi_dirac_interpolator::accept						( interpolator_comparer& dispatcher, base_interpolator const& interpolator ) const
{
	interpolator.visit		( dispatcher, *this );
}

void fermi_dirac_interpolator::visit						( interpolator_comparer& dispatcher, instant_interpolator const& interpolator ) const
{
	dispatcher.dispatch		( interpolator, *this );
}

void fermi_dirac_interpolator::visit						( interpolator_comparer& dispatcher, linear_interpolator const& interpolator ) const
{
	dispatcher.dispatch		( interpolator, *this );
}

void fermi_dirac_interpolator::visit						( interpolator_comparer& dispatcher, fermi_dirac_interpolator const& interpolator ) const
{
	dispatcher.dispatch		( interpolator, *this );
}