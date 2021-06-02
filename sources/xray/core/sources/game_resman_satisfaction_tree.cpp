////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resman_satisfaction_tree.h"

namespace xray {
namespace resources {

bool   compare_by_target_satisfaction::operator () (resource_base const & left, resource_base const & right) const
{
	float const left_satisfaction		=	left.target_satisfaction();
	float const right_satisfaction		=	right.target_satisfaction();
	if ( left_satisfaction < right_satisfaction )
		return								true;

	if ( left_satisfaction == right_satisfaction )
		return								& left < & right;	

	return									false;
}

} // namespace resources
} // namespace xray