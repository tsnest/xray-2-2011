////////////////////////////////////////////////////////////////////////////
//	Created		: 29.07.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESMAN_FREE_H_INCLUDED
#define GAME_RESMAN_FREE_H_INCLUDED

#include "game_resman_free_collection.h"
#include "game_resman_data.h"

namespace xray {
namespace resources {

class zero_outer_reference_checker;

class resource_freeing_functionality : public core::noncopyable
{
public:
					resource_freeing_functionality		(game_resources_manager_data & data,
														 resources_to_free_collection &	collection) 
					: m_data(data), m_collection(collection) {}

	void			free_collected						();
	bool			try_collect_to_free					();

	bool			try_collect_to_free_resource		(resource_base * resource);

	resources_to_free_collection & get_collection		() { return m_collection; }

	void   			release_sub_fat						(vfs_sub_fat_resource * sub_fat);

private:
	void   			release_sub_fat_from_parents		(vfs_sub_fat_resource * sub_fat);

	resource_base * try_collect_next					();
	void			collect_to_free						(resource_base * resource);
	bool			try_collect_parents_to_free			(resource_base * resource);
	bool			can_be_freed						(resource_base * resource, bool * can_try_free, bool * can_try_decrease_quality);
	bool			parents_can_be_freed				(resource_base * resource, bool * can_try_free, bool * can_try_decrease_quality);

	game_resources_manager_data &	data				() { return m_data; }

private:
	resources_to_free_collection &						m_collection;
	game_resources_manager_data &						m_data;

	friend class										quality_decreasing_functionality;
};

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_FREE_H_INCLUDED