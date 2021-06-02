////////////////////////////////////////////////////////////////////////////
//	Created		: 13.10.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/base_interpolator.h>
#include "interpolator_comparer.h"

using xray::animation::base_interpolator;

xray::animation::comparison_result_enum xray::animation::compare	(
		base_interpolator const& left,
		base_interpolator const& right
	)
{
	xray::animation::interpolator_comparer	comparer;
	left.accept								( comparer, right );
	return									comparer.result;
}