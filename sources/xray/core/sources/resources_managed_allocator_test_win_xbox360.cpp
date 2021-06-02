#include "pch.h"

#include <xray/math_randoms_table.h>
#include "resources_managed_allocator.h"

#include <functional>
#include "member_to_functor.h"

#define USE_DL_PREFIX
#include "ptmalloc3/malloc-2.8.3.h"

#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMSG
#undef NOWINMESSAGES
#undef NOCTLMGR
#include <xray/os_include.h>

HANDLE		s_helper_thread_handle	=	NULL;
DWORD		s_helper_thread_id		=	0;

namespace xray		{
namespace resources {

fixed_vector<managed_resource*, 10000>					s_resources;
class tester_pinned_resource
{
public:
	void		pin (managed_resource * resource)
	{ 
		res		=	resource;
		pin_ptr	=	resource->pin();
	}
	void		unpin () { res->unpin(pin_ptr); }

	managed_resource *	res;
	pcbyte		pin_ptr;
	u32			unpin_time;
};

static u32 const	max_simultaneous_pins	=	50;
fixed_vector<tester_pinned_resource, max_simultaneous_pins>	s_pinned;

static threading::mutex*						s_mutex;

enum { helper_process_dead, helper_process_alive, helper_process_dying };
static long s_helper_process_state			=	helper_process_dead;

void   managed_resource_allocator::test_speed ()
{
	u32 const test_arena_size				=	128*1024*1024;
	u32 const test_temp_arena_size			=	16*1024*1024;
	u32 const test_reserved_size			=	8*1024*1024;

	LOGI_INFO("resources:test", "arena size: %d, max allocation size: %d", 
												 test_arena_size, 
												 test_temp_arena_size);

	R_ASSERT									(m_arena_size >= test_arena_size &&
												 m_temp_arena.m_arena_size >= test_temp_arena_size);
	R_ASSERT									(get_free_size() == m_arena_size);
	size_t const saved_arena_size			=	m_arena_size;
	size_t const saved_temp_arena_size		=	m_temp_arena.m_arena_size;
 	u32 const saved_reserved_size			=	m_reserved_size;
	m_arena_size							=	test_arena_size;
	m_free_size								=	m_arena_size;
	m_reserved_size							=	test_reserved_size;
	get_first_node()->m_size				=	m_free_size;
	m_temp_arena.m_arena_size				=	test_temp_arena_size;
	m_temp_arena.m_free_size				=	m_temp_arena.m_arena_size;
	m_temp_arena.get_first_node()->m_size	=	m_temp_arena.m_free_size;

	size_t const		max_allocation_size	=	m_temp_arena.total_size();
	u32 const		granularity				=	get_granularity();

	u32 const		dl_arena_size			=	test_arena_size * 3;
	pbyte 			dl_arena				=	(pbyte)RES_ALLOC(char, dl_arena_size);

	memory::doug_lea_allocator					dl_alloc;
	dl_alloc.initialize							(dl_arena, dl_arena_size, "dl_test_arena");

	math::randoms_table<10000>					randoms;

	pbyte			dl_max_addr				=	NULL;
	u32				dl_max_grow				=	0;

	for ( u32 test=0; test<2; ++test )
	{
		u64	iteration_index					=	0;
		randoms.to_first_random					();
		s_resources.clear						();
		u32	num_allocations					=	0;

		size_t free_size						=	total_size();

		timing::timer	timer;
		timer.start								();

		while ( xray::identity(true) )
		{
			if ( iteration_index == 1000 )
			{
				break;
			}

			u32	allocation_size				=	( ((randoms.next_random() * 1024) % max_allocation_size) 
												 / granularity + 1) * granularity;

			u32 const min_block_size		=	4096;
			if ( allocation_size < min_block_size )
			{
				allocation_size				=	min_block_size;
			}

			bool const do_deallocate		=	!(randoms.next_random() % 3) && s_resources.size();

			while ( do_deallocate || free_size - m_reserved_size < allocation_size )
			{
				u32 const index				=	randoms.next_random() % (u32)s_resources.size();
				managed_resource* 	res		=	s_resources[index];

				free_size					+=	res->m_node->m_size;
				
				if ( test == 0 )
				{
					deallocate					(s_resources[index]);
					R_ASSERT					(free_size == m_free_size);
				}
				else
				{
					managed_node*	node	=	res->m_node;
					res->~managed_resource		();

					XRAY_FREE_IMPL				(dl_alloc, res);
					XRAY_FREE_IMPL				(dl_alloc, node);
				}

				s_resources.erase				(s_resources.begin() + index);
				if ( do_deallocate )
				{
					break;
				}
			}

			if ( !do_deallocate )
			{
				++num_allocations;
				free_size					-=	allocation_size;

				managed_resource * res		=	NULL;
				if ( test == 0 )
				{
					res						=	allocate(allocation_size - sizeof(managed_node));
					R_ASSERT					(free_size == m_free_size);
				}
				else
				{
					managed_node*	node	=	(managed_node*)XRAY_ALLOC_IMPL(dl_alloc, char, 
												allocation_size);

					if ( (pbyte)node + allocation_size > dl_max_addr )
					{
						dl_max_addr			=	(pbyte)node + allocation_size;
						dl_max_grow			=	(u32)(dl_max_addr - dl_arena);
					}

					new (node)					managed_node(managed_node::allocated_node, allocation_size);
					res						=	(managed_resource*)XRAY_ALLOC_IMPL(dl_alloc, char, sizeof(managed_resource));

					if ( (pbyte)res > dl_max_addr )
					{
						dl_max_addr			=	(pbyte)res;
						dl_max_grow			=	(u32)(dl_max_addr - dl_arena);
					}

					fat_iterator it;
					it.set_end();
					new (res)					managed_resource(allocation_size);
					res->m_node				=	node;
				}

				R_ASSERT						(res);
				s_resources.push_back			(res);
			}
			
			//LOGI_INFO("resources:allocator", "test iteration: %d", iteration_index);
			++iteration_index;
			//log_nodes							();
		}

		LOGI_INFO("resources:test", "%s %d iterations ms: %d", 
												 !test ? "managed resource allocator" : "dlmalloc allocator",
												 (u32)iteration_index, 
												 timer.get_elapsed_ms());

		if ( test == 0 )
		{
			LOGI_INFO("resources:test", "%d defragmentations moved %d kbytes", 
												 m_defrag_iteration,
												 (u32)(m_whole_moved_bytes/1024));
			for ( u32 i=0; i<s_resources.size(); ++i )
			{
				deallocate						(s_resources[i]);
			}
		}
		else
		{
			for ( u32 i=0; i<s_resources.size(); ++i )
			{
				managed_resource *	res		=	s_resources[i];
				managed_node *	node		=	res->m_node;
				res->~managed_resource			();

				XRAY_FREE_IMPL					(dl_alloc, res);
				XRAY_FREE_IMPL					(dl_alloc, node);
			}

			LOGI_INFO("resources:test", "dl grow size: %d", (u32)dl_max_grow);
		}

	} // for 

	R_ASSERT									(m_free_size == m_arena_size);
	m_arena_size							=	saved_arena_size;
	m_free_size								=	m_arena_size;

	new ( get_first_node() )					managed_node(managed_node::free_node, m_arena_size);
	R_ASSERT									(m_temp_arena.m_free_size == m_temp_arena.m_arena_size);
	m_temp_arena.m_arena_size				=	saved_temp_arena_size;
	m_temp_arena.m_free_size				=	m_temp_arena.m_arena_size;
	m_temp_arena.get_first_node()->m_size	=	m_temp_arena.m_free_size;
	m_reserved_size							=	saved_reserved_size;

	RES_FREE									(dl_arena);
}

void   managed_resource_allocator::log_test_resources () const
{
	for ( u32 i=0; i<s_resources.size(); ++i )
	{
		managed_resource *	res				=	s_resources[i];
		LOGI_INFO("resources:test",			"resource: %d %s", 
											 i, 
											 check_valid_node(res->m_node) ? "main_arena" : "temp_arena");
	}
}

void   managed_resource_allocator::test ()
{
	u32 const test_arena_size				=	1024*1024;
	u32 const test_temp_arena_size			=	256*1024;
	u32 const test_reserved_size			=	0;

	R_ASSERT									(m_arena_size >= test_arena_size &&
												 m_temp_arena.m_arena_size >= test_temp_arena_size);
	R_ASSERT									(m_free_size == m_arena_size);
	size_t const saved_arena_size			=	m_arena_size;
	size_t const saved_temp_arena_size		=	m_temp_arena.m_arena_size;
	u32 const saved_reserved_size			=	m_reserved_size;
	m_arena_size							=	test_arena_size;
	m_free_size								=	m_arena_size;
	m_reserved_size							=	test_reserved_size;
	get_first_node()->m_size				=	m_free_size;
	m_temp_arena.m_arena_size				=	test_temp_arena_size;
	m_temp_arena.m_free_size				=	m_temp_arena.m_arena_size;
	m_temp_arena.get_first_node()->m_size	=	m_temp_arena.m_free_size;

	size_t const max_allocation_size		=	m_temp_arena.total_size();
	u32 const granularity					=	get_granularity();

	u32	iteration_index						=	0;
	s_resources.clear							();

	while ( xray::identity(true) )
	{
		s_mutex->lock							();
		u32 const num_pinned				=	(u32)s_pinned.size();
		s_mutex->unlock							();

		if ( !(iteration_index % 10) )
		{
			LOGI_INFO("resources:test",			"iteration: %d, num pinned: %d", iteration_index, num_pinned);
//			bool catcher = true;
		}

		if ( iteration_index == 8000 )
		{
			break;
		}

		u32	allocation_size					=	( ((rand() * 1024) % max_allocation_size) / granularity + 1) 
													* 
												granularity;

		u32 const min_block_size			=	64;
		if ( allocation_size < min_block_size )
		{
			allocation_size					=	min_block_size;
		}

		bool const do_deallocate			=	!(rand() % 3) && s_resources.size();

		while ( do_deallocate || m_free_size - m_reserved_size < allocation_size )
		{
			u32 const index					=	rand() % (u32)s_resources.size();
			remove_test_resource				(index);
		
			if ( do_deallocate )
			{
				break;
			}
		}

		if ( !do_deallocate )
		{
			managed_resource * res			=	allocate(allocation_size - sizeof(managed_node));
			R_ASSERT							(res);
			s_mutex->lock();
			s_resources.push_back				(res);
			s_mutex->unlock();
		}

		for ( u32 i=0; i<s_resources.size(); ++i )
		{
#ifndef MASTER_GOLD
			managed_resource *	res			=	s_resources[i];
			R_ASSERT							( check_valid_node(res->m_node) );
#endif // #ifndef MASTER_GOLD
		}
		
		check_consistency						();
		++iteration_index;
		//log_nodes								();
	}

	LOGI_INFO("resources:test", "%d defragmentations moved %d bytes", 
											 m_defrag_iteration-1,
											 (u32)m_whole_moved_bytes);

	for ( u32 i=0; i<s_resources.size(); ++i )
		deallocate								(s_resources[i]);

	R_ASSERT									(m_free_size == m_arena_size);
	m_arena_size							=	saved_arena_size;
	m_free_size								=	m_arena_size;
	new ( get_first_node() )					managed_node(managed_node::free_node, m_arena_size);
	R_ASSERT									(m_temp_arena.m_free_size == m_temp_arena.m_arena_size);
	m_temp_arena.m_arena_size				=	saved_temp_arena_size;
	m_temp_arena.m_free_size				=	m_temp_arena.m_arena_size;
	m_temp_arena.get_first_node()->m_size	=	m_temp_arena.m_free_size;
	m_reserved_size							=	saved_reserved_size;
}

#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMSG
#undef NOWINMESSAGES
#undef NOCTLMGR
#include <xray/os_include.h>

void   managed_resource_allocator::remove_test_resource (u32 const index)
{
	threading::mutex_raii						raii(*s_mutex);
	managed_resource*	res					=	s_resources[index];
	R_ASSERT									(res);
	s_resources.erase							(s_resources.begin() + index);

	for ( u32 i=0; i<s_pinned.size(); ++i )
	{
		if ( s_pinned[i].res == res )
		{
			s_pinned[i].res->unpin				(s_pinned[i].pin_ptr);
			s_pinned.erase						(s_pinned.begin() + i);
			--i;
		}
	}

	deallocate									(res);
}

static DWORD __stdcall	helper_thread_proc (pvoid)
{
	u32 const max_simultaneous_pins			=	10;
	u32 const max_pin_time					=	10;
#if XRAY_PLATFORM_WINDOWS
	PostQuitMessage								(0);
#endif // #if XRAY_PLATFORM_WINDOWS

	math::randoms_table<30000>					randoms;

	timing::timer	timer;
	timer.start									();

	while ( s_helper_process_state == helper_process_alive )
	{
		threading::mutex_raii					raii(*s_mutex);
		u32 const cur_time					=	timer.get_elapsed_ms();

		for ( u32 i=0; i<s_pinned.size(); ++i )
		{
			if ( cur_time >= s_pinned[i].unpin_time )
			{
				s_pinned[i].unpin				();
				s_pinned.erase					(s_pinned.begin() + i);
				--i;
			}
		}

		if ( !s_resources.size() || s_pinned.size() >= max_simultaneous_pins )
			continue;

		managed_resource*	res				=	s_resources[ randoms.next_random() % s_resources.size() ];
		R_ASSERT								(res);
		tester_pinned_resource new_pinned;
		new_pinned.pin							(res);
		new_pinned.unpin_time				=	cur_time + (randoms.next_random() % max_pin_time);

		s_pinned.push_back						(new_pinned);
	}

	s_helper_process_state					=	helper_process_dead;
	return										0;
}

void   run_helper_process ()
{
	s_helper_thread_id						=	0;
	s_helper_thread_handle					=	CreateThread(0, 
															 0, 
															 &helper_thread_proc, 
															 (pvoid)NULL, 
															 CREATE_SUSPENDED, 
															 &s_helper_thread_id);
	s_helper_process_state					=	helper_process_alive;

	SetThreadPriority							(s_helper_thread_handle, THREAD_PRIORITY_NORMAL);
	ResumeThread								(s_helper_thread_handle);
}

void   close_helper_process ()
{
	threading::interlocked_exchange				(s_helper_process_state, helper_process_dying);
	while ( s_helper_process_state != helper_process_dead )
	{
		;
	}
	CloseHandle									(s_helper_thread_handle);
}

void   managed_resource_allocator::test_defragment ()
{	
	//logging::push_rule							("core:resources:defrag", logging::silent);
	s_mutex									=	RES_NEW(threading::mutex);

 	run_helper_process							();
 	test										();
 	close_helper_process						();

#if XRAY_PLATFORM_WINDOWS
	PostQuitMessage								(0);
#endif // #if XRAY_PLATFORM_WINDOWS

	RES_DELETE									(s_mutex);
}

//----------------------------------------------------
// unmovables test
//----------------------------------------------------
struct unmovable_test_node
{
	managed_node*			node;
	u32						index;
	u32						unlock_time;
};

static u32 const			max_test_unmovables_nodes	=	7;
typedef fixed_vector<unmovable_test_node, max_test_unmovables_nodes> unmovable_test_container;
unmovable_test_container	s_unmovables_test_nodes;

void   managed_resource_allocator::test_unmovable_init (u32 const start_ms)
{
	u32 num_nodes							=	0;
	for ( managed_node*			cur_node	=	get_first_node(); 
												cur_node; 
								cur_node	=	cur_node->m_next )
	{
		if ( !cur_node->is_allocated() )
		{
			continue;
		}

		++num_nodes;
	}

	u32 const max_unmove_test_time			=	1000;
	u32 const num_test_unmovables			=	math::min(num_nodes, rand() % max_test_unmovables_nodes);

	LOGI_INFO("resources:test",					"testing with %d unmovables", num_test_unmovables);

	for ( u32 i=0; i<num_test_unmovables; ++i )
	{
		unmovable_test_node	test_node;
		test_node.unlock_time				=	start_ms + (rand() % max_unmove_test_time);

		u32 index							=	rand() % num_nodes;

 		for ( u32 j=0; j<i; ++j )
 		{
 			if ( s_unmovables_test_nodes[j].index == index )
			{
				index						=	(index + 1) % num_nodes;
				j							=	u32(-1);
				continue;
			}
 		}

		test_node.index						=	index;

		s_unmovables_test_nodes.push_back		(test_node);		
	}

	u32 index								=	0;
	for ( managed_node*			cur_node	=	get_first_node(); 
												cur_node; 
								cur_node	=	cur_node->m_next ) 
	{
		if ( !cur_node->is_allocated() )
		{
			continue;
		}

		for ( u32 i=0; i<s_unmovables_test_nodes.size(); ++i )
		{
			if ( s_unmovables_test_nodes[i].index == index )
			{
				s_unmovables_test_nodes[i].node		
											=	cur_node;
				R_ASSERT						(!cur_node->m_is_unmovable);
				cur_node->set_is_unmovable		(true);
				notify_unmovable_changed		(cur_node);
			}
		}

		++index;
	}
}

void   managed_resource_allocator::test_unmovable_unlock_expired (u32 const cur_ms)
{
	for ( u32 i=0; i<s_unmovables_test_nodes.size(); ++i )
	{
		if ( cur_ms > s_unmovables_test_nodes[i].unlock_time )
		{
			s_unmovables_test_nodes[i].node->set_is_unmovable	(false);
			notify_unmovable_changed			(s_unmovables_test_nodes[i].node);

			s_unmovables_test_nodes.erase		(s_unmovables_test_nodes.begin() + i);
			--i;
			continue;
		}
	}
}

void   managed_resource_allocator::test_unmovables_list_valid () const
{
	for ( u32 i=0; i<s_unmovables_test_nodes.size(); ++i )
	{
		R_ASSERT								(s_unmovables_test_nodes[i].node->m_is_unmovable);
	}
	managed_node*	node					=	get_first_node();
	while ( node )
	{
		unmovable_test_container::iterator	it	=
			std::find_if(s_unmovables_test_nodes.begin(),
				s_unmovables_test_nodes.end(),
				pass_member_to_functor(
				&unmovable_test_node::node, 
				std::bind1st(std::equal_to<managed_node*>(), node))
			);

		if ( node->m_is_unmovable )
		{
			R_ASSERT_U							(it != s_unmovables_test_nodes.end());
		}
		else
		{
			R_ASSERT_U							(it == s_unmovables_test_nodes.end());
		}

		node								=	node->m_next;
	}
}

} // namespace xray		
} // namespace resources 



