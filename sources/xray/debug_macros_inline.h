////////////////////////////////////////////////////////////////////////////
//	Created		: 18.06.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DEBUG_MACROS_INLINE_H_INCLUDED
#define XRAY_DEBUG_MACROS_INLINE_H_INCLUDED

#include <stdarg.h>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_float.hpp>
#include <boost/type_traits/is_enum.hpp>
namespace xray {
namespace debug {
namespace detail {

inline
void	expression_eater	(...) {}

class XRAY_CORE_API string_helper
{
public:
	string_helper () { m_buffer[0] = 0; }

	pcstr	c_str () const		{ return m_buffer; }
	u32		size () const;
	void	appendf				(pcstr format, ...);
	void	appendf_va_list		(pcstr const format, va_list argptr);

private:
	char	m_buffer[4096];
};

enum to_string_flag_enum		{	to_string_flag_unknown,	
									to_string_flag_integer, 
									to_string_flag_float		};

template <class T, to_string_flag_enum flag>
struct to_string_helper {
	static inline string_helper	convert (T const value)
	{
//!		COMPILE_ASSERT	( false, unknown_type );
		XRAY_UNREFERENCED_PARAMETER	( value );
		return			string_helper( );
	}
}; // class to_string_helper

template <class T>
struct to_string_helper<T,to_string_flag_integer> {
	static inline string_helper	convert (T const value)
	{
		COMPILE_ASSERT	(boost::is_integral<T>::value || boost::is_enum<T>::value, wrong_type_T);
		string_helper	string;
		s64				value64	=	(s64)value;
		string.appendf	("%I64i", value64);
		return			string;
	}
}; // class to_string_helper

template <class T>
struct to_string_helper<T,to_string_flag_float> {
	static inline string_helper	convert (T const value)
	{
		COMPILE_ASSERT	(boost::is_float<T>::value, wrong_type_T);
		string_helper	string;
		string.appendf	("%f", double(value));
		return			string;
	}
}; // class to_string_helper

template <class T>
string_helper		to_string (T const & value)
{
	return		to_string_helper<T, (boost::is_integral<T>::value || boost::is_enum<T>::value) ? to_string_flag_integer:
		(boost::is_float<T>::value ? to_string_flag_float : to_string_flag_unknown)	>::convert(value);
}

template <class T1, class T2>
string_helper   make_fail_message (T1 const & value1, T2 const & value2)
{
	string_helper								result;
	string_helper	value1_string			=	to_string(value1);
	if ( value1_string.size() )
	{
		result.appendf							("(first_arg = %s", value1_string.c_str());
	}

	string_helper	value2_string			=	to_string(value2);
	if ( value2_string.size() )
	{
		result.appendf							(", second_arg = %s)", value2_string.c_str());
	}

	if ( result.size() )
	{
		result.appendf							("%s", " ");
	}

	return										result;
}

template <class T1, class T2>
string_helper   make_fail_message (T1 const & value1, T2 const & value2, pcstr format, ...)
{
	string_helper	result					=	make_fail_message(value1, value2);
	va_list										argptr;
	va_start 									(argptr, format);
	result.appendf_va_list						(format, argptr);
	va_end	 									(argptr);
	return										result;
}

} // namespace detail
} // namespace debug
} // namespace xray

#endif // #ifndef XRAY_DEBUG_MACROS_INLINE_H_INCLUDED
