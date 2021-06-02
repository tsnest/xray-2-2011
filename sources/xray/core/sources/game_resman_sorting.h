////////////////////////////////////////////////////////////////////////////
//	Created		: 10.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESMAN_SORTING_H_INCLUDED
#define GAME_RESMAN_SORTING_H_INCLUDED

#include "game_resman_data.h"

namespace xray {
namespace resources {

class sorting_functionality : public core::noncopyable
{
public:
				sorting_functionality		(game_resources_manager_data & data);

	void		sort_resources_if_needed	(memory_type *);
private:
	void			log_memory_type_resources	(memory_type *);

private:
	u64										m_sort_actuality_tick;
	game_resources_manager_data &			m_data;
};

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_SORTING_H_INCLUDED