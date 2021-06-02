////////////////////////////////////////////////////////////////////////////
//	Created		: 19.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef LUA_PROPERTY_VALUE_H_INCLUDED
#define LUA_PROPERTY_VALUE_H_INCLUDED

using namespace System;

template<typename TConfigType, typename TValueType>
//generic<typename TValueType>
public interface class i_lua_value_converter
{
	TValueType	convert			( TConfigType );
	TConfigType convert_back	( TValueType );
};// interface class i_lua_value_converter

template<typename TConfigType, typename TValueType>
//generic<typename TValueType>
public ref class lua_converted_property_value: xray::editor::wpf_controls::i_property_value
{
public:
	lua_converted_property_value( const xray::configs::lua_config_value& config_value, String^ property_path, i_lua_value_converter<TConfigType,TValueType>^ converter  )
	{
		m_config_value  = CRT_NEW(xray::configs::lua_config_value)(config_value);
		m_path_steps	= property_path->Split('/');
		m_converter		= converter;
	}
	
private:
	xray::configs::lua_config_value*	m_config_value;
	array<String^>^				m_path_steps;
	i_lua_value_converter<TConfigType,TValueType>^ m_converter;

private:
	xray::configs::lua_config_value config_value( )
	{
		xray::configs::lua_config_value return_value = *m_config_value;

		for each( String^ path_step in m_path_steps )
			return_value = return_value[xray::editor_base::unmanaged_string( path_step ).c_str( )];

		return return_value;
	}

public:
	Object^			get_value			( )
	{
		return converter->convert( (TConfigType)config_value( ) );
	}
	void			set_value			( Object^ value )
	{
		config_value( ) = converter->convert_back( (TValueType)value );
	}
	virtual property System::Type^			value_type
	{
		System::Type^ get() {return TValueType::typeid;}
	}
}; // class lua_property_value

//typedef lua_property_value<int>		lua_property_value_int;
//typedef lua_property_value<float>	lua_property_value_float;
//typedef lua_property_value<bool>	lua_property_value_bool;


#endif // #ifndef LUA_PROPERTY_VALUE_H_INCLUDED