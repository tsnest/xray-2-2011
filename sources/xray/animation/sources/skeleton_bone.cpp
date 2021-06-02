////////////////////////////////////////////////////////////////////////////
//	Created		: 11.08.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/skeleton_bone.h>

using xray::animation::skeleton_bone;

skeleton_bone::skeleton_bone( 
		pcstr const id,
		skeleton_bone const* const parent,
		skeleton_bone const* const children_begin,
		skeleton_bone const* const children_end
	) :
	m_id					( id ),
	m_parent				( parent ),
	m_children_begin		( children_begin ),
	m_children_end			( children_end )
{
}