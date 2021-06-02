////////////////////////////////////////////////////////////////////////////
//	Created		: 13.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CONSOLE_COMMAND_INLINE_H_INCLUDED
#define CONSOLE_COMMAND_INLINE_H_INCLUDED

namespace xray {
namespace console_commands{

template<class T>
inline cc_value<T>::cc_value(pcstr name, T& value, T const min, T const max, bool serializable, command_type const command_type, execution_filter const execution_filter)
:super		( name, serializable, command_type, execution_filter ), 
m_value		( value ), 
m_min		( min ), 
m_max		( max )
{
	m_need_args		= true;
}


} // namespace console_commands
} // namespace xray

#endif // #ifndef CONSOLE_COMMAND_INLINE_H_INCLUDED