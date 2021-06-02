////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef INTERPOLATOR_SIZE_CALCULATOR_H_INCLUDED
#define INTERPOLATOR_SIZE_CALCULATOR_H_INCLUDED

#include <xray/animation/interpolator_visitor.h>

namespace xray {
namespace animation {

class interpolator_size_calculator : public interpolator_visitor {
public:
	inline	interpolator_size_calculator( ) : m_size( 0 ) { }
	inline	u32		calculated_size		( ) const { return m_size; }
	virtual	void	visit				( instant_interpolator const& interpolator );
	virtual	void	visit				( linear_interpolator const& interpolator );
	virtual	void	visit				( fermi_interpolator const& interpolator );

private:
	mutable u32		m_size;
}; // class interpolator_size_calculator

} // namespace animation
} // namespace xray

#endif // #ifndef INTERPOLATOR_SIZE_CALCULATOR_H_INCLUDED