// 	s_resources.push_back						( allocate(53-overhead,  0, NULL) );
// 	s_resources.push_back						( allocate(53-overhead,  0, NULL) ); 
// 	s_resources.push_back						( allocate(54-overhead,  0, NULL) ); 
// 	s_resources.push_back						( allocate(53-overhead,  0, NULL) ); 
// 	s_resources.push_back						( allocate(53-overhead,  0, NULL) );
// 	//s_resources.push_back						( allocate(53-overhead,  0, NULL) );
// 
// 	deallocate									( s_resources[0] );
// 	deallocate									( s_resources[3] );
// 
// 	s_resources[1]->set_is_unmovable			(true);
// 	s_resources[4]->set_is_unmovable			(true);
// 
// 	s_resources.erase							( s_resources.begin()+3 );
// 	s_resources.erase							( s_resources.begin()+0 );
// 
//  	for ( u32 i=0; i<s_resources.size(); ++i )
//  	{
//  		resource*	res						=	s_resources[i];
//  		pbyte data							=	const_cast<pbyte>( res->pin() );
//  		*data								=	(char)i;
//  		res->unpin								(data);
//  	}
// 
// 	check_consistency							();
// 	log_nodes									();
// 
// 	allocate									(110-overhead, 0, NULL); 
// 
// 	for ( u32 i=0; i<s_resources.size(); ++i )
// 	{
// 		resource*	res						=	s_resources[i];
// 		pbyte data							=	const_cast<pbyte>( res->pin() );
// 		LOG_INFO								("%d", (int)*data);
// 		res->unpin								(data);
// 	}
// 
// 	check_consistency							();
// 	log_nodes									();
// 
// class log_table
// {
// public:
// 	log_table(pstr data, u32 size) : m_data(data), m_size(size) {}
// 	
// 	void	add_cell	(pcstr format, ...)
// 	{
// 		
// 	}
// 
// 	void	next_row	()
// 	{
// 		
// 	}
// 
// private:
// 	u32				m_size;
// 	pstr			m_data;
// 	pstr			m_cur;
// };


