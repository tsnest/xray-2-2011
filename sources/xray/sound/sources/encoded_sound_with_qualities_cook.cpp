////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "encoded_sound_with_qualities_cook.h"
#include <xray/sound/encoded_sound_with_qualities.h>

namespace xray {
namespace sound {

using namespace resources;

encoded_sound_with_qualities_cook::encoded_sound_with_qualities_cook	( ) : 
	translate_query_cook	(	encoded_sound_interface_class, 
								cook_base::reuse_true, 
								use_resource_manager_thread_id )
{

}

void encoded_sound_with_qualities_cook::translate_query	( query_result_for_cook& parent )
{
	ASSERT								( parent.is_autoselect_quality_query() );

	encoded_sound_with_qualities* sound	= UNMANAGED_NEW( sound, encoded_sound_with_qualities )( );

	u32 const target_quality_level		= parent.target_quality_level( );
	ASSERT								( target_quality_level != u32(-1) );
	parent.set_unmanaged_resource		( sound, nocache_memory, sizeof(encoded_sound_with_qualities) );
	sound->increase_quality				( target_quality_level, parent.target_satisfaction(), &parent );
}

void encoded_sound_with_qualities_cook::delete_resource	( resource_base* resource )
{
	UNMANAGED_DELETE			( resource );
}

u32 encoded_sound_with_qualities_cook::calculate_quality_levels_count	( query_result_for_cook const * )
{
		return encoded_sound_with_qualities::max_quality_levels_count; 
}

float encoded_sound_with_qualities_cook::satisfaction_with	( u32 quality_level, math::float4x4 const &user_matrix, u32 users_count )
{
	XRAY_UNREFERENCED_PARAMETERS	( &user_matrix, users_count );
	if ( quality_level == u32(-1) )
		return					0;
	u32 const max_levels_count	= encoded_sound_with_qualities::max_quality_levels_count;
	return						(float)(max_levels_count - quality_level) / max_levels_count;
}

} // namespace sound
} // namespace xray
