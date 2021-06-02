////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_QUERIES_RESULT_H_INCLUDED
#define XRAY_RESOURCES_QUERIES_RESULT_H_INCLUDED

#include <xray/type_uid_object.h>
#include <xray/resources_query_result.h>

namespace xray {
namespace resources {

class XRAY_CORE_API queries_result : public uid_object<queries_result>
{
public:
	u32						size						() const;
	bool					empty						() const;

	memory::base_allocator *	get_user_allocator		() const { return m_allocator; }

	query_result_for_user &			operator []			(u32 index);
	query_result_for_user const &	operator []			(u32 index) const;

	bool					is_successful				() const;
	
	query_result_for_cook *	get_parent_query			() const { return m_parent_query; }
	assert_on_fail_bool		assert_on_fail				() const { return m_assert_on_fail; }

private:
							queries_result				(u32						num_queries, 
														 query_callback				callback,
														 memory::base_allocator*	allocator,
														 u32						user_thread_id,
														 query_result_for_cook*		parent,
														 float *					target_satisfactions,
														 bool const *				disable_cache,
														 u32 const *				quality_indexes,
														 query_type_enum			queries_type,
														 autoselect_quality_bool const *	autoselect_quality,
														 assert_on_fail_bool		assert_on_fail);
							~queries_result				();

	bool					is_finished					() const;
	void    				on_child_query_end			(query_result* res, bool result);
	bool					calculate_result_from_children	();
	void    				on_query_end				(bool result);
	void					query_fs_iterators			();
	void					set_result					(bool result);
	query_result &			at							(u32 const index)		{ R_ASSERT(index < m_size); return m_queries[index]; }
	query_result const &	at							(u32 const index) const { R_ASSERT(index < m_size); return m_queries[index]; }
	void					translate_request_paths		();
	void					end_and_delete_self			(bool finalizing_thread);
	void						mark_inconsistent_qualities_as_failed ();
	bool							quality_loaded		(u32 quality);
	void						call_user_callback		();
	void						push_to_grm_cache		();

	void							increment_reference_count	() { threading::interlocked_increment(m_reference_count); ASSERT_CMP(m_reference_count, <=, 2); }
	threading::atomic32_value_type	decrement_reference_count	() { ASSERT_CMP(m_reference_count, >, 0); return threading::interlocked_decrement(m_reference_count); }

	u32						calculate_fs_iterator_requests_count	();
	void					on_fs_iterator_ready		(vfs::vfs_locked_iterator const & it, query_result * query);

private:
	query_callback				m_callback;
	query_result_for_cook *		m_parent_query;
	queries_result *			m_next_ready;
	memory::base_allocator*		m_allocator;
	threading::atomic32_type	m_reference_count;
	threading::atomic32_type	m_children_ended;
	u32							m_fs_iterator_requests_left;
	u32							m_size;
	u32							m_thread_id;
	enum result_enum			{ result_uninitialized	=	u32(-1), result_failed = 0, result_success = 1 };
	threading::atomic32_type	m_result;

	bool						m_is_queries_for_quality;
	assert_on_fail_bool			m_assert_on_fail;

#if defined(_MSC_VER)
#	pragma warning (push)
#	pragma warning (disable:4200)
#endif // #if defined(_MSC_VER)
	query_result				m_queries[];
#if defined(_MSC_VER)
#	pragma warning (pop)
#endif // #if defined(_MSC_VER)

	friend	class				query_result;
	friend	class				resources_manager;
	friend	class				thread_local_data;
	friend	class				query_resources_helper;
};

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_QUERIES_RESULT_H_INCLUDED