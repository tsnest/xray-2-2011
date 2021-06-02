////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/editable_animation_event_channels.h>

namespace xray {
namespace animation {

	editable_animation_event_channels::editable_animation_event_channels( animation_event_channels const& source ):
	m_event_channels_memory( 0 ),
	m_event_channels( 0 ),
	m_dirty( true )
	{
		m_config = configs::create_lua_config( );
		configs::lua_config_value cfg = config_value( );
		source.write( cfg );
		create_channels_from_config( );
	}

	editable_animation_event_channels::~editable_animation_event_channels( )
	{
		m_event_channels->~animation_event_channels	( );
		m_event_channels = 0;
		XRAY_FREE_IMPL	( debug::g_mt_allocator, m_event_channels_memory );
	}

	animation_event_channels const&	editable_animation_event_channels::channels	( )
	{
		create_channels_from_config();
		return *m_event_channels;
	}

	configs::lua_config_value editable_animation_event_channels::config_value( ) const
	{ 
		m_dirty = true;
		return m_config->get_root( )["data"]["event_channels"]; 
	}

	void editable_animation_event_channels::create_channels_from_config()
	{
		if ( !m_dirty )
			return;

		size_t internal_mem_size  = animation_event_channels::count_internal_memory_size( config_value( ) );
		if ( m_event_channels )
			m_event_channels->~animation_event_channels	( );
		m_event_channels_memory = XRAY_REALLOC_IMPL( debug::g_mt_allocator, m_event_channels_memory, internal_mem_size + sizeof( animation_event_channels ) , "editable_animation_event_channels::event_channels" );
		new ( m_event_channels_memory ) animation_event_channels;
		m_event_channels = (animation_event_channels*)m_event_channels_memory;
		m_event_channels->create_in_place_internals( config_value( ), pbyte(m_event_channels_memory) + sizeof( animation_event_channels ) );
		
		m_dirty = false;

	}
	
	void editable_animation_event_channels::save( pcstr path ) const
	{
		ASSERT( m_config );
		m_config->save_as( path, configs::target_sources );
	}

} // namespace animation
} // namespace xray