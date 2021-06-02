////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_BASE_ALLOCATOR_H_INCLUDED
#define XRAY_MEMORY_BASE_ALLOCATOR_H_INCLUDED

#include <xray/detail_noncopyable.h>
#include <xray/memory_debug_parameters.h>

#ifdef MASTER_GOLD
#	define XRAY_USE_MEMORY_TOOLS	0
#else // #ifdef MASTER_GOLD
#	define XRAY_USE_MEMORY_TOOLS	1
#endif // #ifdef MASTER_GOLD

namespace xray {
namespace memory {

class XRAY_CORE_API base_allocator : private core::noncopyable {
public:
					base_allocator		(
#ifndef MASTER_GOLD
						bool const use_leak_detector = true
#endif // #ifndef MASTER_GOLD
					);
	virtual			~base_allocator		( ) { }
	virtual	void	initialize			( pvoid arena, u64 size, pcstr arena_id );
			void	do_register			( u64 arena_size, pcstr description );
			void	finalize			( );
			void	dump_statistics		( ) const;

			pvoid	malloc_impl			( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
			pvoid	realloc_impl		( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
			void	free_impl			( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );

	inline	bool	initialized			( ) const { return !!m_arena_start; }
	inline	pcstr	arena_id			( ) const { return m_arena_id; }
	virtual	size_t	total_size			( ) const = 0;
	virtual	size_t	allocated_size		( ) const = 0;
			size_t	usable_size			( pvoid pointer ) const;

protected:
	virtual	pvoid	call_malloc			( size_t size_t XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION ) = 0;
	virtual	pvoid	call_realloc		( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION ) = 0;
	virtual	void	call_free			( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION ) = 0;
	virtual	size_t	usable_size_impl	( pvoid pointer ) const = 0;

protected:
			void	copy				( base_allocator const& allocator );
#if XRAY_USE_MEMORY_TOOLS
			pvoid	on_malloc			( pvoid buffer, size_t buffer_size, size_t previous_size, pcstr description ) const;
			void	on_free				( pvoid& buffer, bool clear_with_magic = true ) const;
			size_t	needed_size			( size_t const size ) const;
#else // #if XRAY_USE_MEMORY_TOOLS
	inline	pvoid	on_malloc			( pvoid buffer, size_t buffer_size, size_t previous_size, pcstr description ) const { XRAY_UNREFERENCED_PARAMETER(buffer_size); XRAY_UNREFERENCED_PARAMETER(previous_size); XRAY_UNREFERENCED_PARAMETER(description); return buffer; }
	inline	void	on_free				( pvoid buffer, bool can_clear = true ) const { XRAY_UNREFERENCED_PARAMETER(can_clear); XRAY_UNREFERENCED_PARAMETER(buffer); }
	inline	size_t	needed_size			( size_t const size) const { ASSERT( size ); return size; }
#endif // #if XRAY_USE_MEMORY_TOOLS

protected:
			void	call_initialize_impl( base_allocator& allocator, pvoid arena, u64 size, pcstr arena_id );
			void	call_finalize_impl	( base_allocator& allocator );

private:
			pvoid	on_malloc_check_magic	( pvoid buffer, size_t buffer_size, size_t previous_size, pcstr description ) const;
			void	on_free_check_magic		( pvoid buffer, bool clear_with_magic ) const;
			size_t	needed_size_for_magic	( size_t const size) const;

	virtual	void	initialize_impl		( pvoid arena, u64 size, pcstr arena_id ) = 0;
	virtual	void	finalize_impl		( ) = 0;

protected:
	pvoid	m_arena_start;
	pvoid	m_arena_end;
	pcstr	m_arena_id;

private:
#ifndef MASTER_GOLD
	bool	m_use_leak_detector;
#endif // #ifndef MASTER_GOLD
}; // class doug_lea_allocator

namespace monitor {
extern u32 const housekeeping_size;
} // namespace monitor

} // namespace memory
} // namespace xray

#endif // #ifndef XRAY_MEMORY_BASE_ALLOCATOR_H_INCLUDED