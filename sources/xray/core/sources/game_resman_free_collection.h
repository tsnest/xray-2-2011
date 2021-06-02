////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESMAN_COLLECTION_TO_FREE_H_INCLUDED
#define GAME_RESMAN_COLLECTION_TO_FREE_H_INCLUDED

namespace xray {
namespace resources {

class resources_to_free_collection
{
public:
	resources_to_free_collection (memory_usage_type const *		required_memory, 
								  memory_type *					info, 
								  query_result *				query)
	: info(info), query(query) 
	{ 
		if ( required_memory )
			this->required_memory	=	* required_memory;
		collected_memory			=	this->required_memory; 
		collected_memory.size		=	0;
	}

	typedef	intrusive_list<resource_base, resource_base *, & resource_base::m_next_for_grm_observer_list, threading::single_threading_policy>
										resources_to_free_list;

	resources_to_free_list				resources;
	memory_usage_type					collected_memory;
	memory_usage_type					required_memory;
	memory_type *						info;
	query_result *						query;
};

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_COLLECTION_TO_FREE_H_INCLUDEDnamespace xray {
