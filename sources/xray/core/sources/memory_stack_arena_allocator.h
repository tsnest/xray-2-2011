#ifndef MEMORY_STACK_ARENA_ALLOCATOR_H_INCLUDED
#define MEMORY_STACK_ARENA_ALLOCATOR_H_INCLUDED

#include <xray/memory_debug_parameters.h>

namespace xray {
namespace memory {

#if !XRAY_USE_CRT_MEMORY_ALLOCATOR
// this allocator doesn't handle fragmentation at all
// it just handle the top address
class XRAY_CORE_API stack_arena_allocator : public base_allocator {
public:

					stack_arena_allocator	( );
			void	user_thread_id			( u32 user_thread_id ) const;
			void	user_current_thread_id	( ) const;
			pvoid	malloc_impl				( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
			pvoid	realloc_impl			( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
			void	free_impl				( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );

private:
	typedef	base_allocator					super;

protected:
	virtual	pvoid	call_malloc				( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	virtual	pvoid	call_realloc			( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	virtual	void	call_free				( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );

private:
	virtual	void	initialize_impl			( pvoid arena, u64 arena_size, pcstr arena_id );
	virtual	void	finalize_impl			( );
	virtual	size_t	total_size				( ) const;
	virtual	size_t	allocated_size			( ) const;

private:
	inline	bool	initialized				( ) const;
	inline	u32&	ref_prev_size			( pbyte addr );
	inline	u8&		ref_is_free				( pbyte addr );
	inline	pbyte	align_8					( pbyte addr );

private:
	pbyte			m_cur_top;
	mutable u32		m_user_thread_id;
	bool			m_data_owned;
}; // class stack_arena_allocator

#else // #if !XRAY_USE_CRT_MEMORY_ALLOCATOR

typedef crt_allocator	stack_arena_allocator;

#endif // #if !XRAY_USE_CRT_MEMORY_ALLOCATOR


} // namespace memory
} // namespace xray

#endif // #ifndef MEMORY_STACK_ARENA_ALLOCATOR_H_INCLUDED