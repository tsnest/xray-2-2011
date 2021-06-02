////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "game_resource_manager.h"
#include "object.h"

namespace stalker2{

//void game_resource_manager::on_resource_loaded( resources::queries_result& data )
//{
//	R_ASSERT(!data.is_failed());
//
//	for(u32 i=0; i<data.size(); ++i)
//	{
//		resources::query_result_for_user& r	= data[i];
//
//		resources::unmanaged_resource_ptr gr = r.get_unmanaged_resource();
//		
//		game_queries_it it			= std::find(m_active_queries.begin(), m_active_queries.end(), data.unique_id());
//		if(it==m_active_queries.end())
//			it			= std::find(m_active_queries.begin(), m_active_queries.end(), -1); // sync call
//		
//		ASSERT						(it!=m_active_queries.end());
//
//		query_ q					= *it;
//		m_active_queries.erase		(it);
//		
//		if(q.customer)
//		{
//			q.callback				(gr);
//		}
//	}
//}

} // namespace stalker2
