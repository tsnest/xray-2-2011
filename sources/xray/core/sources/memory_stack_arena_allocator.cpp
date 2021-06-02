#include "pch.h"

#if !XRAY_USE_CRT_MEMORY_ALLOCATOR

#include "memory_stack_arena_allocator.h"
#include "memory_monitor.h"

using xray::memory::stack_arena_allocator;

static const u32 s_block_header_size		= sizeof( u32 ) + sizeof( char );

stack_arena_allocator::stack_arena_allocator	( ) :
	m_cur_top					( 0 ),
	m_user_thread_id			( threading::current_thread_id( ) )
{
}

void stack_arena_allocator::initialize_impl	( pvoid const arena, u64 const arena_size, pcstr const arena_id )
{
	XRAY_UNREFERENCED_PARAMETERS( arena_id, arena_size );
	m_cur_top					= align_8( (pbyte)arena + s_block_header_size );
	R_ASSERT					( m_cur_top < (pbyte)m_arena_end );
	ref_is_free( m_cur_top )	= true;
	ref_prev_size( m_cur_top )	= 0;
}

void stack_arena_allocator::finalize_impl	( )
{
}

void stack_arena_allocator::user_thread_id	( u32 user_thread_id ) const
{
	threading::interlocked_exchange	( (threading::atomic32_value_type&)m_user_thread_id, user_thread_id );
}

void stack_arena_allocator::user_current_thread_id	( ) const
{
	user_thread_id				( threading::current_thread_id( ) );
}

inline bool	stack_arena_allocator::initialized	( ) const
{
	if ( !super::initialized( ) )
		return					( false );

	u32 const current_thread_id	= xray::threading::current_thread_id ( );
	XRAY_UNREFERENCED_PARAMETER	( current_thread_id );
	R_ASSERT					( current_thread_id == m_user_thread_id );
	return						( true );
}

pvoid stack_arena_allocator::malloc_impl	( size_t const raw_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;

	ASSERT						( initialized ( ) );

	size_t const size			= needed_size( raw_size );
	pbyte next_top				= align_8(m_cur_top + size + s_block_header_size);
	R_ASSERT					( next_top < (pbyte)m_arena_end );

	ref_is_free( m_cur_top )	= false;

	ref_is_free( next_top )		= true;
	ref_prev_size( next_top )	= ( u32 )( next_top - m_cur_top );

	pbyte const result			= m_cur_top;
	m_cur_top					= next_top;

//	printf						( "allocating %d block %d\n", size, u32( res ) );

	return						on_malloc( result, raw_size, 0, XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_PARAMETER );
}

pvoid stack_arena_allocator::realloc_impl	( pvoid const pointer, size_t const new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	ASSERT						( initialized ( ) );

	pvoid new_ptr			=	malloc_impl ( new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS );
	size_t const old_size	=	buffer_size	(pointer);

	memory::copy				(new_ptr, new_size, pointer, math::min(new_size, old_size));
	free_impl					( pointer XRAY_CORE_DEBUG_PARAMETERS );
	return						new_ptr;
}

void stack_arena_allocator::free_impl		( pvoid block XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;

	ASSERT						( initialized ( ) );
//	printf						( "dealloc of block %d\n", ( u32 )block );

	on_free						( block );

	pbyte const real_block		=	(pbyte)block;

	u8* is_free_ptr				=	&ref_is_free( real_block );
	u8  is_free					=	*is_free_ptr;

	u32 prev_size				=	ref_prev_size( m_cur_top );

	ASSERT						( !is_free );
	if ( is_free )
		return;

	*is_free_ptr				= true;

	pbyte last_block			= m_cur_top - prev_size;
	
	while ( ref_is_free( last_block ) ) {
		m_cur_top				= last_block;
		if ( !ref_prev_size( m_cur_top ) )
			break;				// origin

		last_block				= m_cur_top - ref_prev_size( m_cur_top );
	}
}

pvoid stack_arena_allocator::call_malloc	( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return							( malloc_impl( size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS ) );
}

pvoid stack_arena_allocator::call_realloc	( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return							( realloc_impl( pointer, new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS ) );
}

void stack_arena_allocator::call_free		( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	free_impl						( pointer XRAY_CORE_DEBUG_PARAMETERS );
}

inline u32& stack_arena_allocator::ref_prev_size( pbyte const addr )
{
	return						( *( u32* )( addr - 4 ) );
}

inline u8& stack_arena_allocator::ref_is_free	( pbyte const addr )
{
	return						( *( addr - 5 ) );
}

inline pbyte stack_arena_allocator::align_8		( pbyte const addr )
{
	size_t const aligned		= ( size_t )addr & ~( 1 | 2 | 4 );
	if ( aligned == (u64)addr )
		return					( ( pbyte )aligned );

	// aligned < addr
	return						( ( pbyte )aligned + 8 );		
}

size_t stack_arena_allocator::total_size		( ) const
{
	return						( size_t( (pbyte)m_arena_end - (pbyte)m_arena_start ) );
}

size_t stack_arena_allocator::allocated_size	( ) const
{
	pbyte const	top_header_addr		=	m_cur_top - s_block_header_size;
	pbyte const	top_addr			=	(pbyte)( (ptrdiff_t)top_header_addr & ~( 1 | 2 | 4 ) );

	return								( size_t( top_addr - (pbyte)m_arena_start ) );
}

#endif // #if !XRAY_USE_CRT_MEMORY_ALLOCATOR