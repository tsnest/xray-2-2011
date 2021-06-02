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

class quality_decreasing_functionality : public core::noncopyable
{
public:
				quality_decreasing_functionality	(resource_freeing_functionality & freeing_functionality, 
													 float lowest_satisfaction_level);

	bool		collect_by_decreasing_quality		();

private:
	bool		try_decrease						(resource_base *	resource);
	bool		decrease_is_possible				(resource_base *	resource);
	void		decrease_for_parents				(resource_base *	top_resource);
	static void	prepare_satisfaction_tree			();

private:
	resources_to_free_collection &					m_collection;
	resource_freeing_functionality &				m_freeing_functionality;
	float											m_lowest_satisfaction_level;
};

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_QUALITY_QUEUE_H_INCLUDED