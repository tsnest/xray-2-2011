////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef INTERPOLATOR_COMPARER_H_INCLUDED
#define INTERPOLATOR_COMPARER_H_INCLUDED

#include <xray/animation/base_interpolator.h>

namespace xray {
namespace animation {

class instant_interpolator;
class linear_interpolator;
class fermi_interpolator;

enum comparison_result_enum;

class XRAY_ANIMATION_API interpolator_comparer {
public:
	void	dispatch	( instant_interpolator const& left, instant_interpolator const& right );
	void	dispatch	( instant_interpolator const& left, linear_interpolator const& right );
	void	dispatch	( instant_interpolator const& left, fermi_interpolator const& right );

	void	dispatch	( linear_interpolator const& left, instant_interpolator const& right );
	void	dispatch	( linear_interpolator const& left, linear_interpolator const& right );
	void	dispatch	( linear_interpolator const& left, fermi_interpolator const& right );

	void	dispatch	( fermi_interpolator const& left, instant_interpolator const& right );
	void	dispatch	( fermi_interpolator const& left, linear_interpolator const& right );
	void	dispatch	( fermi_interpolator const& left, fermi_interpolator const& right );

public:
	comparison_result_enum	result;
}; // class interpolator_comparer

} // namespace animation
} // namespace xray

#endif // #ifndef INTERPOLATOR_COMPARER_H_INCLUDED