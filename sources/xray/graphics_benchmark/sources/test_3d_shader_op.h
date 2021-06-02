////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEST_3D_SHADER_OP_H_INCLUDED
#define TEST_3D_SHADER_OP_H_INCLUDED

#include "test_simple_shader_op.h"

namespace xray {
namespace graphics_benchmark {


class test_3d_shader_op: public test_simple_shader_op, public creator_base<test_3d_shader_op>
{
public:
	virtual bool initialize			   ( benchmark&, group_to_execute&, test_to_execute&, std::map<std::string,std::string>&, std::string&);
	virtual void execute			   ( benchmark&, u32, u32, test_to_execute&, gpu_timer&);
			void on_test_visual_loaded ( xray::resources::queries_result& data);
private:
	xray::render::ref_effect	 m_sh;
	u32								 m_mesh_index;
	xray::math::float4x4			 m_sphere_matrix;
}; // class test_3d_shader_op

} // namespace graphics_benchmark
} // namespace xray

#endif // #ifndef TEST_3D_SHADER_OP_H_INCLUDED