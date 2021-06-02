////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "bi_spline_skeleton_animation.h"
#include <xray/animation/skeleton.h>
#include <xray/animation/skeleton_animation.h>

namespace xray {
namespace animation {

bi_spline_skeleton_animation::bi_spline_skeleton_animation	( ):
	m_bone_names_memory		( 0 ),
	m_event_channels_memory	( 0 ),
	m_event_channels		( 0 ),
	m_animation_type		( animation_type_full ),
	m_bone_names			( 0 )
{
}

bi_spline_skeleton_animation::~bi_spline_skeleton_animation	( )
{
	if ( m_event_channels )
		m_event_channels->~animation_event_channels();

	m_event_channels = 0;

	m_bone_names->~bone_names();
	FREE( m_bone_names_memory );
	FREE( m_event_channels_memory );
}

void bi_spline_skeleton_animation::set_bone_name			( bone_index_type bone_index, pcstr name )	
{
	ASSERT( m_bone_names_memory );
	ASSERT( m_bone_names );
	m_bone_names->set_name( bone_index, name );
}

void bi_spline_skeleton_animation::set_bone_names_num_bones	( u32 num )
{
	if(m_bone_names)
	{
		m_bone_names->~bone_names();
		FREE( m_bone_names_memory );
	}
	m_bone_names_memory = MALLOC( sizeof(animation::bone_names) + bone_names::count_internal_memory_size( num ), "bi_spline_skeleton_animation::m_bone_names_memory" );
	m_bone_names = new(m_bone_names_memory)animation::bone_names;
	m_bone_names->create_internals_in_place( num, sizeof(animation::bone_names) + pbyte( m_bone_names_memory ) );
}

void bi_spline_skeleton_animation::set_bone_names( const animation::bone_names &bn )
{
	size_t mem_size = bone_names::count_internal_memory_size( bn.bones_number() ) ;
	set_bone_names_num_bones( bn.bones_number() );
	memory::copy( (pbyte)m_bone_names_memory + sizeof(animation::bone_names), mem_size, bn.bone_names_idx(), mem_size );
}

void bi_spline_skeleton_animation::set_num_bones( u32 bones_count )
{
	m_bone_data.resize( bones_count );
}

#ifndef	MASTER_GOLD
void bi_spline_skeleton_animation::save( xray::configs::lua_config_value cfg )const
{
	xray::configs::lua_config_value cfg_data  = cfg["data"];
	const u32 size = m_bone_data.size();
	for ( u32 i=0; i<size; ++i )
		m_bone_data[i].save( cfg_data[i] );
///////////////////////////////////////////////////
	ASSERT( m_bone_names_memory );
	ASSERT( m_bone_names );
	xray::configs::lua_config_value temp = cfg["bone_names"];
	m_bone_names->write( temp );
	cfg["is_additive"]	= (m_animation_type == animation_type_additive);
}
#endif // #ifndef	MASTER_GOLD

void bi_spline_skeleton_animation::write					( stream &file ) const
{
	const u32 size = m_bone_data.size();
	fwrite( &(size), sizeof(size), 1, file );
	for ( u32 i=0; i<size; ++i )
		m_bone_data[i].write( file );

	ASSERT( m_bone_names_memory );
	ASSERT( m_bone_names );
	m_bone_names->write( file );
	fwrite( &m_animation_type, sizeof(m_animation_type), 1, file );
}

void bi_spline_skeleton_animation::create_event_channels	( xray::configs::binary_config_value const &cfg )
{
	ASSERT( !m_event_channels );
	size_t internal_mem_size  = animation_event_channels::count_internal_memory_size( cfg );
	m_event_channels_memory = MALLOC( internal_mem_size + sizeof(animation_event_channels), "bi_spline_skeleton_animation::event_channels" );
	m_event_channels = new( m_event_channels_memory ) animation_event_channels;
	m_event_channels->create_in_place_internals( cfg, pbyte(m_event_channels_memory) + sizeof(animation_event_channels) );
}

void bi_spline_skeleton_animation::load						( xray::configs::binary_config_value const &cfg )
{
	xray::configs::binary_config_value const cfg_data  = cfg["data"];
	const u32 size = cfg_data.size();
	m_bone_data.resize( size );
	for ( u32 i=0; i<size; ++i )
		m_bone_data[i].load( cfg_data[i] );
////////////////////////////////////////////////
	set_bone_names_num_bones( size );
	m_bone_names->read( cfg["bone_names"] );
	
	pcstr const is_additive_string = "is_additive";
	m_animation_type	= (cfg.value_exists(is_additive_string) ? cfg[is_additive_string] : false) ? animation_type_additive : animation_type_full;
}

bool bi_spline_skeleton_animation::compare					( bi_spline_skeleton_animation const & data ) const
{
	const u32 size = bones_count();
	if ( data.bones_count() != size )
		return false;

	for ( u32 i=0; i<size; ++i )
	{
		const xray::animation::bi_spline_bone_animation&d = data.bone( i );
		if ( !m_bone_data[i].compare( d ) )
			return false;
	}
	return true;
}

void bi_spline_skeleton_animation::skeleton_bone_index		( skeleton const& skeleton, vector< u32 >& idx )const
{ 
	idx.resize( skeleton.get_bones_count() );
	ASSERT( m_bone_names );
	ASSERT( m_bone_names_memory );
	m_bone_names->create_index( skeleton, idx );
}

} // namespace animation
} // namespace xray