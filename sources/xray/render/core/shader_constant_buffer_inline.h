////////////////////////////////////////////////////////////////////////////
//	Created		: 01.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_BUFFER_INLINE_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_CONSTANT_BUFFER_INLINE_H_INCLUDED

namespace xray {
namespace render {

inline void shader_constant_buffer::set_memory( const u32 offset, char const * src_ptr, const u32 size)
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


// 	ASSERT						((s32)size <= ((s32)m_buffer_size - offset));
// 	
// 	pvoid destination_ptr		= (pbyte)m_buffer_data + offset;
// 	u32 const copy_size			= ((s32)size <= ( (s32)m_buffer_size - offset)) ? size : ((s32) m_buffer_size - offset);
// 	
// 	m_changed					= true;//xray::memory::compare(
// 		//xray::const_buffer(destination_ptr, copy_size),
// 		//xray::const_buffer(src_ptr, copy_size)
// 	//) != 0;
// 	
// 	if (m_changed)
// 		xray::memory::copy		(destination_ptr, copy_size, src_ptr, copy_size);
}

inline void shader_constant_buffer::zero_memory( const u32 offset, const u32 size)
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

// 	ASSERT						((s32)size <= ((s32)m_buffer_size - offset));
// 	
// 	pvoid destination_ptr		= (pbyte)m_buffer_data + offset;
// 	u32 const copy_size			= ((s32)size <= ( (s32)m_buffer_size - offset)) ? size : ((s32) m_buffer_size - offset);
// 
// 	xray::memory::zero			(destination_ptr, copy_size);
// 	
// 	m_changed					= true;
}

inline pvoid shader_constant_buffer::access( u32 offset)
{
	ASSERT( offset < m_buffer_size);

	return( offset < m_buffer_size) ? ((char*)m_buffer_data) + offset : 0;
}

inline void shader_constant_buffer::set		( shader_constant_slot const& slot, void* ptr, u32 size)
{
	u32 data_size = slot.class_id() & constant_class_size_mask;
	ASSERT_U( size >= data_size && slot.array_size() == 1);
	set_memory( slot.slot_index()*line_size, (char*)ptr, data_size);
}

inline void shader_constant_buffer::set		( shader_constant_slot const& slot, void* ptr, u32 size, u32 array_size)
{
	u32 data_size = slot.class_id() & constant_class_size_mask;
	//ASSERT_U( size == data_size && slot.array_size() <= array_size && slot.array_size() > 0);
	ASSERT_U( size == data_size && slot.array_size() >= array_size && slot.array_size() > 0);
	set_memory( slot.slot_index()*line_size, (char*)ptr, data_size*slot.array_size());
}

inline void shader_constant_buffer::zero		( shader_constant_slot const& slot, u32 size)
{
	u32 data_size = slot.class_id() & constant_class_size_mask;
	ASSERT_U( size >= data_size);
	zero_memory( slot.slot_index()*line_size, data_size);
}

template < typename T > 
inline void shader_constant_buffer::set_typed		( shader_constant_slot const& slot, T const& value)
{
	set( slot, (char*)&value, constant_type_traits<T>::size);
}

template < typename T > 
inline void shader_constant_buffer::set_typed		( shader_constant_slot const& slot, T const * value, u32 array_size)
{
	set( slot, (char*)value, constant_type_traits<T>::size, array_size);
}

inline void shader_constant_buffer::set(shader_constant const& c)
{
	set( c.slot(), c.source().pointer(), c.source().size(), c.slot().array_size());
}

inline void shader_constant_buffer::zero(shader_constant const& c)
{
	zero( c.slot(), c.source().size());
}


inline ID3DConstantBuffer* shader_constant_buffer::hardware_buffer	( ) const
{
	return m_hardware_buffer;
}

inline name_string_type const& shader_constant_buffer::name			( )	const
{
	return m_name;
}

inline enum_shader_type& shader_constant_buffer::dest				( )
{
	return m_dest;
}

inline D3D_CBUFFER_TYPE shader_constant_buffer::type				( )	const
{
	return m_type;
}

inline u32 shader_constant_buffer::size								( )	const
{
	return m_buffer_size;
}

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_BUFFER_INLINE_H_INCLUDED