////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "object_weapon.h"

namespace stalker2 {

object_weapon::object_weapon	( ai::weapon_types_enum type, pcstr name, u32 id, u32 ammo_count ) :
	m_type						( type ),
	m_name						( name ),
	m_id						( id ),
	m_ammo_count				( ammo_count ),
	m_next						( 0 )
{
}

object_weapon::~object_weapon	( )
{
}

float3 object_weapon::get_random_surface_point( u32 const current_time ) const
{
	NOT_IMPLEMENTED				( current_time );
	UNREACHABLE_CODE			( return float3() );
}

float4x4 object_weapon::local_to_cell	( float3 const& requester ) const
{
	NOT_IMPLEMENTED				( requester );
	UNREACHABLE_CODE			( return float4x4() );
}

} // namespace stalker2
