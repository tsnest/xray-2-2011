////////////////////////////////////////////////////////////////////////////
//	Created		: 01.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef CONSTANT_BUFFER_INLINE_H_INCLUDED
#define CONSTANT_BUFFER_INLINE_H_INCLUDED

#include "constant_defines.h"

namespace xray {
namespace render_dx10 {

void res_constant_buffer::set_memory( const u32 offset, char const * src_ptr, const u32 size)
{
	ASSERT( (int)size <= ((int)m_buffer_size - offset) );
	char const * const end = (char*)m_buffer_data + offset + (((int)size <= ( (int)m_buffer_size - offset)) ? size : ((int) m_buffer_size - offset)) ;

	u8 diff = 0;
	for( char* ptr = (char*)m_buffer_data + offset; ptr != end; ++ptr, ++src_ptr)
	{
		diff |= (*ptr)^(*src_ptr);
		*ptr = *src_ptr;
	}

	m_changed |= (diff>0);
}

void res_constant_buffer::zero_memory( const u32 offset, const u32 size)
{
	ASSERT( (int)size <= ((int)m_buffer_size - offset) );
	char* const end = (char*)m_buffer_data + offset + (((int)size <= ( (int)m_buffer_size - offset)) ? size : ((int) m_buffer_size - offset)) ;

	u8 diff = 0;
	for( char* ptr = (char*)m_buffer_data + offset; ptr != end; ++ptr)
	{
		diff |= (*ptr)^(0);
		*ptr = 0;
	}

	m_changed |= (diff>0);
}

void* res_constant_buffer::access( u32 offset)
{
	ASSERT( offset < m_buffer_size);

	return( offset < m_buffer_size) ? ((char*)m_buffer_data) + offset : 0;
}

void res_constant_buffer::set		( constant_slot const& slot, void* ptr, u32 size)
{
	u32 data_size = slot.cls&constant_class_size_mask;
	ASSERT_U( size >= data_size);
	set_memory( slot.slot_index*line_size, (char*)ptr, data_size);
}

void res_constant_buffer::zero	( constant_slot const& slot, u32 size)
{
	u32 data_size = slot.cls&constant_class_size_mask;
	ASSERT_U( size >= data_size);
	zero_memory( slot.slot_index*line_size, data_size);
}

void res_constant_buffer::set_a	( constant_slot const& slot, u32 l, void* ptr, u32 size)
{
	u32 data_size = slot.cls&constant_class_size_mask;
	ASSERT_U( size >= data_size);
	set_memory( (slot.slot_index+l)*line_size, (char*)ptr, data_size);
}

template<typename T> 
void res_constant_buffer::set		( constant_slot const& slot, T const& value)
{
	set( slot, (char*)&value, constant_type_traits<T>::size);
}


void res_constant_buffer::set(constant const& c)
{
	set( c.slot, c.source.ptr, c.source.size);
//	XRAY_UNREFERENCED_PARAMETER(c);
// 	//ASSERT( rc_float == c.type); // ???? 
// 	ASSERT( c.source.size <= (c.slot.cls&constant_class_size_mask));
// 
// 	set_memory( c.slot.slot_index*line_size, (char*)c.source.ptr, c.source.size);
// // 	void* ptr = access( s.slot_index);
// // 	memory::copy( ptr, m_buffer_size-((char*)ptr - (char*)m_buffer_data), source_ptr, source_size);
// //	m_changed = true;
}

void res_constant_buffer::zero(constant const& c)
{
	zero( c.slot, c.source.size);
}

void res_constant_buffer::set_a	( constant const& c, u32 l)
{
	set_a( c.slot, l, c.source.ptr, c.source.size);
// 	XRAY_UNREFERENCED_PARAMETER(c);
// 	//ASSERT( rc_float == c.type); // ???? 
// 	ASSERT( c.source.size <= (c.slot.cls&constant_class_size_mask));
// 
// 	set_memory( (c.slot.slot_index+l)*line_size, (char*)c.source.ptr, c.source.size);
// //	set_memory( (s.slot_index+l)*line_size, (char*)source_ptr, source_size);
// // 	void* ptr = access( s.slot_index + l*line_size);
// // 	memory::copy( ptr, m_buffer_size - ((char*)ptr - (char*)m_buffer_data), source_ptr, source_size);
// //	m_changed = true;
}


// void res_constant_buffer::set(constant const& c, constant_slot const& s, void* source_ptr, u32 source_size)
// {
// 	XRAY_UNREFERENCED_PARAMETER(c);
// 	//ASSERT( rc_float == c.type); // ???? 
// 	ASSERT( source_size <= (s.cls&constant_class_size_mask));
// 
// 	set_memory( s.slot_index*line_size, (char*)source_ptr, source_size);
// // 	void* ptr = access( s.slot_index);
// // 	memory::copy( ptr, m_buffer_size-((char*)ptr - (char*)m_buffer_data), source_ptr, source_size);
// //	m_changed = true;
// }
// 
// void res_constant_buffer::set_a	( constant const& c, constant_slot const& s, u32 l, void* source_ptr, u32 source_size)
// {
// 	XRAY_UNREFERENCED_PARAMETER(c);
// 	//ASSERT( rc_float == c.type); // ???? 
// 	ASSERT( source_size <= (s.cls&constant_class_size_mask));
// 
// 	set_memory( (s.slot_index+l)*line_size, (char*)source_ptr, source_size);
// // 	void* ptr = access( s.slot_index + l*line_size);
// // 	memory::copy( ptr, m_buffer_size - ((char*)ptr - (char*)m_buffer_data), source_ptr, source_size);
// //	m_changed = true;
// }
// // --Porting to DX10_
/*

void res_constant_buffer::seta(constant const& c, constant_slot const& l, u32 e, const float4x4& a)
{
	XRAY_UNREFERENCED_PARAMETER	( c );

	ASSERT(rc_float == c.type);
	
	u32		base;
	float4*	it;

	switch(l.cls)
	{
	case rc_2x4:
		base = l.slot_index + 2*e;
		it   = m_float_cache.access(base);
		m_float_cache.dirty(base,base+2);
		it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
		it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
		break;
	case rc_3x4:
		base = l.slot_index + 3*e;
		it   = m_float_cache.access(base);
		m_float_cache.dirty(base,base+3);
		it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
		it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
		it[2].set(a.e02, a.e12, a.e22, a.e32);// = a.k;
		break;
	case rc_4x4:
		base = l.slot_index + 4*e;
		it   = m_float_cache.access(base);
		m_float_cache.dirty(base,base+4);
		it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
		it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
		it[2].set(a.e02, a.e12, a.e22, a.e32);// = a.k;
		it[3].set(a.e03, a.e13, a.e23, a.e33);// = a.c;
		break;
	default:
#ifdef DEBUG
		LOG_ERROR("invalid constant run-time-type for '%s'",c.get_name());
#else
		NODEFAULT;
#endif
	}
}

void res_constant_buffer::seta(constant const& c, constant_slot const& l, u32 e, const float4& a)
{
	XRAY_UNREFERENCED_PARAMETER	( c );

	ASSERT(rc_float	== c.type);
	ASSERT(rc_1x4 == l.cls);

	u32	base = l.slot_index+e;
	*m_float_cache.access(base) = a;
	m_float_cache.dirty(base, base+1);
}*/

// void res_constant_buffer::set(constant const& c, constant_slot const& l, const float4x4& a)
// {
// 	XRAY_UNREFERENCED_PARAMETER	( c );
// 
// 	ASSERT(rc_float == c.type);
// 	
// 	float4*	it = m_float_cache.access(l.slot_index);
// 	switch(l.cls)
// 	{
// 	case rc_2x4:
// 		m_float_cache.dirty(l.slot_index,l.slot_index+2);
// 		it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
// 		it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
// 		break;
// 	case rc_3x4:
// 		m_float_cache.dirty(l.slot_index,l.slot_index+3);
// 		it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
// 		it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
// 		it[2].set(a.e02, a.e12, a.e22, a.e32);// = a.k;
// 		break;
// 	case rc_4x4:
// 		m_float_cache.dirty(l.slot_index,l.slot_index+4);
// 		it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
// 		it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
// 		it[2].set(a.e02, a.e12, a.e22, a.e32);// = a.k;
// 		it[3].set(a.e03, a.e13, a.e23, a.e33);// = a.c;
// 		break;
// 	default:
// #ifdef DEBUG
// 		LOG_ERROR("invalid constant run-time-type for '%s'",c.get_name());
// #else
// 		NODEFAULT;
// #endif
// 	}
// }
// 
// void res_constant_buffer::set(constant const& c, constant_slot const& l, const float4& a)
// {
// 	XRAY_UNREFERENCED_PARAMETER	( c );
// 
// 	ASSERT(rc_float == c.type);
// 	ASSERT(rc_1x4 == l.cls);
// 	
// 	*m_float_cache.access(l.slot_index) = a;
// 	m_float_cache.dirty(l.slot_index, l.slot_index+1);
// }
// 
// void res_constant_buffer::seta(constant const& c, constant_slot const& l, u32 e, const float4x4& a)
// {
// 	XRAY_UNREFERENCED_PARAMETER	( c );
// 
// 	ASSERT(rc_float == c.type);
// 	
// 	u32		base;
// 	float4*	it;
// 
// 	switch(l.cls)
// 	{
// 	case rc_2x4:
// 		base = l.slot_index + 2*e;
// 		it   = m_float_cache.access(base);
// 		m_float_cache.dirty(base,base+2);
// 		it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
// 		it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
// 		break;
// 	case rc_3x4:
// 		base = l.slot_index + 3*e;
// 		it   = m_float_cache.access(base);
// 		m_float_cache.dirty(base,base+3);
// 		it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
// 		it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
// 		it[2].set(a.e02, a.e12, a.e22, a.e32);// = a.k;
// 		break;
// 	case rc_4x4:
// 		base = l.slot_index + 4*e;
// 		it   = m_float_cache.access(base);
// 		m_float_cache.dirty(base,base+4);
// 		it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
// 		it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
// 		it[2].set(a.e02, a.e12, a.e22, a.e32);// = a.k;
// 		it[3].set(a.e03, a.e13, a.e23, a.e33);// = a.c;
// 		break;
// 	default:
// #ifdef DEBUG
// 		LOG_ERROR("invalid constant run-time-type for '%s'",c.get_name());
// #else
// 		NODEFAULT;
// #endif
// 	}
// }
// 
// void res_constant_buffer::seta(constant const& c, constant_slot const& l, u32 e, const float4& a)
// {
// 	XRAY_UNREFERENCED_PARAMETER	( c );
// 
// 	ASSERT(rc_float	== c.type);
// 	ASSERT(rc_1x4 == l.cls);
// 
// 	u32	base = l.slot_index+e;
// 	*m_float_cache.access(base) = a;
// 	m_float_cache.dirty(base, base+1);
// }


} // namespace render
} // namespace xray

#endif // #ifndef CONSTANT_BUFFER_INLINE_H_INCLUDED