////////////////////////////////////////////////////////////////////////////
//	Created		: 06.11.2009
//	Author		: Sergey Chechin, Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_FIXED_SIZE_ALLOCATOR_INLINE_H_INCLUDED
#define XRAY_MEMORY_FIXED_SIZE_ALLOCATOR_INLINE_H_INCLUDED

namespace xray {
namespace memory {

template < class T, class ThreadPolicy >
fixed_size_allocator< T, ThreadPolicy >::fixed_size_allocator		( ) :
	m_arena_id			( 0 )
{
}

template < class T, class ThreadPolicy >
fixed_size_allocator< T, ThreadPolicy >::~fixed_size_allocator		( )
{
	XRAY_DESTROY_REFERENCE			( m_allocator );
}

template < class T, class ThreadPolicy >
inline void fixed_size_allocator< T, ThreadPolicy >::initialize_impl(
		pvoid arena,
		u64 size,
		pcstr arena_id
	)
{	
	m_arena_id						= arena_id;
	pbyte const aligned_arena		= (pbyte)math::align_up( (size_t)arena, (size_t)XRAY_DEFAULT_ALIGN_SIZE );
	XRAY_CONSTRUCT_REFERENCE		( m_allocator, single_size_allocator )( aligned_arena, (size_t)size );
}

template < class T, class ThreadPolicy >
pvoid fixed_size_allocator< T, ThreadPolicy >::allocate ()
{
	return							m_allocator->allocate();
}

template < class T, class ThreadPolicy >
inline pvoid fixed_size_allocator< T, ThreadPolicy >::call_malloc ( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;
	XRAY_UNREFERENCED_PARAMETER		( XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_PARAMETER );
	R_ASSERT_U						( size == sizeof( T ) );
	return							allocate();
}

template < class T, class ThreadPolicy >
inline pvoid fixed_size_allocator< T, ThreadPolicy >::call_realloc (
		pvoid pointer,
		size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION
	)
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;
	XRAY_UNREFERENCED_PARAMETER		( XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_PARAMETER );
	XRAY_UNREFERENCED_PARAMETERS	( new_size, pointer );
	NOT_IMPLEMENTED					( return 0 );
}

template < class T, class ThreadPolicy >
inline void fixed_size_allocator< T, ThreadPolicy >::call_free ( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;
	deallocate						( pointer );
}

template < class T, class ThreadPolicy >
void fixed_size_allocator< T, ThreadPolicy >::deallocate	( pvoid& data )
{
	m_allocator->deallocate			( data );
}

} // namespace memory
} // namespace xray

#endif // #ifndef XRAY_MEMORY_FIXED_SIZE_ALLOCATOR_INLINE_H_INCLUDED