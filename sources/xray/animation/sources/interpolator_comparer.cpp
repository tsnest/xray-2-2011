////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "interpolator_comparer.h"
#include <xray/animation/instant_interpolator.h>
#include <xray/animation/linear_interpolator.h>
#include <xray/animation/fermi_interpolator.h>

using xray::animation::interpolator_comparer;
using xray::animation::instant_interpolator;
using xray::animation::linear_interpolator;
using xray::animation::fermi_interpolator;

void interpolator_comparer::dispatch	( instant_interpolator const& left, instant_interpolator const& right )
{
	XRAY_UNREFERENCED_PARAMETERS(&left, &right );
	result						= equal;
}

void interpolator_comparer::dispatch	( instant_interpolator const& left, linear_interpolator const& right )
{
	XRAY_UNREFERENCED_PARAMETERS(&left, &right );
	result						= less;
}

void interpolator_comparer::dispatch	( instant_interpolator const& left, fermi_interpolator const& right )
{
	XRAY_UNREFERENCED_PARAMETERS(&left, &right );
	result						= less;
}

void interpolator_comparer::dispatch	( linear_interpolator const& left, instant_interpolator const& right )
{
	XRAY_UNREFERENCED_PARAMETERS(&left, &right );
	result						= more;
}

void interpolator_comparer::dispatch	( linear_interpolator const& left, linear_interpolator const& right )
{
	float const left_transition_time	= left.transition_time();
	float const right_transition_time	= right.transition_time();
	result						= left_transition_time < right_transition_time ? less : left_transition_time > right_transition_time ? more : equal;
}

void interpolator_comparer::dispatch	( linear_interpolator const& left, fermi_interpolator const& right )
{
	XRAY_UNREFERENCED_PARAMETERS(&left, &right );
	result						= less;
}

void interpolator_comparer::dispatch	( fermi_interpolator const& left, instant_interpolator const& right )
{
	XRAY_UNREFERENCED_PARAMETERS(&left, &right );
	result						= more;
}

void interpolator_comparer::dispatch	( fermi_interpolator const& left, linear_interpolator const& right )
{
	XRAY_UNREFERENCED_PARAMETERS(&left, &right );
	result						= more;
}

void interpolator_comparer::dispatch	( fermi_interpolator const& left, fermi_interpolator const& right )
{
	float const left_transition_time	= left.transition_time();
	float const right_transition_time	= right.transition_time();
	if ( left_transition_time < right_transition_time ) {
		result					= less;
		return;
	}

	if ( right_transition_time < left_transition_time ) {
		result					= more;
		return;
	}

	float const left_epsilon	= left.epsilon();
	float const right_epsilon	= right.epsilon();
	result						= left_epsilon < right_epsilon ? less : left_epsilon > right_epsilon ? more : equal;
}