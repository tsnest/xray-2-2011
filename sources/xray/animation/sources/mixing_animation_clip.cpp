////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_animation_clip.h"
#include "base_interpolator.h"

using xray::animation::mixing::animation_clip;

bool animation_clip::operator ==			( animation_clip const& other ) const
{
	if ( type() != other.type() )
		return		false;

	if ( compare(interpolator(), other.interpolator()) != interpolator_comparer::equal )
		return		false;

	return			!strings::compare( identifier(), other.identifier() );
}

bool xray::animation::mixing::operator <	( animation_clip const& left, animation_clip const& right )
{
	if ( left.type() < right.type() )
		return		true;

	if ( right.type() < left.type() )
		return		false;

	switch ( compare(left.interpolator(), right.interpolator()) ) {
		case interpolator_comparer::less : return true;
		case interpolator_comparer::more : return false;
		case interpolator_comparer::equal : 
			return	strings::compare( left.identifier(), right.identifier() ) < 0;
		default :	NODEFAULT(return false);
	}
}