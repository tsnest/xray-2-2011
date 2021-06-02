////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef THREADING_FUNCTIONS_GUARD_H_INCLUDED
#define THREADING_FUNCTIONS_GUARD_H_INCLUDED

namespace xray {
namespace threading {

// interlocked functions
template <class T>
inline	atomic32_value_type	interlocked_increment			( T & )
{
	COMPILE_ASSERT	( false, do_not_pass_NON_VOLATILE_values_to_INTERLOCKED_functions );
	return			atomic32_value_type(-1);
}

template <class T>
inline	atomic32_value_type	interlocked_decrement			( T & )
{
	COMPILE_ASSERT	( false, do_not_pass_NON_VOLATILE_values_to_INTERLOCKED_functions );
	return			atomic32_value_type(-1);
}

template <class T>
inline	atomic32_value_type	interlocked_exchange_add		( T & , T )
{
	COMPILE_ASSERT	( false, do_not_pass_NON_VOLATILE_values_to_INTERLOCKED_functions );
	return			atomic32_value_type(-1);
}

template <class T>
inline	atomic32_value_type	interlocked_exchange_sub		( T & , T )
{
	COMPILE_ASSERT	( false, do_not_pass_NON_VOLATILE_values_to_INTERLOCKED_functions );
	return			atomic32_value_type(-1);
}

template <class T>
inline	atomic32_value_type	interlocked_and					( T & , T )
{
	COMPILE_ASSERT	( false, do_not_pass_NON_VOLATILE_values_to_INTERLOCKED_functions );
	return			atomic32_value_type(-1);
}

template <class T>
inline	atomic32_value_type	interlocked_or					( T & , T )
{
	COMPILE_ASSERT	( false, do_not_pass_NON_VOLATILE_values_to_INTERLOCKED_functions );
	return			atomic32_value_type(-1);
}

template <class T>
inline	atomic32_value_type	interlocked_exchange			( T & , T )
{
	COMPILE_ASSERT	( false, do_not_pass_NON_VOLATILE_values_to_INTERLOCKED_functions );
	return			atomic32_value_type(-1);
}

#if !XRAY_PLATFORM_PS3
template <class T>
inline	pvoid	interlocked_exchange_pointer				( T & , T )
{
	COMPILE_ASSERT	( false, do_not_pass_NON_VOLATILE_values_to_INTERLOCKED_functions );
	return			0;
}
#endif // #if !XRAY_PLATFORM_PS3

template <class T>
inline	atomic32_value_type	interlocked_compare_exchange	( atomic32_value_type & , T , T )
{
	COMPILE_ASSERT	( false, do_not_pass_NON_VOLATILE_values_to_INTERLOCKED_functions );
	return			atomic32_value_type(-1);
}

template <class T>
inline	atomic64_value_type	interlocked_compare_exchange	( atomic64_value_type &, T , T )
{
	COMPILE_ASSERT	( false, do_not_pass_NON_VOLATILE_values_to_INTERLOCKED_functions );
	return			atomic64_value_type(-1);
}

template <class T>
inline	pvoid	interlocked_compare_exchange_pointer		( T & , T , T )
{
	COMPILE_ASSERT	( false, do_not_pass_NON_VOLATILE_values_to_INTERLOCKED_functions );
	return			0;
}

} // namespace threading
} // namespace xray

#endif // #ifndef THREADING_FUNCTIONS_GUARD_H_INCLUDED