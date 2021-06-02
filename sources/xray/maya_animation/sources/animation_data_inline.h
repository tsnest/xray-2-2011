////////////////////////////////////////////////////////////////////////////
//	Created		: 10.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
//using xray::animation::animation_data;
//using xray::animation::anim_track_discrete;
//using xray::animation::animation_data_discret;
#include <xray/configs_lua_config.h>
#include "anim_track_discrete_channels.h"

namespace xray {
namespace maya_animation {

template< class TrackType >
void	t_animation_data< TrackType >::build( animation_data& ad )
{
	const u32 count = ad.anim_bone_count();
	m_bone_animations.resize(count);
	for( u32 i=0; i<count; ++i )
		m_bone_animations[i].build( ad.bone_animation(i) );
}

template< class TrackType >
void	t_animation_data< TrackType >::bone_pose( xray::float4x4 &pose, u32 id, float time )
{
	m_bone_animations[id].bone_pos( pose, time );
}

template< typename T >
inline void write(const T &v, FILE*  f )
{
	fwrite( &v, sizeof(T), 1, f);
}

template< typename T  >
inline void read( T &v,  FILE*  f )
{
	fread( &v, sizeof(T), 1, f);
}

template< typename T  >
inline void write( const  vector<T> &v,  FILE*  f )
{
	u32 size = v.size();
	write( size, f );
	for( u32 i= 0; i< size ; ++i )
		write( v[i], f );
}

template< typename T >
inline void read(  vector<T> &v,  FILE*  f )
{
	u32 size = 0;
	read( size, f );
	v.resize( size );
	for( u32 i= 0; i< size ; ++i )
		read( v[i], f );
}

template< class TrackType >
void t_animation_data< TrackType >::save( FILE* f )const
{
	
	write( m_header, f );

	write( m_bone_animations, f  );
}

template< class TrackType >
void t_animation_data< TrackType >::load( FILE* f )
{
	read( m_header, f );

	read( m_bone_animations, f );
}

template<class TrackType>
bool t_animation_data<TrackType>::save_file(  pcstr fn )const
{
	XRAY_UNREFERENCED_PARAMETER	( fn );
#ifdef DEBUG
	return false;
#endif // #ifdef DEBUG
}

template< class TrackType >
bool t_animation_data< TrackType >::load_file( pcstr fn )
{
	FILE*				f;
	int res				= fopen_s(&f, fn, "rb");
	
	if ( res != 0 )
		return			false;
	
	load( f );

	fclose( f );
	return true;

}

template < typename T >
inline void lua_write( T const& v,  xray::configs::lua_config_value &cfg )
{
	cfg = v; //[0]
}

template <>
inline void lua_write<float>( float const& v,  xray::configs::lua_config_value &cfg )
{
	cfg = v; //[0]
}


template < typename T >
inline void lua_read( T& v, xray::configs::lua_config_value const &cfg )
{
	v = cfg; // [0]
}

inline void lua_write( const animation_curve_data_header &h, xray::configs::lua_config_value f )
{
	f["type"] = (u32)h.type;
	f["frequency"] = h.equidistant_frames_frequency;
}

inline void lua_read(  animation_curve_data_header &h, xray::configs::lua_config_value const &f )
{
	h.type =(animation_curve_data_type) ( (u32)f["type"] ) ;
	h.equidistant_frames_frequency = f["frequency"];
}

template< typename T >
inline void lua_read_vector(  vector<T> &v, xray::configs::lua_config_value const &f )
{
	u32 size = f.size();

	v.resize( size );
	for( u32 i= 0; i< size ; ++i )
		lua_read( v[i], f[i] );
}

template< typename  T >
inline void lua_write_vector( vector<T> const& v, xray::configs::lua_config_value cfg )
{
	u32 size = v.size();

	for( u32 i= 0; i< size ; ++i ) {
		xray::configs::lua_config_value tmp_cfg = cfg[i];
		lua_write( v[i], tmp_cfg );
	}
}

template< class TrackType >
void	t_animation_data< TrackType >::save( configs::lua_config_value &cfg )const
{
	lua_write( m_header, cfg["header"] );
	lua_write_vector<TrackType>( m_bone_animations, cfg["animation_data"] );

}

template< class TrackType >
void	t_animation_data<TrackType>::load( configs::lua_config_value const& cfg )
{
	lua_read( m_header, cfg["header"] );

	lua_read_vector<TrackType>( m_bone_animations, cfg["animation_data"] );
}

template < class TrackType >
void	t_animation_data< TrackType >::check( configs::lua_config_value const& cfg ) const
{

	u32 size		= cfg["vector_size"];
	ASSERT_CMP		( m_bone_animations.size(), ==, size );

	for( u32 i= 0; i< size ; ++i ) {
		m_bone_animations[i].check	( cfg["vector_data"][i] );
	}
}

} // namespace maya_animation
} // namespace xray