////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "interpolator_size_calculator.h"
#include <xray/animation/instant_interpolator.h>
#include <xray/animation/linear_interpolator.h>
#include <xray/animation/fermi_interpolator.h>

using xray::animation::interpolator_size_calculator;
using xray::animation::instant_interpolator;
using xray::animation::linear_interpolator;
using xray::animation::fermi_interpolator;

void interpolator_size_calculator::visit	( instant_interpolator const& interpolator )
{
	XRAY_UNREFERENCED_PARAMETER	( interpolator );
	m_size						+= sizeof( instant_interpolator );
}

void interpolator_size_calculator::visit	( linear_interpolator const& interpolator )
{
	XRAY_UNREFERENCED_PARAMETER	( interpolator );
	m_size						+= sizeof( linear_interpolator );
}

void interpolator_size_calculator::visit	( fermi_interpolator const& interpolator )
{
	XRAY_UNREFERENCED_PARAMETER	( interpolator );
	m_size						+= sizeof( fermi_interpolator );
}