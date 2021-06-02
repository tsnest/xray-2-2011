////////////////////////////////////////////////////////////////////////////
//	Created		: 13.09.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESMAN_TEST_RESOURCE_WITH_QUALITY_H_INCLUDED
#define GAME_RESMAN_TEST_RESOURCE_WITH_QUALITY_H_INCLUDED

#include "game_resman_test_resource.h"

namespace xray {
namespace resources {

class test_resource_with_quality : public test_resource
{
public:
	enum				{ max_quality_levels_count	=	3 };

public:
						test_resource_with_quality	(pcstr name);
						~test_resource_with_quality	();

			bool		add_quality					(test_resource_ptr child, u32 quality);

	virtual bool		is_increasing_quality		() const;
	virtual void		increase_quality_to_target	(query_result_for_cook * parent_query);
	virtual void		decrease_quality			(u32 new_best_quality);
	void				on_quality_loaded			(queries_result & resources);

private:
	class_id_enum									m_qualities_types[max_quality_levels_count];
	bool											m_increasing_quality;
	test_resource_ptr::child_type					m_qualities	[max_quality_levels_count];
	query_result_for_cook *							m_parent_query;

}; // class game_resman_test_resource_with_quality

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_TEST_RESOURCE_WITH_QUALITY_H_INCLUDED