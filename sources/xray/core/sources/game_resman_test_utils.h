////////////////////////////////////////////////////////////////////////////
//	Created		: 13.09.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESMAN_TEST_UTILS_H_INCLUDED
#define GAME_RESMAN_TEST_UTILS_H_INCLUDED

#include <xray/math_randoms_generator.h>

namespace xray {
namespace resources {

extern math::random32	s_random;
static class_id_enum const s_class_ids[]	=	{ test_resource_class1, test_resource_class2, test_resource_class3 };

enum	log_test_resource_enum {	log_test_resource_created, 
									log_test_resource_no_user_reference, 
									log_test_resource_zero_reference, 
									log_test_resource_destroyed				};

class	test_resource;
void	log_test_resource	(log_test_resource_enum , test_resource *);
pcstr   test_allocator_name (class_id_enum);

typedef fixed_string<512>			log_string_type;
log_string_type		log_string		(resource_base * );

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_TEST_UTILS_H_INCLUDED