////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef LINEAR_INTERPOLATOR_H_INCLUDED
#define LINEAR_INTERPOLATOR_H_INCLUDED

#include "base_interpolator.h"

namespace xray {
namespace animation {

class linear_interpolator : public base_interpolator {
public:
	inline			linear_interpolator	( float const transition_time ) : m_total_transition_time(transition_time) { }
	virtual	float	interpolated_value	( float current_transition_time ) const;
	virtual	float	transition_time		( ) const;
	virtual	linear_interpolator* clone	( mutable_buffer& buffer ) const;

	virtual void	accept				( interpolator_visitor& visitor ) const;

	virtual void	accept				( interpolator_comparer& dispatcher, base_interpolator const& interpolator ) const;
	virtual void	visit				( interpolator_comparer& dispatcher, instant_interpolator const& interpolator ) const;
	virtual void	visit				( interpolator_comparer& dispatcher, linear_interpolator const& interpolator ) const;
	virtual void	visit				( interpolator_comparer& dispatcher, fermi_dirac_interpolator const& interpolator ) const;

private:
	float			m_total_transition_time;
}; // class linear_interpolator

} // namespace animation
} // namespace xray

#endif // #ifndef LINEAR_INTERPOLATOR_H_INCLUDED