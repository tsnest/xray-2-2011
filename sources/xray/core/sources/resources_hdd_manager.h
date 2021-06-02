////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_HDD_MANAGER_H_INCLUDED
#define RESOURCES_HDD_MANAGER_H_INCLUDED

#include "resources_helper.h"
#include "resources_device_manager.h"
#include <queue>

namespace xray {
namespace resources {

class hdd_manager : public device_manager
{
public:
							hdd_manager				(u32 sector_size) : 
													device_manager(sector_size) {}

	virtual bool			can_handle_query		(pcstr path);
	virtual void			push_query_impl			(query_result* res);
	virtual void			grab_sorted_queries		(queries_list & out_queries);

private:
	typedef					intrusive_list< query_result, 
											query_result *, 
											& query_result::m_next_in_device_manager, 
											threading::mutex, 
											size_policy >	queries_container;

	queries_container		m_queries;
};

} // namespace xray
} // namespace resources

#endif // RESOURCES_HDD_MANAGER_H_INCLUDED