////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.12.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_world.h"
#include <xray/sound/api.h>
#include <xray/linkage_helper.h>

XRAY_DECLARE_LINKAGE_ID(sound_entry_point)

using xray::sound::sound_world;

static xray::uninitialized_reference<sound_world>	s_world;

namespace xray {
namespace sound {

static bool is_enum_exported = false;

#ifndef MASTER_GOLD
static configs::enum_tuple tuples[] =
{
	std::make_pair( "single_sound_type",		resources::single_sound_class	  ),
	std::make_pair( "composite_sound_type",		resources::composite_sound_class  ),
	std::make_pair( "collection_sound_type",	resources::sound_collection_class )
};
#endif // #ifndef MASTER_GOLD

} // namespace sound
} // namespace xray

xray::sound::allocator_type* xray::sound::g_allocator = 0;

xray::sound::world* xray::sound::create_world	( engine& engine, base_allocator_type& logic_allocator, base_allocator_type* editor_allocator )
{
#ifndef MASTER_GOLD	
	if ( !xray::sound::is_enum_exported )
	{
		xray::configs::export_enum( "resources", xray::sound::tuples, xray::sound::tuples + array_size( tuples ) );
		xray::sound::is_enum_exported = true;
	}
#endif // #ifndef MASTER_GOLD
	
	XRAY_CONSTRUCT_REFERENCE	( s_world, sound_world )( engine, logic_allocator, editor_allocator );
	return						( &*s_world );
}

void xray::sound::destroy_world	( xray::sound::world*& world )
{
	R_ASSERT					( &*s_world == world );
	XRAY_DESTROY_REFERENCE		( s_world );
	world						= 0;
}

void xray::sound::set_memory_allocator	( allocator_type& allocator )
{
	ASSERT						( !g_allocator );
	g_allocator					= &allocator;
}
