////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_extensions.h
//	Created 	: 24.08.2006
//  Modified 	: 24.08.2006
//	Author		: Dmitriy Iassenev
//	Description : memory extensions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_MEMORY_EXTENSIONS_H_INCLUDED
#define CS_CORE_MEMORY_EXTENSIONS_H_INCLUDED

#include <boost/type_traits/is_polymorphic.hpp>

struct CS_CORE_API cs_memory {
		void	initialize		();
		void	uninitialize	();
#if CS_DEBUG_LIBRARIES
		pvoid	mem_realloc		( pcvoid buffer, size_t const& new_buffer_size, pcstr description );
#else // #if CS_DEBUG_LIBRARIES
		pvoid	mem_realloc		( pcvoid buffer, size_t const& new_buffer_size );
#endif // #if CS_DEBUG_LIBRARIES
		void	switch_mt_alloc	();
};

extern CS_CORE_API cs_memory	memory;

#if CS_DEBUG_LIBRARIES
#	define cs_realloc( pointer, size, description )	memory.mem_realloc ( pointer, size, description )
#else // #if CS_DEBUG_LIBRARIES
#	define cs_realloc( pointer, size, description )	memory.mem_realloc ( pointer, size )
#endif // #if CS_DEBUG_LIBRARIES

#define cs_malloc( size, description )	cs_realloc ( 0, size, description )

inline pvoid cs_free_helper2	(pvoid address)
{
	return						cs_realloc ( address, 0, "" ) ;
}

inline void mem_copy			(pvoid destination, pcvoid source, size_t size)
{
	memcpy						( destination, source, size );
}

inline void mem_fill			(pvoid destination, int const value, size_t size)
{
	memset						( destination, value, size );
}

template <typename T>
inline void cs_free_helper		(T* &pointer, pvoid top_pointer)
{
	ASSERT						(top_pointer);
	ASSERT						(pointer);
	cs_free_helper2				(top_pointer);
	pointer						= 0;
}

template <typename T>
inline void cs_free				(T* &pointer)
{
	if (pointer)
		cs_free_helper			(pointer,(pvoid)pointer);
}

template <typename T>
inline void cs_delete_helper2	(T* &pointer, pvoid top_pointer)
{
	pointer->~T					();
	cs_free_helper				(pointer,top_pointer);
}

template <typename T, bool polymorphic>
struct cs_delete_helper {
	static inline void apply	(T* &pointer)
	{
		cs_delete_helper2		(pointer,dynamic_cast<pvoid>(pointer));
	}
};

template <typename T>
struct cs_delete_helper<T,false> {
	static inline void apply	(T* &pointer)
	{
		cs_delete_helper2		(pointer,pointer);
	}
};

template <typename T>
inline void cs_delete			(T* &pointer)
{
	if (pointer)
		cs_delete_helper<
			T,
			boost::is_polymorphic<T>::value
		>::apply				(
			pointer
		);
}

template <typename T>
inline T* cs_alloc				(u32 const count, pcstr description )
{
	CS_UNREFERENCED_PARAMETER	( description );
	return						(T*)cs_malloc( sizeof(T)*count, description ) ;
}

#define	CS_MEMORY_MANAGER_GENERATOR_MAX_ARITY	10
#include <cs/core/memory_manager_generator.h>

#endif // #ifndef CS_CORE_MEMORY_EXTENSIONS_H_INCLUDED