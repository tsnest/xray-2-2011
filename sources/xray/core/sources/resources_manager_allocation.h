////////////////////////////////////////////////////////////////////////////
//	Created		: 25.11.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_MANAGER_ALLOCATION_H_INCLUDED
#define RESOURCES_MANAGER_ALLOCATION_H_INCLUDED

namespace xray {
namespace resources {

enum reallocating_bool		{ reallocating_false, reallocating_true };

class allocate_functionality
{
public:
	void		prepare_final_resource		(query_result * query);
	void		prepare_raw_resource		(query_result * query, reallocating_bool reallocating);
	void		tick						(bool finalizing_thread);

private:
	void		prepare_raw_resource_for_managed_or_unmanaged_cook			(query_result * query, reallocating_bool reallocating);
	void		prepare_raw_resource_for_inplace_managed_cook				(query_result * query, reallocating_bool reallocating);
	void		prepare_raw_resource_for_inplace_unmanaged_cook				(query_result * query);
	void		continue_prepare_raw_resource_for_inplace_unmanaged_cook	(query_result * query);

	template <class query_list>
	void		allocate_final_resources		(query_list & queries, bool finalizing_thread);
	void		allocate_raw_resources			(thread_local_data * tls, bool finalizing_thread);

private:
	// we need this to prevent the following Code Analysis error
	// "function 'xray::intrusive_list<BaseWithMember,PointerType,MemberNext>::intrusive_list(void)' already has a body"
	//typedef intrusive_list<query_result, query_result *, &query_result::m_next_to_allocate_resource>	allocate_list;
	typedef intrusive_list<query_result, query_result *, &query_result::m_union_nexts>					allocate_list;
	allocate_list		m_queries_to_allocate_managed_resource;
};

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_MANAGER_ALLOCATION_H_INCLUDED