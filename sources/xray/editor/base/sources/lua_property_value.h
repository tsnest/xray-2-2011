////////////////////////////////////////////////////////////////////////////
//	Created		: 19.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef LUA_PROPERTY_VALUE_H_INCLUDED
#define LUA_PROPERTY_VALUE_H_INCLUDED

using namespace System;


template<typename TConfigType>
public ref class lua_property_value: xray::editor::wpf_controls::i_property_value
{
internal:
	lua_property_value( )
	{
	}
public:
	lua_property_value( const xray::configs::lua_config_value& config_value, String^ property_path )
	{
		m_config_value  = CRT_NEW(xray::configs::lua_config_value)(config_value);
		m_path_steps	= property_path->Split('/');
	}
	
private:
	xray::configs::lua_config_value*	m_config_value;
	array<String^>^				m_path_steps;

private:
	xray::configs::lua_config_value config_value( )
	{
		xray::configs::lua_config_value return_value = *m_config_value;

		if( m_path_steps == nullptr || ( m_path_steps->Length == 1 && m_path_steps[0] == "" ))
			return return_value;

		for each( String^ path_step in m_path_steps )
			return_value = return_value[ xray::editor_base::unmanaged_string( path_step ).c_str( )];

		return return_value;
		
	}

public:
	virtual Object^			get_value			( )
	{
		return (TConfigType)config_value( );
	}
	virtual void			set_value			( Object^ value )
	{
		config_value( ) = (TConfigType)value;

		ASSERT( (TConfigType)value == (TConfigType)config_value( ) );
	}
	virtual property System::Type^		value_type
	{
		System::Type^ get() {return TConfigType::typeid;}
	}
}; // class lua_property_value

public ref class lua_property_value_int		: lua_property_value<int>
{
public:
	lua_property_value_int( const xray::configs::lua_config_value& config_value, String^ property_path ):
		lua_property_value<int>( config_value, property_path ){};
};
public ref class lua_property_value_float	: lua_property_value<float>
{
public:
	lua_property_value_float( const xray::configs::lua_config_value& config_value, String^ property_path ):
		lua_property_value<float>( config_value, property_path ){};
};
public ref class lua_property_value_bool	: lua_property_value<bool>
{
public:
	lua_property_value_bool( const xray::configs::lua_config_value& config_value, String^ property_path ):
		lua_property_value<bool>( config_value, property_path ){};
};

	
#endif // #ifndef LUA_PROPERTY_VALUE_H_INCLUDED