////////////////////////////////////////////////////////////////////////////
//	Created		: 05.08.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "composite_sound_item.h"
#include "sound_collection_item.h"

using xray::sound_editor::composite_sound_item;
using xray::sound_editor::sound_collection_item;

composite_sound_item::composite_sound_item()
{
	m_playing_offset_min	= 0;
	m_playing_offset_max	= 0;
	is_random				= false;
}

composite_sound_item::~composite_sound_item()
{
}

void composite_sound_item::save(xray::configs::lua_config_value& cfg)
{
	sound_collection_item::save(cfg);

	if ( offset_type == "random" )
	{
		cfg["offset_type"]			= unmanaged_string( offset_type ).c_str( );
		cfg["playing_offset_min"]	= m_playing_offset_min;
		cfg["playing_offset_max"]	= m_playing_offset_max;
	}
	else
	{
		cfg["offset_type"]			= unmanaged_string( offset_type ).c_str( );
		cfg["playing_offset"]		= m_playing_offset_min;
	}
}

void composite_sound_item::load(xray::configs::lua_config_value const& cfg)
{
	sound_collection_item::load(cfg);

	if ( cfg.value_exists( "offset_type" ) )
	{
		String^ of= gcnew String( cfg["offset_type"] );
		if ( of	== "random" )
		{
			playing_offset_min		= cfg["playing_offset_min"];
			playing_offset_max		= cfg["playing_offset_max"];
		}
		else
		{
			playing_offset_min		= cfg["playing_offset"];
			playing_offset_max		= cfg["playing_offset"];
		}
	}
	else
	{
		playing_offset_min			= 0;
		playing_offset_max			= 0;
	}
}