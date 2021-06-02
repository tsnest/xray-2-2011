////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_DOUG_LEA_MT_ALLOCATOR_H_INCLUDED
#define XRAY_MEMORY_DOUG_LEA_MT_ALLOCATOR_H_INCLUDED

#if 1//XRAY_DEBUG_ALLOCATOR
#include <xray/memory_doug_lea_allocator.h>
#include <xray/threading_mutex.h>

namespace xray {
namespace memory {

#if !XRAY_USE_CRT_MEMORY_ALLOCATOR
class XRAY_CORE_API doug_lea_mt_allocator : public doug_lea_allocator {
public:
				doug_lea_mt_allocator	(
						bool const crash_after_out_of_memory = true,
						bool const return_null_after_out_of_memory = false,
						bool use_guards = true
#ifndef MASTER_GOLD
						, bool const use_leak_detector = true
#endif // #ifndef MASTER_GOLD
					);
			pvoid	malloc_impl			( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
			pvoid	realloc_impl		( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
			void	free_impl			( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	inline	threading::mutex& mutex		( ) const { return m_mutex; }
	virtual	void	initialize			( pvoid arena, u64 arena_size, pcstr arena_id );

private:
	typedef	doug_lea_allocator			super;

protected:
	virtual	pvoid	call_malloc			( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	virtual	pvoid	call_realloc		( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	virtual	void	call_free			( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );

private:
	virtual	void	initialize_impl		( pvoid arena, u64 arena_size, pcstr arena_id );
	virtual	void	finalize_impl		( );
	virtual	size_t	total_size			( ) const;
	virtual	size_t	allocated_size		( ) const;

public:
	u64							dummy;

private:
	mutable threading::mutex	m_mutex;
}; // class doug_lea_allocator

#else // #if !XRAY_USE_CRT_MEMORY_ALLOCATOR

typedef crt_allocator doug_lea_mt_allocator;

#endif // #if !XRAY_USE_CRT_MEMORY_ALLOCATOR

} // namespace memory
} // namespace xray
#endif // #if XRAY_DEBUG_ALLOCATOR

#endif // #ifndef XRAY_MEMORY_DOUG_LEA_MT_ALLOCATOR_H_INCLUDED