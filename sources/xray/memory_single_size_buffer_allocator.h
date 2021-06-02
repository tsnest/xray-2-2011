////////////////////////////////////////////////////////////////////////////
//	Created		: 14.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_SINGLE_SIZE_BUFFER_ALLOCATOR_H_INCLUDED
#define XRAY_MEMORY_SINGLE_SIZE_BUFFER_ALLOCATOR_H_INCLUDED

#include <xray/memory_single_threading_single_size_allocator_policy.h>
#include <xray/memory_multi_threading_single_size_allocator_policy.h>

namespace xray {
namespace memory {

template < typename T, typename NodeType >
struct select_allocator_policy
{
	typedef	multi_threading_single_size_allocator_policy< NodeType >	type;
};

template < typename NodeType >
struct select_allocator_policy< threading::single_threading_policy, NodeType >
{
	typedef	single_threading_single_size_allocator_policy< NodeType >	type;
};

template < int DataSize, class ThreadingPolicy >
class single_size_buffer_allocator : public boost::noncopyable
{
	template < class T, class ThreadPolicy >
	friend class fixed_size_allocator;

public:
	inline				single_size_buffer_allocator	( pvoid arena, size_t arena_size );
	inline	pvoid		allocate						( );
	inline	void		deallocate						( pvoid& pointer );
	inline	size_t		total_size						( ) const { return	m_max_count * DataSize; }
	inline	size_t		allocated_size					( ) const { return	m_allocated_count * DataSize; }
	
	// complex signatures just for support of XRAY_NEW_IMPL/XRAY_DELETE_IMPL calls
	inline	pvoid		malloc_impl						( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	inline	void		free_impl						( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );

	inline	void		swap							( single_size_buffer_allocator& other );

private:
//	Dima: uncomment this in case you _really_ need unaligned to sizeof(pvoid) structures
//	#pragma pack( push )
//	#pragma pack( 1 )
	union node {
		node*			next;
		char			data[DataSize];
	}; // union node
//	#pragma pack( pop )

	typedef typename select_allocator_policy< ThreadingPolicy, node >::type	PolicyType;
	typedef typename PolicyType::free_list_type								free_list_type;
	typedef typename PolicyType::counter_type								counter_type;

	enum {
		element_size	= sizeof( node ),
	};

private:
	free_list_type	m_free_list_head;
	counter_type	m_allocated_count;
	size_t			m_max_count;
};	// class single_size_buffer_allocator

} // namespace memory
} // namespace xray

#include <xray/memory_single_size_buffer_allocator_inline.h>

#endif // #ifndef XRAY_MEMORY_SINGLE_SIZE_BUFFER_ALLOCATOR_H_INCLUDED