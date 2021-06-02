////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_BUFFER_H_INCLUDED
#define XRAY_MEMORY_BUFFER_H_INCLUDED

namespace xray {

class XRAY_CORE_API mutable_buffer
{
public:
				mutable_buffer	() : m_data(NULL), m_size(0) {}
				mutable_buffer	(pvoid data, u32 size) : m_data((pstr)data), m_size(size) {}

				template <class T>
				mutable_buffer	(T * data, u32 size) : m_data((pstr)data), m_size(size) 
				{
					COMPILE_ASSERT	(sizeof(T) == 1, You_can_only_construct_mutable_buffer_from_pointer_to_void_or_char_or_byte);
				}

				template <class T, u32 count>
				mutable_buffer	(T (& data)[count]) : m_data((pstr)data), m_size(sizeof(T) * count) {}

	u32			size			() const	{ return m_size; }
	pvoid		c_ptr			() const	{ return m_data; }
	void		operator +=		(u32 offs);
	operator	bool			() const	{ return !!m_data; }
	bool		operator ==		(mutable_buffer const & other) const;

	char &		operator []		(u32 index)			{ ASSERT(index < m_size); return m_data[index]; }
	char		operator []		(u32 index) const	{ ASSERT(index < m_size); return m_data[index]; }

	static mutable_buffer	zero	() { return mutable_buffer(NULL, 0); }

	mutable_buffer	slice		(u32 offs, u32 count) const;
	void		reset			() { m_data = NULL; m_size = 0; }

private:
	pstr		m_data;
	u32			m_size;
}; // class mutable_buffer

class XRAY_CORE_API const_buffer
{
public:
				const_buffer	() : m_data(NULL), m_size(0) {}
				const_buffer	(pcvoid data, u32 size) : m_data((pcstr)data), m_size(size) {}
				template <class T>
				const_buffer	(T const * data, u32 size) : m_data((pcstr)data), m_size(size) 
				{
					COMPILE_ASSERT	(sizeof(T) == 1, You_can_only_construct_const_buffer_from_pointer_to_void_or_char_or_byte);
				}

				const_buffer	(mutable_buffer const& buffer);

				template <class T, u32 count>
				const_buffer	(T const (& data)[count]) : m_data((pcstr)data), m_size(sizeof(T) * count) {}

	u32			size			() const	{ return m_size; }
	pcvoid		c_ptr			() const	{ return m_data; }
	void		operator +=		(u32 offs);
	operator	bool			() const	{ return !!m_data || m_size; }
	bool		operator ==		(const_buffer const & other) const;
	char		operator []		(u32 index) const	{ ASSERT(index < m_size); return m_data[index]; }

	const_buffer	slice		(u32 offs, u32 count) const;
	void		reset			() { m_data = NULL; m_size = 0; }

	static const_buffer	zero	() { return const_buffer(NULL, 0); }

private:
	pcstr		m_data;
	u32			m_size;
}; // class const_buffer

mutable_buffer	operator + (mutable_buffer const&	buffer,	u32 const offs);
const_buffer	operator + (const_buffer const&		buffer,	u32 const offs);

inline
mutable_buffer	cast_away_const (const_buffer const & buffer)
{
	return		mutable_buffer((pbyte)buffer.c_ptr(), buffer.size());
}

namespace memory {

template <class T>
inline mutable_buffer buffer	(T* data, u32 size);

template <class T>
inline const_buffer	buffer		(T const* data, u32 size);

template <class T, u32 size>
inline mutable_buffer buffer	(T (&array)[size]);

template <class T, u32 size>
inline const_buffer	buffer		(T const (&array)[size]);

} // namespace memory
} // namespace xray

#include "memory_buffer_inline.h"

#endif // #ifndef XRAY_MEMORY_BUFFER_H_INCLUDED