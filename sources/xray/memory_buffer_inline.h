////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_BUFFER_INLINE_H_INCLUDED
#define XRAY_MEMORY_BUFFER_INLINE_H_INCLUDED

namespace xray {

inline
const_buffer::const_buffer (mutable_buffer const& buffer) 
							:	m_data((pcstr)buffer.c_ptr()), m_size(buffer.size())
{
}

inline
bool   const_buffer::operator == ( const_buffer const& other ) const
{
	if ( size() != other.size() )
		return					false;

	return						c_ptr() == other.c_ptr();
}

inline
const_buffer   const_buffer::slice (u32 offs, u32 count) const
{
	R_ASSERT_CMP				(offs + count, <=, m_size);
	return						const_buffer(m_data + offs, count);
}

inline
mutable_buffer   mutable_buffer::slice (u32 offs, u32 count) const
{
	R_ASSERT_CMP				( offs + count, <=, m_size );
	return						mutable_buffer( m_data + offs, count );
}

inline
bool   mutable_buffer::operator == ( mutable_buffer const& other ) const
{
	if ( size() != other.size() )
		return					false;

	return						c_ptr() == other.c_ptr();
}

inline
void   mutable_buffer::operator += (u32 offs)
{
	R_ASSERT_CMP				( offs, <=, m_size );
	(char*&)m_data			+=	offs;
	m_size					-=	offs;
}

inline
void   const_buffer::operator += (u32 offs)
{
	R_ASSERT_CMP				( offs, <=, m_size );
	(char const*&)m_data	+=	offs;
	m_size					-=	offs;
}

inline
mutable_buffer   operator + (mutable_buffer const& buffer, u32 const offs)
{
	mutable_buffer	result	=	buffer;
	result					+=	offs;
	return						result;
}

inline
const_buffer   operator + (const_buffer const& buffer, u32 const offs)
{
	const_buffer	result	=	buffer;
	result					+=	offs;
	return						result;
}

namespace memory {

template <class T>
mutable_buffer	buffer (T* data, u32 size)
{
	return						mutable_buffer( data, size );
}

template <class T>
const_buffer	buffer (T const* data, u32 size)
{
	return						const_buffer( data, size );
}

template <class T, u32 size>
mutable_buffer	buffer (T (&array)[size])
{
	return						mutable_buffer( array, sizeof(T)*size );
}


template <class T, u32 size>
const_buffer	buffer (T const (&array)[size])
{
	return						const_buffer( array, sizeof(T)*size );
}

} // namespace memory
} // namespace xray

#endif // #ifndef XRAY_MEMORY_BUFFER_INLINE_H_INCLUDED