////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_hdd_manager.h"
#include <xray/resources_query_result.h>

namespace xray {
namespace resources {

struct hdd_manager_sorting_predicate
{	
	bool operator ()					(query_result * r1, query_result * r2)
	{
		vfs::vfs_iterator const fat_it1	=	r1->get_fat_it();
		vfs::vfs_iterator const fat_it2	=	r2->get_fat_it();

		if ( fat_it1.is_end() && fat_it2.is_end() )
			return							r1 < r2;
		else if ( fat_it1.is_end() )	
			return							true;
		else if ( fat_it2.is_end() )	
			return							false;

		queries_result * r1_queries		=	r1->get_owner_queries();
		queries_result * r2_queries		=	r2->get_owner_queries();
		if ( r1_queries == r2_queries )
		{
			if ( r1->quality_index() > r2->quality_index() )
				return						true;
			else if ( r1->quality_index() < r2->quality_index() )
				return						false;
		}
		
		if ( fat_it1.is_archive() && fat_it2.is_archive() && 
			 fat_it1.get_physical_path() == fat_it2.get_physical_path() )
		{
			return							fat_it1.get_file_offs() < fat_it2.get_file_offs();
		}

		if ( !r1_queries && !r2_queries )
			return							r1 < r2;
		else if ( !r1_queries )
			return							(pvoid)r1 < (pvoid)r2_queries;
		else if ( !r2_queries )
			return							(pvoid)r1_queries < (pvoid)r2;
	
		return								r1_queries < r2_queries; 
	}
};

bool   hdd_manager::can_handle_query (pcstr path) 
{
	XRAY_UNREFERENCED_PARAMETER		(path);
	return							true; 
}

void   hdd_manager::push_query_impl (query_result* res)
{
	m_queries.push_back				(res);
}

void   hdd_manager::grab_sorted_queries (queries_list & out_queries)
{
	u32 queries_count					=	0;
	query_result * queries				=	m_queries.pop_all_and_clear(& queries_count);
	if ( !queries )
		return;

	typedef	buffer_vector<query_result *>	query_array_type;
	query_array_type						queries_array	(ALLOCA(queries_count * sizeof(query_result *)),
															 queries_count);

	u32 queries_pushed					=	0;
	for ( query_result *	it_query	=	queries;
							it_query;
							it_query	=	m_queries.get_next_of_object(it_query) )
	{
		queries_array.push_back				(it_query);
		++queries_pushed;
	}

	R_ASSERT_CMP							(queries_pushed, ==, queries_count);

	std::sort								(queries_array.begin(), queries_array.end(), hdd_manager_sorting_predicate());

	for ( query_array_type::iterator	it		=	queries_array.begin(),
										it_end	=	queries_array.end();
										it		!=	it_end;
										++it )
	{
		out_queries.push_back				(* it);
	}
}

} // namespace resources
} // namespace xray