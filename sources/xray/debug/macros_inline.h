////////////////////////////////////////////////////////////////////////////
//	Created		: 18.06.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DEBUG_MACROS_INLINE_H_INCLUDED
#define XRAY_DEBUG_MACROS_INLINE_H_INCLUDED

#include <xray/debug/api.h>
#include <stdarg.h>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_float.hpp>
#include <boost/type_traits/is_enum.hpp>

namespace xray {
namespace debug {
namespace detail {

inline
void	expression_eater	(...) {}

class XRAY_DEBUG_API string_helper
{
public:
	string_helper () { m_buffer[0] = 0; }

	pcstr	c_str () const		{ return m_buffer; }
	u32		size () const;
	void	appendf				(pcstr format, ...);
	void	appendf				() {}
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
		result.appendf							("(left = %s", value1_string.c_str());
	}

	string_helper	value2_string			=	to_string(value2);
	if ( value2_string.size() )
	{
		result.appendf							(", right = %s)", value2_string.c_str());
	}

	if ( result.size() )
	{
		result.appendf							("%s", " ");
	}

	return										result;
}

template < typename T1, typename T2 >
struct tuple {
	T1 left;
	T2 right;
}; // struct tuple

template < typename T1, typename T2 >
tuple<T1, T1> make_tuple( T1 const& left, T2 const& right )
{
	tuple<T1, T1>	result;
	result.left		= left;
	result.right	= (boost::remove_cv<T1>::type)(right);
	return			result;
}

template < typename T >
tuple<T, T> make_tuple( T const& left, T const& right )
{
	tuple<T, T>		result;
	result.left		= left;
	result.right	= right;
	return			result;
}

struct compare_helper { };

template < typename T1, typename T2 >
struct compare_result {
	inline compare_result	( tuple<T1, T2> const& operands, bool const result ) :
		operands	( operands ),
		result		( result )
	{
	}

	tuple<T1, T2>	operands;
	bool			result;
}; // struct compare_result

template < typename T1, typename T2 >
string_helper make_fail_message( bool& comparison_result, compare_result<T1, T2> const& operands_and_result )
{
	comparison_result							= operands_and_result.result;
	if ( !operands_and_result.result )
		return
			make_fail_message(
				operands_and_result.operands.left,
				operands_and_result.operands.right
			);

	return										string_helper( );
}

template < typename T1, typename T2 >
inline void make_fail_message( string_helper * out_helper, bool& comparison_result, compare_result<T1, T2> const& operands_and_result )
{
	if ( operands_and_result.result ) {
		comparison_result						= true;
		return;
	}

	* out_helper						= make_fail_message( comparison_result, operands_and_result);
	return;
}

#ifdef XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP
#	error do not define macro XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP
#endif // #ifdef XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP

#define XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP( comparison_operator )	\
	template < typename T1, typename T2 >											\
	inline compare_result<T1, T2> operator comparison_operator (					\
			tuple<T1, T2> const& left,												\
			compare_helper const& right												\
		)																			\
	{																				\
		XRAY_UNREFERENCED_PARAMETER	( right );										\
		return																		\
			compare_result<T1, T2>	(												\
				left,																\
				left.left comparison_operator left.right							\
			);																		\
	}

XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP( == )
XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP( != )
XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP( < )
XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP( <= )
XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP( > )
XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP( >= )
XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP( || )
XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP( && )
XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP( | )
XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP( & )
XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP( ^ )

#ifdef XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP
#	undef XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP
#else // #ifdef XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP
#	error do not undefine macro XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP
#endif // #ifdef XRAY_DEFINE_COMPARISON_OPERATOR_FOR__R_ASSERT_CMP

} // namespace detail
} // namespace debug
} // namespace xray

#endif // #ifndef XRAY_DEBUG_MACROS_INLINE_H_INCLUDED