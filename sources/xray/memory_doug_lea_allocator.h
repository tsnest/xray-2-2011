////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_DOUG_LEA_ALLOCATOR_H_INCLUDED
#define XRAY_MEMORY_DOUG_LEA_ALLOCATOR_H_INCLUDED

#include <xray/memory_base_allocator.h>
#include <xray/memory_debug_parameters.h>

namespace xray {
namespace memory {

enum	thread_id_const_bool			{	thread_id_const_false, 
											thread_id_const_true,	};
	
#if !XRAY_USE_CRT_MEMORY_ALLOCATOR
class XRAY_CORE_API doug_lea_allocator : public base_allocator {
public:
					doug_lea_allocator	(
						thread_id_const_bool const thread_id_const	=	thread_id_const_true,
						bool const crash_after_out_of_memory = true,
						bool const return_null_after_out_of_memory = false,
						bool use_guards = true						
#ifndef MASTER_GOLD
						, bool const use_leak_detector = true
#endif // #ifndef MASTER_GOLD
					);
			void	user_thread_id		( u32 user_thread_id ) const;
			void	user_current_thread_id	( ) const;
	inline	u32		user_thread_id		( ) const { return m_user_thread_id; }
			pvoid	malloc_impl			( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
			pvoid	realloc_impl		( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
			void	free_impl			( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	inline	size_t	usable_size			( pvoid pointer ) const { return m_use_guards ? base_allocator::usable_size(pointer) : doug_lea_allocator::usable_size_impl(pointer);}

private:
	inline	pvoid	on_malloc			( pvoid buffer, size_t buffer_size, size_t previous_size, pcstr description ) const { return m_use_guards ? base_allocator::on_malloc(buffer, buffer_size, previous_size,description) : buffer; }
	inline	void	on_free				( pvoid& buffer, bool clear_with_magic = true ) const { if ( m_use_guards ) base_allocator::on_free( buffer, clear_with_magic ); }
	inline	size_t	needed_size			( size_t const size ) const { return m_use_guards ? base_allocator::needed_size(size) : size; }

private:
	typedef	base_allocator				super;

protected:
	virtual	pvoid	call_malloc			( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	virtual	pvoid	call_realloc		( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	virtual	void	call_free			( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );

protected:
	virtual	void	initialize_impl		( pvoid arena, u64 arena_size, pcstr arena_id );
	virtual	void	finalize_impl		( );
	virtual	size_t	total_size			( ) const;
	virtual	size_t	allocated_size		( ) const;
	virtual	size_t	usable_size_impl	( pvoid pointer ) const;

private:
	inline	bool	initialized			( ) const;

private:
	pvoid			m_arena;
	mutable pcstr	m_user_thread_logging_name;
	
	mutable thread_id_const_bool m_thread_id_const;
	mutable bool	m_user_thread_id_called;

	mutable u32		m_user_thread_id;
	bool			m_crash_after_out_of_memory;
	bool			m_return_null_after_out_of_memory;
	bool			m_out_of_memory;
	bool			m_use_guards;
}; // class doug_lea_allocator

#else // #if !XRAY_USE_CRT_MEMORY_ALLOCATOR

typedef crt_allocator doug_lea_allocator;

#endif // #if !XRAY_USE_CRT_MEMORY_ALLOCATOR

} // namespace memory
} // namespace xray

#endif // #ifndef XRAY_MEMORY_DOUG_LEA_ALLOCATOR_H_INCLUDED