////////////////////////////////////////////////////////////////////////////
//	Created		: 17.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_MULTI_THREADING_SINGLE_SIZE_ALLOCATOR_POLICY_H_INCLUDED
#define XRAY_MEMORY_MULTI_THREADING_SINGLE_SIZE_ALLOCATOR_POLICY_H_INCLUDED

namespace xray {
namespace memory {

template < typename T >
struct multi_threading_single_size_allocator_policy
{
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4201)
	union free_list_type
	{
		threading::atomic64_type	whole;

#if XRAY_PLATFORM_32_BIT
		void			set_pointer ( T* p )	{ pointer = p; }
		T*				get_pointer	() const	{ return pointer; }
		struct
		{
			T*			pointer;
			u32			counter;
		};
#else // #if XRAY_PLATFORM_32_BIT
		void			set_pointer ( T* p )	{ pointer = (T*)((u64)p | ((u64)counter << 40)); }
		T*				get_pointer	() const	{ return (T*)((u64)pointer ^ ((u64)counter << 40)); }

		private:
			T*			pointer;
		
		public:
			struct
			{
				u64		dummy	: 40;
				u64		counter	: 24;
			};
#endif // #if XRAY_PLATFORM_32_BIT
	};
#pragma warning(pop)
#else // #if defined(_MSC_VER)
	union free_list_type
	{
		void			set_pointer ( T* p )	{ pointer = p; }
		T*				get_pointer	() const	{ return pointer; }

		threading::atomic64_type	whole;
		struct
		{
			T*			pointer;
			u32			counter;
		};
	};
#endif // #if defined(_MSC_VER)

	typedef threading::atomic32_type	counter_type;

	static void		initialize	( free_list_type& free_list_head );
	static	T*		allocate	( free_list_type& free_list_head );
	static	void	deallocate	( free_list_type& free_list_head, free_list_type& freeing_node );
	
	static	void	increment	( counter_type& operand );
	static	void	decrement	( counter_type& operand );
}; // struct multi_threading_single_size_allocator_policy

} // namespace memory
} // namespace xray

#include <xray/memory_multi_threading_single_size_allocator_policy_inline.h>

#endif // #ifndef XRAY_MEMORY_MULTI_THREADING_SINGLE_SIZE_ALLOCATOR_POLICY_H_INCLUDED