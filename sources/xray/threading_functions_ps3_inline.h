////////////////////////////////////////////////////////////////////////////
//	Created		: 18.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_THREADING_FUNCTIONS_PS3_INLINE_H_INCLUDED
#define XRAY_THREADING_FUNCTIONS_PS3_INLINE_H_INCLUDED

#include <cell/atomic.h>

namespace xray {
namespace threading {

XRAY_THREADING_INLINE atomic32_value_type interlocked_increment			( atomic32_type& value )
{
	return				cellAtomicIncr32( (u32*)&value ) + 1;
}

XRAY_THREADING_INLINE atomic32_value_type interlocked_decrement			( atomic32_type& value )
{
	return				cellAtomicDecr32( &value ) - 1;
}

XRAY_THREADING_INLINE atomic32_value_type interlocked_exchange_add		( atomic32_type& value, atomic32_value_type increment )
{
	return				cellAtomicAdd32( &value, increment );
}

XRAY_THREADING_INLINE atomic32_value_type	interlocked_exchange_sub	( atomic32_type& value,  atomic32_value_type  subtract )
{
	return				cellAtomicSub32( &value, -subtract );
}

XRAY_THREADING_INLINE atomic32_value_type interlocked_and				( atomic32_type& value, atomic32_value_type mask )
{
	return				cellAtomicAnd32( &value, mask );
}

XRAY_THREADING_INLINE atomic32_value_type interlocked_or				( atomic32_type& value, atomic32_value_type mask )
{
	return				cellAtomicOr32( &value, mask );
}

XRAY_THREADING_INLINE atomic32_value_type interlocked_exchange			( atomic32_type& target, atomic32_value_type value )
{
	return				cellAtomicStore32( &target, value );
}

XRAY_THREADING_INLINE pvoid interlocked_exchange_pointer				( atomic_ptr_type& target, pvoid value )
{
	atomic32_value_type const result	= cellAtomicStore32( (u32*)&target, *(u32*)&value );
	return				*(pvoid*)&result;
}

XRAY_THREADING_INLINE atomic32_value_type interlocked_compare_exchange	( atomic32_type& target, atomic32_value_type exchange, atomic32_value_type comparand )
{
	return				cellAtomicCompareAndSwap32( &target, exchange, comparand);
}

XRAY_THREADING_INLINE atomic64_value_type interlocked_compare_exchange	( atomic64_type& target, atomic64_value_type exchange, atomic64_value_type comparand )
{
	return				cellAtomicCompareAndSwap64((atomic64_value_type*)&target, exchange, comparand);
}

XRAY_THREADING_INLINE atomic64_value_type interlocked_exchange			( atomic64_type& target, atomic64_value_type exchange )
{
	return				cellAtomicStore64(&target, exchange);
}

//XRAY_THREADING_INLINE atomic64_value_type interlocked_exchange_add				( atomic64_type& target, atomic64_value_type exchange )
//{
//	return				cellAtomicAdd64(&target, exchange);
//}

XRAY_THREADING_INLINE pvoid interlocked_compare_exchange_pointer		( atomic_ptr_type& target, pvoid const exchange, pvoid const comparand )
{
	atomic32_value_type const result	= cellAtomicCompareAndSwap32( (atomic32_value_type*)&target, *(u32 const*)&comparand, *(u32 const*)&exchange );
	return				*(atomic_ptr_value_type*)&result;
}

} // namespace threading
} // namespace xray

#endif // #ifndef XRAY_THREADING_FUNCTIONS_PS3_INLINE_H_INCLUDED