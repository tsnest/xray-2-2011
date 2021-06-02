////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.02.2008
//	Author		: Konstantin Slipchenko
//	Description : static element
////////////////////////////////////////////////////////////////////////////
#include "pch.h"

#include "static_element.h"
#include <xray/collision/geometry_instance.h>
#include "island.h"

bool	static_element::collide( island& i, const xray::collision::geometry_instance& gi, bool reverted_order )const
{
	XRAY_UNREFERENCED_PARAMETERS	( &i, &gi, reverted_order );
	UNREACHABLE_CODE				( return false );
}

bool	static_element::collide( island& i, const collide_element_interface& o, bool reverted_order )const
{
	XRAY_UNREFERENCED_PARAMETERS( reverted_order );
	return o.collide( i, m_geometry_instance, !reverted_order );
}

bool	static_element::collide( island& , const xray::collision::geometry_instance& , body& )const
{
	UNREACHABLE_CODE(return false);
}

bool	static_element::integrate( const step_counter_ref& sr, float time_delta )
{
	XRAY_UNREFERENCED_PARAMETERS( &sr, time_delta );
	UNREACHABLE_CODE(return false);
}