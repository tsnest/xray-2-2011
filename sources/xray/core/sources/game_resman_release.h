////////////////////////////////////////////////////////////////////////////
//	Created		: 05.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESMAN_RELEASING_H_INCLUDED
#define GAME_RESMAN_RELEASING_H_INCLUDED

#include "game_resman_data.h"

namespace xray {
namespace resources {

class releasing_functionality : public core::noncopyable
{
public:

				releasing_functionality				(game_resources_manager_data & data) : m_data(data) {}

	void		release_resource					(resource_base * resource);
	bool		release_resources_by_class_id		(class_id_enum class_id);
	bool		release_all_resources				();

	template <class RemoveIfPredicate>
	void		release_resources_of_memory_type	(memory_type *, RemoveIfPredicate);

private:
	game_resources_manager_data &					m_data;
};



} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_RELEASING_H_INCLUDED