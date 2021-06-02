////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object.h"

namespace stalker2{

game_object_::game_object_( game_world& w )
:m_game_world	( w ),
m_loaded_cnt	( 0 )
{}

void game_object_::load( configs::binary_config_value const& )
{
}

game_object_static::game_object_static( game_world& w )
:super			( w )
{}

void game_object_static::load( configs::binary_config_value const& t )
{
	super::load				( t );
	float3 const& scale		= t["scale"];
	float3 const& rotation	= t["rotation"];
	float3 const& position	= t["position"];
	m_transform				= create_scale(scale) * create_rotation(rotation) * create_translation(position);
}

} // namespace stalker2