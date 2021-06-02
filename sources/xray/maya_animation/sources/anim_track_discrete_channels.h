////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ANIM_TRACK_DISCRETE_CHANNELS_H_INCLUDED
#define ANIM_TRACK_DISCRETE_CHANNELS_H_INCLUDED


#include <xray/animation/anim_track_common.h>

namespace xray {

namespace configs
{
	class lua_config_value;
} // namespace configs

namespace maya_animation {

struct animation_curve_data_header;
struct anim_track;

struct anim_track_discrete_channels
{
	anim_track_discrete_channels():m_start_time(0){}

	void				build			( anim_track&  ){};
	
	static void			setup_header	( animation_curve_data_header &h );
	void				save			( FILE* f )const;
	void				load			( FILE* f );

	void				save			( configs::lua_config_value f )const;
	void				load			( configs::lua_config_value const& f );
	void				check			( configs::lua_config_value const& f )const;
	
	u32					frames_count	()const;
	animation::frame	get_frame		( u32 i )const;
	void				set_frame		( u32 i, const animation::frame &f );
	
	void				set_difference	( const animation::frame &f );
	void				set_difference	( );

	void				bone_pos		( xray::float4x4 &pose, float time );

	typedef vector<float>			channels_type;
	channels_type					m_channels[ animation::channel_max ];
	float							m_start_time;

}; // struct anim_track_discrete_channels

} // namespace maya_animation
} // namespace xray

#include "anim_track_discrete_channels_inline.h"

#endif // #ifndef ANIM_TRACK_DISCRETE_CHANNELS_H_INCLUDED