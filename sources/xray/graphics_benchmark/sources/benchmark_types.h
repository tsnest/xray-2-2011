////////////////////////////////////////////////////////////////////////////
//	Created		: 18.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BENCHMARK_TYPES_H_INCLUDED
#define BENCHMARK_TYPES_H_INCLUDED

#include "benchmark_test.h"
#include "counters_result.h"

namespace xray {
namespace graphics_benchmark {

	typedef benchmark_test*			(*creator_func_type)();

	enum benchmark_target_enum
	{
		target_unknown,
		target_general,
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

	struct render_target
	{
		u32								width,
										height;
		std::string						format;
		std::string						name;
		xray::render::ref_rt		rt;
		bool							is_valid;
		//xray::render::ref_texture  texture;
	};

	struct test_to_execute
	{
		std::string		class_name,
						test_name,
						test_comments,
						group_name,
						name_of_base;

		std::vector<render_target>		render_targets;
		render_target					depth_rt;

		std::map<std::string, std::string> parameters;

		u32				num_passes;
		s32				order;
		
		bool			is_base;
		bool			is_color_write_enable;
		bool			is_stencil_enable;
			
		benchmark_test	*object;
		counters_result result;

		std::string get_ps_outs_code ()
		{
			const u32 num_rts = render_targets.size();
			if (num_rts<2)
				return "out float4 result : SV_Target";
			else
			{
				std::string result;
				for ( u32 i=0; i<num_rts; i++)
				{
					char buffer[64];
					::sprintf(buffer,"out float4 result%d : SV_Target%d",i,i);
					result+=buffer;
					if (i!=num_rts-1) result+=",\n";
				}
				return result;
			}
		}
		std::string get_comments(u32 const max_length)
		{
			std::string result;
			
			for (u32 i=0; i<max_length; i++)
			{
				if (i == test_comments.length() || test_comments[i]=='\n') 
					break;
				result.push_back( test_comments[i] );
			}
			
			if (test_comments.length()>max_length)
				result+="...";
			
			//for (u32 i=0; i<result.length(); i++)
			//	if (result[i]=='\n') result[i]=' ';
			
			return result;
		}
		friend bool operator < (test_to_execute A, test_to_execute B)
		{
			if (A.is_base)
				return true;
			
			if (A.order >= 0 && B.order >= 0)
				return A.order < B.order;
			
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
		bool						 has_base;
		bool						 enabled;

		s32							 order;

		friend bool operator < (group_to_execute A, group_to_execute B)
		{
			if (A.order >= 0 && B.order >= 0)
				return A.order < B.order;

			return A.group_name < B.group_name;
		}

	};

} // namespace graphics_benchmark
} // namespace xray

#endif // #ifndef BENCHMARK_TYPES_H_INCLUDED