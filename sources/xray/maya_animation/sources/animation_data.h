////////////////////////////////////////////////////////////////////////////
//	Created		: 07.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_DATA_H_INCLUDED
#define ANIMATION_DATA_H_INCLUDED
//#include "vector .h"
//#include "anim_track_hermite.h"

#include <xray/maya_animation/data_header.h>

namespace xray {

namespace configs
{
	class lua_config_value;
} // namespace configs

namespace maya_animation {

class animation_data;

template< class TrackType >
class t_animation_data
{
public:
						t_animation_data	( ){ TrackType::setup_header( m_header ); }
		void			build				( animation_data& ad );
		void			save				( FILE* f )const;
		void			load				( FILE* f );
		bool			save_file			( pcstr fn )const;
		bool			load_file			( pcstr fn );

		void			save				( configs::lua_config_value &cfg )const;
		void			load				( configs::lua_config_value const& cfg );
		void			check				( configs::lua_config_value const& cfg ) const;

		void			bone_pose			( xray::float4x4 &pose, u32 id, float time );
		u32				anim_bone_count		( )const		{ return m_bone_animations.size(); }
		void			set_anim_bone_count	( u32 cnt )		{ return m_bone_animations.resize(cnt); }
		TrackType&		bone_animation		( u32 i )		{ return m_bone_animations[i]; }
	TrackType const&	bone_animation		( u32 i ) const { return m_bone_animations[i]; }

vector<TrackType> &bone_animations(){ return m_bone_animations; }

		animation_curve_data_header &header( )		{ return m_header; }
const	animation_curve_data_header &header( )const	{ return m_header; }

private:
	vector< TrackType >			m_bone_animations;
	animation_curve_data_header m_header;

}; // class t_animation_data;

} // namespace maya_animation
} // namespace xray

#include "animation_data_inline.h"

#endif // #ifndef ANIMATION_DATA_H_INCLUDED