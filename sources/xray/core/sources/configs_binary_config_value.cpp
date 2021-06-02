////////////////////////////////////////////////////////////////////////////
//	Created		: 01.04.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/configs.h>

using xray::configs::binary_config_value;

binary_config_value::binary_config_value						( ) : 
	id		(NULL),
	data	(NULL),
	id_crc	(0),
	type	(0),
	count	(0)
{
}

static pcstr str_types[] = 
{
	"empty",
	"u32",
	"float",
	"table_named",
	"table_indexed",
	"string",
	"float2",
	"float3",
	"int2",
};

void binary_config_value::dump							(pcstr prefix) const
{
	LOG_DEBUG	(
		"%scrc=[%u] name=[%s] type=[%s] count=%d", 
		prefix ? prefix : "",
		id_crc, 
		id, 
		str_types[type],
		(type==t_table_named || type==t_table_indexed) ? count : 0
	);

	if (type==t_table_named || type==t_table_indexed)
	{
		pstr pref			= 0;
		STR_JOINA			(pref, prefix ? prefix : "", " " );

		binary_config_value::const_iterator it	= begin();
		binary_config_value::const_iterator it_e	= end();
		for(; it!=it_e; ++it)
			(*it).dump		(pref);
	}
}
#include <boost/crc.hpp>

binary_config_value const& binary_config_value::operator[]	( pcstr key ) const
{
	R_ASSERT					(type == t_table_named || type == t_table_indexed);
	const_iterator it			= begin();
	const_iterator it_e			= end();

	boost::crc_32_type			processor;
	processor.process_block		(key, key + strings::length(key));
	u32 const crc				= processor.checksum();

	const_iterator const result	= std::lower_bound(it, it_e, crc);
	
	R_ASSERT					(result!=it_e, "item not found [%s][%s]", id, key );
	R_ASSERT					(result->id_crc==crc, "item not found  [%s][%s]", id, key );

	do {
		if ( strings::equal( key, result->id ) )
			return				*result;
	} while ( result->id_crc == crc );

	UNREACHABLE_CODE			( return *result );
}

bool binary_config_value::value_exists					( pcstr key ) const
{
	R_ASSERT					(type == t_table_named || type == t_table_indexed);
	const_iterator it			= begin();
	const_iterator it_e			= end();

	boost::crc_32_type			processor;
	processor.process_block		(key, key + strings::length(key));
	u32 const crc				= processor.checksum();

	const_iterator const result	= std::lower_bound(it, it_e, crc);
	if ( result == it_e)
		return					false;

	if ( result->id_crc != crc )
		return					false;

	return						strings::equal( key, result->id );
}

void binary_config_value::fix_up						( size_t const offset )
{
	if ( id )
		id						+= offset;

	switch ( type ) {
		case t_boolean :
		case t_integer :
		case t_float : {
			break;
		}
		case t_string :
		case t_float2 :
		case t_float3 :
		case t_float4 :
		case t_int2 : {
			(pcbyte&)(data)		+= offset;
			break;
		}
		case t_table_indexed :
		case t_table_named : {
			(pcbyte&)(data)		+= offset;

			for ( u32 i=0, n=count; i < n; ++i )
				((binary_config_value*)(static_cast<void const*>(data)) + i)->fix_up( offset );

			break;
		}
		default : NODEFAULT();
	}
}