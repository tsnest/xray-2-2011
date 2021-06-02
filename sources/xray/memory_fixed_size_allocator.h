////////////////////////////////////////////////////////////////////////////
//	Created		: 06.11.2009
//	Author		: Sergey Chechin, Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_FIXED_SIZE_ALLOCATOR_H_INCLUDED
#define XRAY_MEMORY_FIXED_SIZE_ALLOCATOR_H_INCLUDED

#include <xray/memory_single_size_buffer_allocator.h>

namespace xray {
namespace memory {

template < class T, class ThreadPolicy >
class fixed_size_allocator : public base_allocator
{
public:
					fixed_size_allocator( );
				   ~fixed_size_allocator( );

	pvoid			allocate			( );
	void			deallocate			( pvoid& );

	virtual	size_t	total_size			( ) const { return m_allocator->total_size(); }
	virtual	size_t	allocated_size		( ) const { return m_allocator->allocated_size(); }

private:
	virtual	void	initialize_impl		( pvoid arena, u64 size, pcstr arena_id );
	virtual	void	finalize_impl		( ) {}

	virtual	pvoid	call_malloc			( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	virtual	pvoid	call_realloc		( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	virtual	void	call_free			( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	virtual	size_t	usable_size_impl	( pvoid ) const { NOT_IMPLEMENTED( return 0); }

private:
	typedef single_size_buffer_allocator< sizeof( T ), ThreadPolicy > single_size_allocator;

private:
	uninitialized_reference< single_size_allocator >	m_allocator;
	pcstr												m_arena_id;
};

} // namespace memory
} // namespace xray

#include <xray/memory_fixed_size_allocator_inline.h>

#endif // #ifndef XRAY_MEMORY_FIXED_SIZE_ALLOCATOR_H_INCLUDED