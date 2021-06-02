////////////////////////////////////////////////////////////////////////////
//	Created		: 30.07.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESMAN_QUALITY_QUEUE_H_INCLUDED
#define GAME_RESMAN_QUALITY_QUEUE_H_INCLUDED

namespace xray {
namespace resources {

class resource_freeing_functionality;
class resources_to_free_collection;

enum decrease_is_possible_bool { decrease_is_possible_false, decrease_is_possible_true };

class quality_decreasing_functionality : public core::noncopyable
{
private:
	typedef			resources_to_free_collection::resources_to_free_list	quality_resources_list;
public:
					quality_decreasing_functionality	(resource_freeing_functionality & freeing_functionality, 
														 float lowest_satisfaction_level);

	bool			try_decrease						(resource_base * resource);

private:
	decrease_is_possible_bool collect_quality_resources	(resource_base * resource, 
														 quality_resources_list * out_quality_resources);

	void			decrease_for_parents				(resource_base * top_resource);
	static void		prepare_satisfaction_tree			();

private:
	resource_freeing_functionality &				m_freeing_functionality;
	float											m_lowest_satisfaction_level;
};

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_QUALITY_QUEUE_H_INCLUDED