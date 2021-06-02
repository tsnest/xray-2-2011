////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include <xray/console_command.h>
#include <xray/console_command_processor.h>

namespace xray {
namespace console_commands{

console_command* s_console_command_root = NULL;

console_command::console_command( pcstr name, bool serializable, command_type const command_type, execution_filter const execution_filter )
:m_name				( name ), 
m_prev				( s_console_command_root ), 
m_next				( NULL ), 
m_need_args			( false ), 
m_serializable		( serializable ),
m_command_type		( command_type ),
m_execution_type	( execution_filter )
{
	//make it thread-safe
	if(m_prev)
		m_prev->m_next		= this;

	s_console_command_root	= this;
}

console_command::~console_command( )
{
}

void console_command::on_changed( pcstr args )
{
	if( m_on_change_event )
		m_on_change_event ( args );
}

void console_command::status( status_str& dest ) const
{
	strings::copy(dest, "unknown");
}

void console_command::info( info_str& dest ) const
{
	strings::copy(dest, "(no info)");
}

void console_command::syntax( syntax_str& dest ) const
{
	strings::copy(dest, "(no arguments)");
}

void console_command::on_invalid_syntax( pcstr args )
{
	syntax_str	buff; 
	syntax		(buff);
	LOG_WARNING	("Invalid syntax in call [%s %s]", name(), args);
	LOG_WARNING	("Valid arguments: %s", buff);
}

void console_command::save_to( console_commands::save_storage& f, memory::base_allocator* a ) const
{
	XRAY_UNREFERENCED_PARAMETERS	( a );
	console_command::status_str	buff;
	status			( buff );
	pcstr out_str	= NULL;
	STR_JOINA		( out_str, name(), " ", buff );
	f.add_line		( out_str );
//	f.w_stringZ_CRLF( out_str );
}

//--delegate
cc_delegate::cc_delegate( pcstr name, functor_type const& functor, bool need_args ) 
:	super		( name, false, command_type_user_specific, execution_filter_general ), 
	m_functor	( functor )
{
	m_need_args = need_args;
}

void cc_delegate::info( info_str& dest ) const
{
	strings::copy(dest, "function call");
};

void cc_delegate::execute( pcstr args )
{
	m_functor			( args );
	super::on_changed	( args );
};

//--string
cc_string::cc_string( pcstr name, pstr value, u32 size, bool serializable, command_type const command_type, execution_filter const execution_filter )
:super		( name, serializable, command_type, execution_filter ), 
m_value		( value ), 
m_size		( size )
{
	m_need_args		= true;
}

void cc_string::execute( pcstr args )
{
	strings::copy_n		( m_value, m_size, args, m_size );
	super::on_changed	( args );
}

void cc_string::status( status_str& dest ) const
{
	strings::copy	( dest, m_value );
}

void cc_string::syntax( syntax_str& dest ) const
{
	sprintf	(dest, "max size is %d", m_size);
}

void cc_string::info( info_str& dest ) const
{
	sprintf	(dest, "string value.");
}

//--bool
cc_bool::cc_bool( pcstr name, bool& value, bool serializable, command_type const command_type, execution_filter const execution_filter )
:super		( name, serializable, command_type, execution_filter ), 
m_value		( value )
{
	m_need_args		= true;
}

void cc_bool::execute( pcstr args )
{
	bool v;
	if(strings::equal(args, "on") ||strings::equal(args, "true") ||strings::equal(args, "1"))
		v	= true;
	else
	if(strings::equal(args, "off") ||strings::equal(args, "false") ||strings::equal(args, "0"))
		v	= false;
	else{
		on_invalid_syntax( args );
		return;
	}
	m_value				= v;
	super::on_changed	( args );
}

pcstr bool_values_str[]=
{"on","off"};

void cc_bool::status( status_str& dest ) const
{
	sprintf		( dest, "%s", m_value ? "on" : "off" );
}

void cc_bool::syntax( syntax_str& dest ) const
{
	sprintf	(dest, "on/off, true/false, 1/0");
}

void cc_bool::info( info_str& dest ) const
{
	sprintf	(dest, "boolean value.");
}

void cc_bool::fill_command_args_list( vectora<pcstr>& dest ) const
{
	dest.clear();
	dest.push_back(bool_values_str[0]);
	dest.push_back(bool_values_str[1]);
}

//--float
cc_float::cc_float( pcstr name, float& value, float const min, float const max, bool serializable, command_type const command_type, execution_filter const execution_filter )
:super(name, value, min, max, serializable, command_type, execution_filter)
{}

void cc_float::execute( pcstr args )
{
	float v;
	if(1!=XRAY_SSCANF(args, "%f", &v) || v<m_min || v>m_max)
	{
		on_invalid_syntax	( args );
		v		= m_min;
	}else
	{
		m_value = v;
	}

	super::on_changed	( args );
}

static void trim_float_str( console_command::status_str& dest )
{
	pstr p = dest+xray::strings::length(dest)-1;
	while(*p=='0' && *(p-1)=='0')
	{
		*p = 0;
		--p;
	}
}

void cc_float::status( status_str& dest ) const
{
	sprintf			( dest, "%3.5f", m_value );
	trim_float_str	( dest );
}

void cc_float::info( info_str& dest ) const
{
	sprintf	(dest, "floating point value.");
}

void cc_float::syntax( syntax_str& dest ) const
{
	sprintf	(dest, "range [%3.3f,%3.3f]", m_min, m_max);
}
//--u32
cc_u32::cc_u32( pcstr name, u32& value, u32 const min, u32 const max, bool serializable, command_type const command_type, execution_filter const execution_filter )
:super(name, value, min, max, serializable, command_type, execution_filter)
{}

void cc_u32::execute( pcstr args )
{
	u32 v;
	if(1!=XRAY_SSCANF(args, "%d", &v) || v<m_min || v>m_max)
	{
		on_invalid_syntax(args);
		v = m_min;
	}else
	{
		m_value = v;
	}

	super::on_changed	( args );
}

void cc_u32::status( status_str& dest ) const
{
	sprintf		( dest, "%d", m_value );
}

void cc_u32::info( info_str& dest ) const
{
	sprintf	(dest, "unsigned integer value.");
}

void cc_u32::syntax( syntax_str& dest ) const
{
	sprintf	(dest, "range [%d,%d]", m_min, m_max);
}
//--float2
cc_float2::cc_float2( pcstr name, math::float2& value, math::float2 const min, math::float2 const max, bool serializable, command_type const command_type, execution_filter const execution_filter )
:super(name, value, min, max, serializable, command_type, execution_filter)
{}

void cc_float2::execute( pcstr args )
{
	math::float2 v;
	if( 2!=XRAY_SSCANF(args, "%f,%f", &v.x, &v.y) || 
		(v.x<m_min.x || v.y<m_min.y || v.x>m_max.x || v.y>m_max.y) )
	{
		on_invalid_syntax	( args );
		v		= m_min;
	}else
	{
		m_value = v;
	}

	super::on_changed	( args );
}

void cc_float2::status( status_str& dest ) const
{
	sprintf		( dest, "%3.5f,%3.5f", m_value.x, m_value.y );
}

void cc_float2::info( info_str& dest ) const
{
	sprintf	(dest, "float2 value.");
}

void cc_float2::syntax( syntax_str& dest ) const
{
	sprintf	(dest, "range [%3.3f,%3.3f]-[%3.3f,%3.3f]", m_min.x, m_min.y, m_max.x, m_max.y);
}

//--float3
cc_float3::cc_float3( pcstr name, math::float3& value, math::float3 const min, math::float3 const max, bool serializable, command_type const command_type, execution_filter const execution_filter )
:super(name, value, min, max, serializable, command_type, execution_filter)
{}

void cc_float3::execute( pcstr args )
{
	math::float3 v;
	if( 3!=XRAY_SSCANF(args, "%f,%f,%f", &v.x, &v.y, &v.z) || 
		(v.x<m_min.x || v.y<m_min.y || v.z<m_min.z || v.x>m_max.x || v.y>m_max.y || v.y>m_max.z) )
	{
		on_invalid_syntax( args );
		v			= m_min;
	}else
	{
		m_value = v;
	}

	super::on_changed	( args );
}

void cc_float3::status( status_str& dest ) const
{
	sprintf		( dest, "%3.5f,%3.5f,%3.5f", m_value.x, m_value.y, m_value.z );
}

void cc_float3::info( info_str& dest ) const
{
	sprintf	(dest, "float3 value.");
}

void cc_float3::syntax( syntax_str& dest ) const
{
	sprintf	(dest, "range [%3.3f,%3.3f,%3.3f]-[%3.3f,%3.3f,%3.3f]", m_min.x, m_min.y, m_min.z, m_max.x, m_max.y, m_max.z );
}

// --cc_token
cc_token::cc_token( pcstr name, u32 &value, const command_token commands[ ], u32 size, bool serializable, command_type const command_type, execution_filter const execution_filter )
:super(	name, serializable, command_type, execution_filter ),
m_commands( commands ),
m_num_commands( size ),
m_value( value ) 
{
} 

u32 cc_token::find_id( pcstr args )const
{
	if ( !args )
		return u32(-1);
	
	u32 size  = num_commands	();

	for( u32 it = 0; it < size; ++it )
		if( strings::equal(m_commands[it].name, args) )
			return  m_commands[it].id;
	return u32(-1);
}


pcstr cc_token::find_name( u32 id )const
{
	u32 size  = num_commands	();

	for( u32 it = 0; it < size; ++it )
		if( m_commands[it].id == id )
			return  m_commands[it].name;

	return 0;
}

void cc_token::all_methods_names ( string512 names )const
{
	
	names[0] = '\0';
	
	u32 size = num_commands();

	for( u32 it = 0; it < size; ++it )
		strings::join( names, sizeof(string512), names, m_commands[it].name, ", " );
}

void cc_token::execute( pcstr args )
{
	u32 id = find_id( args );
	
	if( id == u32(-1) )
	{
		on_invalid_syntax( args );
	}else
	{
		m_value = id;
	}

	super::on_changed	( args );
}

void cc_token::status( status_str& dest ) const
{
	pcstr name = find_name( m_value );
	if( !name )
			return;

	strings::copy( dest, name );
} 

void cc_token::info( info_str& dest ) const
{
	all_methods_names( dest );
}

void cc_token::syntax( syntax_str& dest ) const
{
	all_methods_names( dest );
}

void cc_token::fill_command_args_list( vectora<pcstr>& dest ) const
{
	dest.clear();

	for(u32 i=0; i<m_num_commands; ++i)
		dest.push_back(m_commands[i].name);
}



class cc_help :public console_command
{
public:
						cc_help					(pcstr name):console_command(name, false, command_type_user_specific, execution_filter_general){}
	virtual void		execute					(pcstr args);
	virtual void		info					(info_str& dest) const 
	{
		xray::strings::copy(dest, "[command] - displays help information on that command.");
	}
};

void cc_help::execute(pcstr args)
{
	xray::console_commands::show_help(args);
}

static cc_help	s_help_cmd("help");

} // namespace xray
} // namespace console_commands
