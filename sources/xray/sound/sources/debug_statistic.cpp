////////////////////////////////////////////////////////////////////////////
//	Created		: 09.11.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "debug_statistic.h"
#include <xray/ui/text_tree_draw_helper.h>

namespace xray {
namespace sound {

void propagator_statistic::fill_text_tree	( strings::text_tree_item* item ) const
{
	fixed_string<64> temp;
	strings::text_tree_item* stats		= item->new_child( m_filename.c_str( ) );
	stats								= item->new_child( "length" );
	temp.assignf						( "%d", m_length );
	stats->add_column					( temp.c_str( ) );
	stats								= item->new_child( "current_playing_time" );
	temp.assignf						( "%d", m_current_playing_time );
	stats->add_column					( temp.c_str( ) );
	stats								= item->new_child( "playback_mode" );
	switch ( m_playback_mode )
	{
	case once:		stats->add_column	( "once" );		break;
	case looped:	stats->add_column	( "looped" );	break;
	default:		NODEFAULT			( );
	}
}

void proxy_statistic::fill_text_tree		( strings::text_tree_item* item, bool draw_propagators_stats ) const
{
		fixed_string<64> temp;
		strings::text_tree_item* stats		= item->new_child( "proxy id" );
		temp.assignf						( "%d", m_id );
		stats->add_column					( temp.c_str( ) );
		strings::text_tree_item* detail		= stats->new_child( "emitter type" );
		switch( m_emitter_type )
		{
		case single:		detail->add_column	( "single" );		break;
		case composite:		detail->add_column	( "composite" );	break;
		case collection:	detail->add_column	( "collection" );	break;
		default:			NODEFAULT		( );
		};
		detail								= stats->new_child( "sound type" );
		switch( m_sound_type )
		{
		case point:
			{
				detail->add_column			( "point" );
				detail						= stats->new_child( "position" );
				temp.assignf				( "%.2f, %.2f, %.2f", m_position[0], m_position[1], m_position[2] );
				detail->add_column			( temp.c_str( ) );
			} break;
		case cone:
			{
				detail->add_column			( "cone" );
				detail						= stats->new_child( "position" );
				temp.assignf				( "%.2f, %.2f, %.2f", m_position[0], m_position[1], m_position[2] );
				detail->add_column			( temp.c_str( ) );
				detail						= stats->new_child( "direction" );
				temp.assignf				( "%.2f, %.2f, %.2f", m_direction[0], m_direction[1], m_direction[2] );
				detail->add_column			( temp.c_str( ) );
				detail						= stats->new_child( "cone type" );
				switch ( m_cone_type )
				{
				case human:					detail->add_column( "human" );	break;
				default:					NODEFAULT( );
				};
			} break;
		case volumetric:
			{
				detail->add_column			( "volumetric" );
				detail						= stats->new_child( "position" );
				temp.assignf				( "%.2f, %.2f, %.2f", m_position[0], m_position[1], m_position[2] );
				detail->add_column			( temp.c_str( ) );
			} break;
		default:			NODEFAULT		( );
		};

		if ( !draw_propagators_stats )
			return;

		propagator_statistic* prop			= m_propagator_statistics.front( );
		while ( prop )
		{
			prop->fill_text_tree			( detail );
			prop							= m_propagator_statistics.get_next_of_object( prop );
		}
}

void debug_statistic::fill_text_tree	( strings::text_tree_item* item ) const
{
	fixed_string<64> temp;
	text_tree_item* stats				= item->new_child( "listener position" );
	temp.assignf						( "%.2f, %.2f, %.2f", m_listener_position[0], m_listener_position[1], m_listener_position[2] );
	stats->add_column					( temp.c_str( ) );
	stats								= item->new_child( "listener orient front" );
	temp.assignf						( "%.2f, %.2f, %.2f", m_listener_orient_front[0], m_listener_orient_front[1], m_listener_orient_front[2] );
	stats->add_column					( temp.c_str( ) );
	stats								= item->new_child( "listener orient top" );
	temp.assignf						( "%.2f, %.2f, %.2f", m_listener_orient_top[0], m_listener_orient_top[1], m_listener_orient_top[2] );
	stats->add_column					( temp.c_str( ) );
	
	stats								= item->new_child( "registered receivers count" );
	temp.assignf						( "%d", m_registered_receivers_count );
	stats->add_column					( temp.c_str( ) );
	stats								= item->new_child( "active voices count" );
	temp.assignf						( "%d", m_active_voices_count );
	stats->add_column					( temp.c_str( ) );
	stats								= item->new_child( "active proxies count" );
	temp.assignf						( "%d", m_active_proxies_count );
	stats->add_column					( temp.c_str( ) );

	stats								= item->new_child( "proxies:" );

	proxy_statistic* prox_stats			= m_proxies_statistic.front( );
	while ( prox_stats )
	{
		prox_stats->fill_text_tree		( item, false );
		prox_stats						= m_proxies_statistic.get_next_of_object( prox_stats );
	}
}

} // namespace sound
} // namespace xray