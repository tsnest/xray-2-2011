////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_data.h"
#include "anim_track_discrete_channels.h"

using xray::maya_animation::animation_curve_data_header;
using xray::maya_animation::anim_track_discrete_channels;

using namespace xray::animation;
using namespace xray::maya_animation;

namespace xray {
namespace maya_animation {

void	anim_track_discrete_channels::save( FILE* f ) const
{
	fwrite( &m_start_time, sizeof(m_start_time), 1, f );
	for(u32 i = channel_translation_x; i < channel_max; ++i )
		write( m_channels[i], f ); 
}

void	anim_track_discrete_channels::load( FILE* f )
{
	fread( &m_start_time, sizeof(m_start_time), 1, f );
	for(u32 i = channel_translation_x; i < channel_max; ++i )
		read( m_channels[i], f ); 
}

void	anim_track_discrete_channels::check( configs::lua_config_value const& f )const
{
	for ( u32 i = channel_translation_x; i < channel_max; ++i ) {
		configs::lua_config_value const temp	= f[i]["vector_data"];
		channels_type::const_iterator k			= m_channels[i].begin();
		channels_type::const_iterator const e	= m_channels[i].end();
		for ( u32 j=0; k != e; ++k, ++j ) {
			ASSERT_CMP							( *k, ==, (float)temp[j][0] ) ;
		}
	}
}

void			anim_track_discrete_channels::save( xray::configs::lua_config_value f )const
{
	f["start_time"] = m_start_time;
	for(u32 i = channel_translation_x; i < channel_max; ++i )
			lua_write_vector( m_channels[i], f["data"][i] );

	
}

void			anim_track_discrete_channels::load(  xray::configs::lua_config_value const& f )
{
	m_start_time = 	f["start_time"] ;
	for(u32 i = channel_translation_x; i < channel_max; ++i )
		lua_read_vector( m_channels[i], f["data"][i] );
		
}



void		anim_track_discrete_channels::setup_header(  animation_curve_data_header &h )
{
	h.type = channels_equidistant_frames;
	h.equidistant_frames_frequency = 1.f/30.f;
}


	u32				anim_track_discrete_channels::frames_count	()const
	{
		const u32 cnt = m_channels[channel_translation_x].size();

		for(u32 i = channel_translation_x + 1; i < channel_max; ++i )
			ASSERT( m_channels[i].size() == cnt );

		return cnt;
	}

	frame			anim_track_discrete_channels::get_frame		( u32 id )const
	{
		
		ASSERT( id < frames_count() );

		frame ret = zero;
		for(u32 i = channel_translation_x ; i < channel_max; ++i )
				ret.channels[i] = m_channels[i][id];
		
		return ret;
	}

	void			anim_track_discrete_channels::set_frame		( u32 id, const frame &f )
	{
		
		ASSERT( id < frames_count() );
		
		for(u32 i = channel_translation_x ; i < channel_max; ++i )
			 m_channels[i][id] = f.channels[i];
		
	}

	void	anim_track_discrete_channels::set_difference	( )
	{
			set_difference( get_frame( 0 ) );
	}

	///root non zero
	//void	anim_track_discrete_channels::set_difference	( const animation::frame &/*root_start*/ )
	//{
	//	const u32 cnt = frames_count();
	//	
	//	frame start = get_frame( 0 );
	//	start.rotation = to_close_xyz( start.rotation, zero.rotation, math::pi_d2 );
	//	set_frame( 0, start );

	//	for( u32 i =1; i < cnt; ++i )
	//	{
	//		
	//		frame f = get_frame( i );

	//		if( i > 0 )
	//		{
	//			frame prev_f = get_frame( i - 1 );
	//			f.rotation = to_close_xyz( f.rotation, prev_f.rotation, math::pi_d2 );
	//		}
	//		set_frame( i, f );
	//	}

	//}

	void	anim_track_discrete_channels::set_difference	( const animation::frame &f )
	{
		const u32 cnt = frames_count();

		float4x4 sub; sub.try_invert( frame_matrix( f ) );

		for( u32 i =0; i < cnt; ++i )
		{
			frame f = matrix_to_frame( frame_matrix(  get_frame( i ) ) * sub ) ;
			if( i > 0 )
			{
				frame prev_f = get_frame( i - 1 );
				f.rotation = to_close_xyz( f.rotation, prev_f.rotation, math::pi_d2 );
			}
			set_frame( i, f );
		}

	}


void anim_track_discrete_channels::bone_pos( xray::float4x4 &, float )
{
	
}

} // namespace maya_animation
} // namespace xray





