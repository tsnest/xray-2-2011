////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_manager.h"
#include <xray/resources_queries_result.h>

namespace xray {
namespace resources {

//----------------------------------------------------------
// queries_result
//----------------------------------------------------------

queries_result::queries_result (u32 const						num_queries, 
								query_callback const			callback, 
								memory::base_allocator* const	allocator,
								u32	const						user_thread_id,
								query_result_for_cook* const	parent,
								float *							target_satisfactions,
								bool const *					disable_cache,
								u32 const *						quality_indexes,
								query_type_enum					queries_type,
								autoselect_quality_bool const *	autoselect_quality,
								assert_on_fail_bool				assert_on_fail) 
								:	m_size						(num_queries), 
									m_callback					(callback), 
									m_result					(result_uninitialized),
									m_allocator					(allocator),
									m_thread_id					(threading::current_thread_id()),
									m_next_ready				(NULL),
									m_parent_query				(parent),
									m_reference_count			(1),
									m_children_ended			(0),
									m_is_queries_for_quality	(false),
									m_assert_on_fail			(assert_on_fail)
{
	for ( u32 i=0; i<num_queries; ++i )
	{
		if ( quality_indexes && quality_indexes[i] != 0 )
			m_is_queries_for_quality		=	true;

		new ( &m_queries[i] ) query_result	(query_result::flag_load, 
											 this, 
											 allocator, 
											 user_thread_id, 
											 target_satisfactions ? target_satisfactions[i] : 0,
											 disable_cache ? disable_cache[i] : false,
											 quality_indexes ? quality_indexes[i] : 0,
											 queries_type,
											 autoselect_quality ? autoselect_quality[i] : autoselect_quality_false);
	}

	g_resources_manager->on_added_queries	(m_size);
}

queries_result::~queries_result () 
{
	for ( u32 i=0; i<m_size; ++i )
		m_queries[i].~query_result();

	g_resources_manager->on_dispatched_queries(m_size);
}

u32   queries_result::size () const
{ 
	return									m_size;			
}

bool queries_result::empty () const
{ 
	return									!m_size;
}

query_result_for_user &   queries_result::operator [] (u32 index)			
{
	return									m_queries[index];	
}

const query_result_for_user &   queries_result::operator [] (u32 index) const	
{
	return									m_queries[index];	
}

void   queries_result::set_result		(bool result)
{
	R_ASSERT_CMP							(m_result, ==, result_uninitialized);
	//LOG_INFO								("queries_result::set_result %d %d %d", m_unique_id, (u32)result, threading::current_thread_id());
	if ( result )
		threading::interlocked_exchange		(m_result, result_success);
	else
		threading::interlocked_exchange		(m_result, result_failed);
}

bool   queries_result::is_successful	() const
{
	R_ASSERT_CMP							(m_result, !=, result_uninitialized);
	return									m_result == result_success;
}

bool   queries_result::is_finished		() const
{
	return									m_result != result_uninitialized;
}

bool   queries_result::calculate_result_from_children ()
{
	bool		res						=	true;
	for ( u32 i=0; i<size(); ++i )
	{
		query_result const & cur		 =	at(i);
		R_ASSERT							(cur.is_finished());
		res								&=	cur.is_successful();
	}

	return									res;
}

void   queries_result::on_child_query_end (query_result * const	child, bool const result)
{
	XRAY_UNREFERENCED_PARAMETERS			(child, result);

	if ( unmanaged_resource_ptr unmanaged_resource = child->get_unmanaged_resource() )
		R_ASSERT							(unmanaged_resource->get_deleter_object());

	u32 const children_ended			=	(u32)threading::interlocked_increment(m_children_ended);
	R_ASSERT_CMP							(children_ended, <=, size());

	u32 fs_iterator_requests_counts		=	calculate_fs_iterator_requests_count();

	if ( children_ended == size() - fs_iterator_requests_counts )
	{
		if ( fs_iterator_requests_counts )
		{
			query_fs_iterators				();
			return;
		}

		bool const queries_result		=	calculate_result_from_children();
		on_query_end						(queries_result);		
	}
}

void   queries_result::on_query_end		(bool const result)
{
	ASSERT								(!is_finished());
	set_result							(result);

	if ( decrement_reference_count() == 0 )
		g_resources_manager->on_query_finished	(this);
}

u32    queries_result::calculate_fs_iterator_requests_count	()
{
	u32 fs_iterator_requests_counts		=	0;
	for ( u32 i=0; i<m_size; ++i )
	{
		if ( m_queries[i].is_fs_iterator_query() )
			++fs_iterator_requests_counts;
	}

	return									fs_iterator_requests_counts;
}

void   queries_result::on_fs_iterator_ready	(vfs::vfs_locked_iterator const & it, query_result * query)
{
	query->set_result_iterator				(it);

	--m_fs_iterator_requests_left;
	if ( !m_fs_iterator_requests_left )
	{
		bool const queries_result		=	calculate_result_from_children();
		on_query_end						(queries_result);
	}
}

void   queries_result::query_fs_iterators	()
{
	m_fs_iterator_requests_left			=	calculate_fs_iterator_requests_count();
	u32 size							=	m_size; // this copy is intended not to check dead object
	for ( u32 i=0; i<size; ++i )
	{
		query_result * const query		=	& m_queries[i];
		if ( !query->is_fs_iterator_query() )
			continue;

		pcstr const request_path		=	query->get_requested_path();
		recursive_bool const recursive	=	(recursive_bool)(query->get_class_id() == fs_iterator_recursive_class);
		query_vfs_iterator					(
			request_path, 
			boost::bind(& queries_result::on_fs_iterator_ready, this, _1, query),
			& memory::g_mt_allocator,
			recursive,
			get_parent_query()
		);
	}	
}

void   queries_result::push_to_grm_cache ()
{
	for ( u32 i=0; i<m_size; ++i )
	{
		query_result * const query	=	& at(i);
		if ( query->is_fs_iterator_query() )
			continue;

		if ( resource_base * resource_to_cache = query->resource_for_grm_cache() )
			if ( query->is_successful() )
			{
				g_resources_manager->on_query_finished_callback	(resource_to_cache);
			}
			else
			{
				resource_to_cache->cast_base_of_intrusive_base()->unpin_reference_count_for_query_finished_callback	();
			}
	}
}

void   queries_result::call_user_callback ()
{
	thread_local_data * const tls		=	g_resources_manager->get_thread_local_data(threading::current_thread_id(), true);
	++tls->disable_translate_query_counter_check;

	m_callback						(*this);

	--tls->disable_translate_query_counter_check;
}

bool   queries_result::quality_loaded (u32 const quality)
{
	for ( u32 i=0; i<m_size; ++i )
		if ( m_queries[i].quality_index() == quality )
			return					m_queries[i].is_successful();

	NOT_IMPLEMENTED					(return false);
}

void   queries_result::mark_inconsistent_qualities_as_failed ()
{
	R_ASSERT								(m_is_queries_for_quality);
	u32 * qualities						=	(u32 *)ALLOCA(m_size * sizeof(u32));

	for ( u32 i=0; i<m_size; ++i )
		qualities[i]					=	m_queries[i].quality_index();

	std::sort								(qualities, qualities + m_size);

	u32 best_quality_loaded				=	u32(-1);
	for ( int i=m_size-1; i>=0; --i )
	{
		if ( !quality_loaded(qualities[i]) )
			break;
		best_quality_loaded				=	i;
	}

	for ( u32 i=0; i<m_size; ++i )
	{
		query_result & query			=	m_queries[i];
		u32 const index					=	query.quality_index();
		if ( index < best_quality_loaded && query.is_successful() )
			query.set_error_type			(query_result::error_loaded_but_inconsistent_quality);
	}
}

void   queries_result::end_and_delete_self (bool finalizing_thread)
{
	if ( !finalizing_thread )
	{
		if ( m_is_queries_for_quality )
			mark_inconsistent_qualities_as_failed	();

		call_user_callback					();
		push_to_grm_cache					();
	}

	this->~queries_result					();
	queries_result const * this_ptr		=	const_cast<queries_result *>(this);
	XRAY_FREE_IMPL							(m_allocator, this_ptr);
}

void   queries_result::translate_request_paths ()
{
	for ( u32 i=0; i<size(); ++i )
	{
		query_result & query			=	at(i);
		if ( query.creation_data_from_user() )
			continue;
		if ( query.is_fs_iterator_query() )
			continue;

		query.translate_request_path		();
	}
}

} // namespace xray
} // namespace resources