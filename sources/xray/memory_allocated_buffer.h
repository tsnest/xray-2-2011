////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_ALLOCATED_BUFFER_H_INCLUDED
#define XRAY_MEMORY_ALLOCATED_BUFFER_H_INCLUDED

#include <xray/memory_buffer.h>

namespace xray {

class allocated_buffer : core::noncopyable
{
public:
	allocated_buffer					() : m_allocator(NULL) {}
	allocated_buffer					(u32 size, memory::base_allocator * allocator, pcstr description = "allocated_buffer")
	{
		assign								(size, allocator, description);
	}		

	~allocated_buffer					()	{	reset();	}

	void		assign					(u32 size, memory::base_allocator * allocator, pcstr description = "allocated_buffer")
	{
		reset								();
		XRAY_UNREFERENCED_PARAMETER			(description);
		m_buffer						=	mutable_buffer(XRAY_MALLOC_IMPL(allocator, size, description), size);
		m_allocator						=	allocator;		
	}

	u32			size					() const	{ return m_buffer.size(); }
	pvoid		c_ptr					() const	{ return m_buffer.c_ptr(); }
	operator	bool					() const	{ return m_buffer; }
	
	char &		operator []				(u32 index)			{ return m_buffer[index]; }
	char		operator []				(u32 index) const	{ return m_buffer[index]; }

	operator	const_buffer			() const { return m_buffer; }
	operator	mutable_buffer			() const { return m_buffer; }
	
	void		reset					()
	{
		if ( !m_allocator )
			return;
		pvoid ptr						=	m_buffer.c_ptr(); 
		XRAY_FREE_IMPL						(m_allocator, ptr);	
		m_allocator						=	NULL;
	}

private:
	mutable_buffer						m_buffer;
	memory::base_allocator *			m_allocator;

}; // class allocated_buffer

} // namespace xray

#endif // #ifndef XRAY_MEMORY_ALLOCATED_BUFFER_H_INCLUDED