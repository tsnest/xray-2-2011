////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ENCODED_SOUND_WITH_QUALITIES_COOK_H_INCLUDED
#define ENCODED_SOUND_WITH_QUALITIES_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

class encoded_sound_with_qualities_cook :
	public resources::translate_query_cook,
	private boost::noncopyable
{
public:
					encoded_sound_with_qualities_cook	( );

	virtual	void		translate_query					( resources::query_result_for_cook& parent );
	virtual void		delete_resource					( resources::resource_base* resource );
	virtual u32			calculate_quality_levels_count	( resources::query_result_for_cook const * ) ;
	virtual float		satisfaction_with				( u32 quality_level, math::float4x4 const &user_matrix, u32 users_count );
}; // class encoded_sound_with_qualities_cook

} // namespace sound
} // namespace xray

#endif // #ifndef ENCODED_SOUND_WITH_QUALITIES_COOK_H_INCLUDED
