////////////////////////////////////////////////////////////////////////////
//	Created		: 18.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BENCHMARK_NESTED_TYPES_H_INCLUDED
#define BENCHMARK_NESTED_TYPES_H_INCLUDED

#include "benchmark_test.h"
#include "counters_result.h"

namespace xray {
namespace graphics_benchmark {

	typedef benchmark_test*			(*creator_func_type)();

	enum benchmark_target_enum
	{
		target_none,
		target_ati,
		target_nvidia,
		target_xbox,
		target_ps3
	};

	struct benchmark_class
	{
		creator_func_type	creator;
		std::string			name;
		benchmark_class*	next;
	};

	struct test_to_execute
	{
		std::string		class_name,
						test_name,
						test_comments;
		
		std::map<std::string, std::string> parameters;
		
		u32				num_passes,
						group_pass_index;
		
		benchmark_test	*object;
		counters_result result;

		friend bool operator < (test_to_execute A, test_to_execute B)
		{
			return A.test_name < B.test_name;
		}
	};

	struct group_to_execute
	{
		std::string					 group_name;
		u32							 num_circles;
		u32							 num_skipped_circles;
		std::vector<test_to_execute> tests;
		counters_result				 base_result;
		bool						 has_base_result;
		
		friend bool operator < (group_to_execute A, group_to_execute B)
		{
			return A.group_name < B.group_name;
		}
	};

} // namespace BENCHMARK_NESTED_TYPES_H_INCLUDED
} // namespace xray



#endif // #ifndef BENCHMARK_NESTED_STRUCTURES_H_INCLUDED