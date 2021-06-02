////////////////////////////////////////////////////////////////////////////
//	Module 		: fs_readers_base_inline.h
//	Created 	: 02.09.2006
//  Modified 	: 02.09.2006
//	Author		: Dmitriy Iassenev
//	Description : file system reader base class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_FS_READERS_BASE_INLINE_H_INCLUDED
#define CS_CORE_FS_READERS_BASE_INLINE_H_INCLUDED

inline fs::readers::base::base						(pcvoid buffer, u32 const buffer_size, u32 const position)
{
	setup			(buffer,buffer_size,position);
}

inline fs::readers::base::~base						()
{
}

inline void fs::readers::base::setup				(pcvoid buffer, u32 const buffer_size, u32 const position)
{
	m_buffer		= buffer;
	m_buffer_size	= buffer_size;
	m_position		= position;
}

inline bool fs::readers::base::terminal_character	(char const& character) const
{
	if (character == 13)
		return		true;

	if (character == 10)
		return		true;

	return			false;
}

inline pcstr fs::readers::base::r_string			(pstr destination, u32 const max_length)
{
	u32				start_position = m_position;
	u32				string_length = advance_string();
	R_ASSERT		((string_length + 1) < max_length, "buffer overflow");
	
	mem_copy		(destination,(pstr)m_buffer + start_position,string_length*sizeof(char));
	destination[string_length]	= 0;
	
	return			destination;
}

template <int size>
inline pcstr fs::readers::base::r_string			(char (&destination)[size])
{
	return			r_string(destination, size*sizeof(char));
}

inline bool fs::readers::base::eof					() const
{
	return			m_position >= m_buffer_size;
}

inline u32  fs::readers::base::length			() const
{
	return			m_buffer_size;
}

inline u32  fs::readers::base::tell			() const
{
	return			m_position;
}

inline pcvoid fs::readers::base::pointer			() const
{
	return			(u8*)m_buffer + m_position;
}

inline void fs::readers::base::advance				(u32 const size)
{
	ASSERT			(m_position + size <= m_buffer_size, "advancing out of the end of buffer");
	m_position		+= size;
}

inline void fs::readers::base::seek					(u32 const size)
{
	ASSERT			(size <= m_buffer_size, "advancing out of the end of buffer");
	m_position		= size;
}

inline void fs::readers::base::r					(pvoid buffer, u32 const buffer_size)
{
	ASSERT			(m_position + buffer_size <= m_buffer_size, "reading out of the end of buffer");
	mem_copy		(buffer,(u8*)m_buffer + m_position,buffer_size);
	m_position		+= buffer_size;
}

#endif // #ifndef CS_CORE_FS_READERS_BASE_INLINE_H_INCLUDED