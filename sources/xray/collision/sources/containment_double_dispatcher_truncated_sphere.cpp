////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "containment_double_dispatcher.h"
#include "box_geometry_instance.h"
#include "sphere_geometry_instance.h"
#include "cylinder_geometry_instance.h"
#include "capsule_geometry_instance.h"
#include "truncated_sphere_geometry_instance.h"
#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>

namespace xray{
namespace collision	{

void containment_double_dispatcher::dispatch	( box_geometry_instance const& bounding_volume,				truncated_sphere_geometry_instance const& testee )
{
	XRAY_UNREFERENCED_PARAMETERS( bounding_volume, testee );
	NOT_IMPLEMENTED( );
}

void containment_double_dispatcher::dispatch	( sphere_geometry_instance const& bounding_volume,			truncated_sphere_geometry_instance const& testee )
{
	XRAY_UNREFERENCED_PARAMETERS( bounding_volume, testee );
	NOT_IMPLEMENTED( );
}

void containment_double_dispatcher::dispatch	( cylinder_geometry_instance const& bounding_volume,			truncated_sphere_geometry_instance const& testee )
{
	XRAY_UNREFERENCED_PARAMETERS( bounding_volume, testee );
	NOT_IMPLEMENTED( );
}

void containment_double_dispatcher::dispatch	( capsule_geometry_instance const& bounding_volume,			truncated_sphere_geometry_instance const& testee )
{
	XRAY_UNREFERENCED_PARAMETERS( bounding_volume, testee );
	NOT_IMPLEMENTED( );
}

void containment_double_dispatcher::dispatch	( truncated_sphere_geometry_instance const& bounding_volume,	truncated_sphere_geometry_instance const& testee )
{
	XRAY_UNREFERENCED_PARAMETERS( bounding_volume, testee );
	NOT_IMPLEMENTED( );
}

} // namespace collision
} // namespace xray
