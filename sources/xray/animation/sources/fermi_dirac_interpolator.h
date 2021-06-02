////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FERMI_DIRAC_INTERPOLATOR_H_INCLUDED
#define FERMI_DIRAC_INTERPOLATOR_H_INCLUDED

#include "base_interpolator.h"

namespace xray {
namespace animation {

class fermi_dirac_interpolator : public base_interpolator {
public:
	explicit		fermi_dirac_interpolator( float const transition_time, float const epsilon = .005f );
	virtual	float	interpolated_value		( float current_transition_time ) const;
	virtual	float	transition_time			( ) const;
	inline	float	epsilon					( ) const { return m_epsilon; }

	virtual	fermi_dirac_interpolator* clone	( mutable_buffer& buffer ) const;

	virtual void	accept					( interpolator_visitor& visitor ) const;

	virtual void	accept					( interpolator_comparer& dispatcher, base_interpolator const& interpolator ) const;
	virtual void	visit					( interpolator_comparer& dispatcher, instant_interpolator const& interpolator ) const;
	virtual void	visit					( interpolator_comparer& dispatcher, linear_interpolator const& interpolator ) const;
	virtual void	visit					( interpolator_comparer& dispatcher, fermi_dirac_interpolator const& interpolator ) const;

private:
	fermi_dirac_interpolator& operator =	( fermi_dirac_interpolator const& );

private:
	float const		m_total_transition_time;
	float const		m_epsilon;
}; // class fermi_dirac_interpolator

} // namespace animation
} // namespace xray

#endif // #ifndef FERMI_DIRAC_INTERPOLATOR_H_INCLUDED