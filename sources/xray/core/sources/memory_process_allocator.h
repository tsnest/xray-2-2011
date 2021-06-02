////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_PROCESS_ALLOCATOR_H_INCLUDED
#define XRAY_MEMORY_PROCESS_ALLOCATOR_H_INCLUDED

#include <xray/memory_base_allocator.h>
#include <xray/memory_debug_parameters.h>

namespace xray {
namespace memory {

class XRAY_CORE_API process_allocator : public base_allocator {
public:
			pvoid	malloc_impl			( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
			pvoid	realloc_impl		( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
			void	free_impl			( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	virtual	size_t	allocated_size		( ) const;

private:
	typedef	base_allocator				super;

protected:
	virtual	pvoid	call_malloc			( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	virtual	pvoid	call_realloc		( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	virtual	void	call_free			( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	virtual	size_t	usable_size_impl	( pvoid pointer ) const;

private:
	virtual	void	initialize_impl		( pvoid arena, u64 arena_size, pcstr arena_id );
	virtual	void	finalize_impl		( );
	virtual	size_t	total_size			( ) const;
}; // class process_allocator

} // namespace memory
} // namespace xray

#endif // #ifndef XRAY_MEMORY_PROCESS_ALLOCATOR_H_INCLUDED