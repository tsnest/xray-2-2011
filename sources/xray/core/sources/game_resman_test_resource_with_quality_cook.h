////////////////////////////////////////////////////////////////////////////
//	Created		: 13.09.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESMAN_TEST_RESOURCE_WITH_QUALITY_COOK_H_INCLUDED
#define GAME_RESMAN_TEST_RESOURCE_WITH_QUALITY_COOK_H_INCLUDED

#include "game_resman_test_resource.h"
#include <xray/resources_cook_classes.h>

namespace xray {
namespace resources {

class test_resource_with_quality_cook : public translate_query_cook
{
public:
	test_resource_with_quality_cook ();

	virtual	void		translate_query					(query_result_for_cook & );
	virtual void		delete_resource					(resource_base *);
	virtual u32			calculate_quality_levels_count	(query_result_for_cook const *);
	virtual float		satisfaction_with				(u32 quality_level, math::float4x4 const & user_matrix, u32 users_count);

};

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_TEST_RESOURCE_WITH_QUALITY_COOK_H_INCLUDED