////////////////////////////////////////////////////////////////////////////
//	Created		: 15.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEST_2D_SHADER_OP_H_INCLUDED
#define TEST_2D_SHADER_OP_H_INCLUDED

#include "test_simple_shader_op.h"


namespace xray {
namespace graphics_benchmark {

////////////////////////////////////////////////////////////////////////////
class test_2d_shader_op: public test_simple_shader_op, 
						 public creator_base<test_2d_shader_op>
{
public:
	virtual bool initialize	( benchmark&, group_to_execute&, test_to_execute&, std::map<std::string,std::string>& parameters, std::string&);
	virtual void execute	( benchmark&, u32, u32, test_to_execute&, gpu_timer&);
}; // class test_2d_shader_op

} // namespace graphics_benchmark
} // namespace xray

#endif // #ifndef TEST_2D_SHADER_OP_H_INCLUDED