////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "skeleton_animation_data.h"
#include "bone_names.h"
#include "skeleton.h"

namespace xray {
namespace animation {

skeleton_animation_data::skeleton_animation_data( ):
m_bone_names( NEW(animation::bone_names) ),
m_skeleton	( 0 )
{
	
}

skeleton_animation_data::~skeleton_animation_data( )
{
	DELETE( m_bone_names );
}

i_bone_names* skeleton_animation_data::get_bone_names( )
{
	return m_bone_names;
}

void skeleton_animation_data::set_num_bones( u32 num_bones )
{
	m_bone_data.resize( num_bones );
}

void skeleton_animation_data::save( xray::configs::lua_config_value &cfg )const
{
	xray::configs::lua_config_value cfg_data  = cfg["data"];
	const u32 size = m_bone_data.size();
	for( u32 i=0; i<size; ++i )
		m_bone_data[i].save( cfg_data[i] );
///////////////////////////////////////////////////
	ASSERT( m_bone_names );
	xray::configs::lua_config_value temp = cfg["bone_names"];
	m_bone_names->write( temp );

}

void skeleton_animation_data::load( xray::configs::lua_config_value const &cfg )
{
	xray::configs::lua_config_value const cfg_data  = cfg["data"];
	const u32 size = cfg_data.size();
	m_bone_data.resize( size );
	for( u32 i=0; i<size; ++i )
		m_bone_data[i].load( cfg_data[i] );
////////////////////////////////////////////////
	ASSERT( m_bone_names );
	m_bone_names->read( cfg["bone_names"] );

}

bool skeleton_animation_data::compare( i_skeleton_animation_data const & data ) const
{
	return compare( static_cast_checked< skeleton_animation_data const & > ( data ) );
}

bool skeleton_animation_data::compare( skeleton_animation_data const & data ) const
{
	const u32 size = num_bones();
	if( data.num_bones() != size )
		return false;

	for( u32 i=0; i<size; ++i )
	{
		const xray::animation::bone_animation_data&d = data.bone( i );
		if( !m_bone_data[i].compare( d ) )
			return false;
	}
	return true;
}

void skeleton_animation_data::set_skeleton	( const skeleton *skel )
{
	m_skeleton = skel;
	ASSERT( m_bone_names );
	m_bone_names->create_index( *m_skeleton, m_skeleton_bone_index );
}

const	vector< u32 >&	skeleton_animation_data::skeleton_bone_index( )const
{ 
	ASSERT( m_skeleton );
	ASSERT( m_skeleton->bone_count() == m_skeleton_bone_index.size() );

	return m_skeleton_bone_index;
}

} // namespace animation
} // namespace xray

