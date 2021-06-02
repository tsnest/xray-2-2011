////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "test_3d_shader_op.h"

#include "benchmark.h"
#include "benchmark_renderer.h"
#include "benchmark_helpers.h"
#include "xray/ogf.h"
#include "sphere_primitive.h"

//#include "xray/render/engine/visual.h"
#include "xray/render/engine/model_manager.h"

namespace xray {
namespace graphics_benchmark {


using namespace xray::render;
using namespace xray::math;

const D3D_INPUT_ELEMENT_DESC vertex_descs[][9] = 
{
	{	//24
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
	},
	{	//32
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	1, DXGI_FORMAT_R32G32_FLOAT,	0, 24,	D3D_INPUT_PER_VERTEX_DATA, 0},
	},
	{	//40
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 24,	D3D_INPUT_PER_VERTEX_DATA, 0},
	},
	{	//48
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32,	D3D_INPUT_PER_VERTEX_DATA, 0},
	},
	{	//64
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	2, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48,	D3D_INPUT_PER_VERTEX_DATA, 0},
	},
	{	//96
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	2, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	3, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 64,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	4, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 80,	D3D_INPUT_PER_VERTEX_DATA, 0},
	},
	{	//128
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	2, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	3, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 64,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	4, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 80,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	5, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 96,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	6, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 112,	D3D_INPUT_PER_VERTEX_DATA, 0},
	}
};

class render_visual
{
public:
	render_visual();
	virtual	~render_visual();

	virtual	void						load(memory::chunk_reader& )
	{
		//if(chunk.chunk_exists( model_chunk_gcontainer ))
		{
			/*memory::reader reader	= chunk.open_reader( model_chunk_gcontainer );

			vertex_buffer_id		= reader.r_u32();
			vertex_base				= reader.r_u32();
			vertex_count			= reader.r_u32();

			index_buffer_id			= reader.r_u32();
			index_base				= reader.r_u32();
			index_count				= reader.r_u32();

			primitive_count			= index_count/3;
			geom					= xray::render::model_manager::ref().(vertex_buffer_id, index_buffer_id);*/
		}
	}
	virtual	void						render();
	virtual	void						render_selected();

	ref_geometry	geom;

	u32			vertex_buffer_id;
	u32			vertex_base;
	u32			vertex_count;

	u32			index_buffer_id;
	u32			index_base;
	u32			index_count;
	u32			primitive_count;

	u16	type;
	u32	flags;

	math::sphere	m_sphere;
	math::aabb		m_aabb;

	ref_effect		m_effect;
	ref_effect		m_notexture_shader;
};//class visual


void test_3d_shader_op::on_test_visual_loaded	( xray::resources::queries_result& data)
{
	(void)&data;

	ASSERT							( !data.empty());

	resources::pinned_ptr_const<u8>	model_data	( data[ 0 ].get_managed_resource());

	memory::chunk_reader		chunk_reader( model_data.c_ptr(), model_data.size(), memory::chunk_reader::chunk_type_sequential);

	//u16 type					= model_manager::get_visual_type( chunk_reader);
	//simple_visual*			s_visual;
	//s_visual				= NEW(simple_visual)();////xray::render::engine::model_manager::create_instance( type);
	//s_visual->load			( chunk_reader);

	//effect_3d_shader tb;
	//s_visual->m_effect = effect_manager::ref().create_effect( &tb);
}

template<u32 num_uv_channels=1> struct colored_vertex_sized
{
	math::float3	position;
	u32				color;
	math::float2	uv[num_uv_channels];
}; 
static res_geometry* s_geom = 0;
static u32 s_mesh_idx = 0;

bool test_3d_shader_op::initialize	( benchmark& benchmark, group_to_execute& group, test_to_execute& test, std::map<std::string,std::string>& parameters, std::string& out_comments)	
{ 
	m_mesh_index = 0;

	if (parameters.find("code")!=parameters.end())
	{
		std::string shader_code = parameters["code"];

		//char const* vs_code = 
		m_vs_code = 
			"#include \"graphics_benchmark_common.h\"			\n"
			"v2p_GB_Test main ( v_GB_Test i)					\n"
			"{													\n"
			"	v2p_GB_Test result;								\n"
			"	result.HPos = mul(m_WVP, float4(i.P.xyz,1));	\n"
			"	result.Color = i.Color;							\n"
			"	result.Color0 = i.Color0;						\n"
			"	result.Color1 = i.Color1;						\n"
			"	result.Color2 = i.Color2;						\n"
			"	result.Color3 = i.Color3;						\n"
			"	result.Color4 = i.Color4;						\n"
			"	result.Color5 = i.Color5;						\n"
			"	result.Color6 = i.Color6;						\n"
			"	result.Tex0 = i.Tex0;							\n"
			"	result.Normal = i.Normal;						\n"
			"	return result;									\n"
			"}													\n";
		
		// Initialize shader.
		test_simple_shader_op::initialize(benchmark,group,test,parameters,out_comments);
	}

	u32 wnd_size_x, wnd_size_y;
	
	benchmark.get_window_resolution(wnd_size_x, wnd_size_y);
	
	m_sphere_matrix = xray::math::mul4x3(
		create_camera_at(float3(2.0f,2.0f,2),float3(0.0f,0.0f,0.0f),float3(0.0f,1.0f,0.0f)),
		create_perspective_projection(math::pi_d4,(float)wnd_size_y/(float)wnd_size_x,0.01f,100.0f)
	);
	
	return true;
}

void test_3d_shader_op::execute	(benchmark& benchmark, u32, u32, test_to_execute& test, gpu_timer& gtimer)
{
	test_simple_shader_op::execute(benchmark,0,1,test,gtimer);
	benchmark.renderer().render_3d_sphere( gtimer, m_sphere_matrix );
}


} // namespace graphics_benchmark
} // namespace xray