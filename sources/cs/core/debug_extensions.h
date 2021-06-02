////////////////////////////////////////////////////////////////////////////
//	Module 		: debug_extensions.h
//	Created 	: 27.08.2006
//  Modified 	: 27.08.2006
//	Author		: Dmitriy Iassenev
//	Description : debug extensions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_DEBUG_EXTENSIONS_H_INCLUDED
#define CS_CORE_DEBUG_EXTENSIONS_H_INCLUDED

#include <boost/static_assert.hpp>
#include <cs/core/debug.h>
#include <float.h>

inline bool	valid			(float const value)
{
	int const	value_bits = horrible_cast<float,int>(value).second;

	// +infinity, -infinity, +SNaN, -SNaN, +QNaN, -QNaN
	if ( (value_bits & 0x7f800000) == 0x7f800000 )
		return	( false );

	// +denormal, -denormal
	if ( ( ( (value_bits & 0x7f800000) == 0 ) ) && ( ( value_bits & 0x007fffff ) != 0) )
		return	( false );

	return		( true );
}

#define STRINGIZE_HELPER(x)	#x
#define STRINGIZE(x)		STRINGIZE_HELPER(x)
#define __FILE__LINE__		__FILE__ "(" STRINGIZE(__LINE__) ") : "
#define TODO(x)				message( __FILE__LINE__" TODO :   " #x "\n")

#endif // #ifndef CS_CORE_DEBUG_EXTENSIONS_H_INCLUDED