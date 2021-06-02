////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef INSTANT_INTERPOLATOR_H_INCLUDED
#define INSTANT_INTERPOLATOR_H_INCLUDED

#include "base_interpolator.h"

namespace xray {
namespace animation {

class instant_interpolator : public base_interpolator {
public:
	virtual	float	interpolated_value	( float current_transition_time ) const;
	virtual	float	transition_time		( ) const;
	virtual	instant_interpolator* clone	( mutable_buffer& buffer ) const;

	virtual void	accept				( interpolator_visitor& visitor ) const;

	virtual void	accept				( interpolator_comparer& dispatcher, base_interpolator const& interpolator ) const;
	virtual void	visit				( interpolator_comparer& dispatcher, instant_interpolator const& interpolator ) const;
	virtual void	visit				( interpolator_comparer& dispatcher, linear_interpolator const& interpolator ) const;
	virtual void	visit				( interpolator_comparer& dispatcher, fermi_dirac_interpolator const& interpolator ) const;
}; // class instant_interpolator

} // namespace animation
} // namespace xray

#endif // #ifndef INSTANT_INTERPOLATOR_H_INCLUDED