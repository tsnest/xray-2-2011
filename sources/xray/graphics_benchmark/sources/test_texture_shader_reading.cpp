////////////////////////////////////////////////////////////////////////////
//	Created		: 15.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "test_texture_shader_reading.h"
#include "benchmark.h"
#include "benchmark_renderer.h"
#include "gpu_timer.h"
#include "xray/math_randoms_generator.h"

namespace xray {
namespace graphics_benchmark {

using namespace xray::render_dx10;
using namespace xray::math;

static std::string current_test_name;

class effect_texture_read_shader: public effect
{
public:
	effect_texture_read_shader () : effect() {}
	~effect_texture_read_shader() {}

	enum techniques{ solid };

	virtual void compile( effect_compiler& c, const effect_compilation_options& options)
	{
		XRAY_UNREFERENCED_PARAMETER	( options );

		shader_defines_list	defines;
		make_defines(defines);

		std::string name = current_test_name + "simple_shader";

		c.begin_technique( /*solid*/)
			.begin_pass	 ( name.c_str(), name.c_str(), defines)
			.end_pass	 ()
			.end_technique   ();
	}

private:
};

bool test_texture_shader_reading::initialize	( benchmark&, char const*, char const*)
{
	return true;
}

static std::string file_to_string( char const* file_name )
{
	std::string result;
	FILE* file = fopen(file_name,"r");
	fseek(file,0,SEEK_END);
	size_t file_size = ftell(file);
	fseek(file,0,SEEK_SET);
	result.resize(file_size);
	fread(result.begin(),1,file_size,file);
	fclose(file);
	return result;
}

void shader_compile_error_handler( enum_shader_type shader_type, char const* name, char const* error_string)
{
	printf("\n%s\n",error_string);
}

bool test_texture_shader_reading::initialize	( benchmark& benchmark, char const* test_name, std::map<std::string,std::string>& parameters, std::string&)	
{ 
	m_X					=NULL;
	m_Y					=NULL;
	m_Z					=NULL;
	m_W					=NULL;
	m_texture			=NULL;
	m_texture_host		=NULL;

	char const* vs_code = 
		"#include \"common.h\"								\n"
		"float4 x, y, z, w;									\n"
		"v2p_TL main ( v_TL i)								\n"
		"{													\n"
		"	v2p_TL result;									\n"
		"	result.HPos = float4(i.P.xyz,1);				\n"
		"	result.Color = i.Color;							\n"
		"	result.Tex0 = i.Tex0;							\n"
		"	return result;									\n"
		"}													\n";

	std::string shader_template_file;
	std::string shader_template_data =
		"#include \"common.h\"								\n"
		"float4 x, y, z, w;									\n"
		"Texture2D testTexture;								\n"
		"SamplerState testSampler							\n"
		"{													\n"
		"	Filter   = %s;									\n"
		"	AddressU = %s;									\n"
		"	AddressV = %s;									\n"
		"};													\n"
		"float4 main( v2p_TL i) : SV_TARGET					\n"
		"{													\n"
		"	float4 result = 0;								\n"
		"	float X=sin(i.Color);							\n"
		"	float Y=cos(i.Color);							\n"
		"	float Z=sqrt(i.Color);							\n"
		"	float W=rsqrt(i.Color);							\n"
		"	%s;												\n"
		"	return result;									\n"
		"}													\n";

	if (parameters.find("shader_template_file")!=parameters.end())
	{
		shader_template_file = parameters["shader_template_file"];
		shader_template_data = file_to_string(shader_template_file.c_str());
	}
	else
	{
		printf("\ntest_texture_shader_reading::initialize: field 'shader_template_file' not found in test '%s'!\n",test_name);
		//return false;
	}

	DXGI_FORMAT texture_format = DXGI_FORMAT_R8G8B8A8_UINT;

	std::string filter_code, address_u_code, address_v_code, shader_code;
	
	filter_code = "MIN_MAG_MIP_LINEAR";
	address_u_code = "Wrap";
	address_v_code = "Wrap";

	if (parameters.find("filter")!=parameters.end())
		filter_code = parameters["filter"];

	if (parameters.find("address_u")!=parameters.end())
		address_u_code = parameters["address_u"];

	if (parameters.find("address_v")!=parameters.end())
		address_v_code = parameters["address_v"];

	if (parameters.find("code")!=parameters.end())
		shader_code = parameters["code"];

	if (parameters.find("texture_format")!=parameters.end())
	{
		std::string tex_format_code = parameters["texture_format"];
		if (tex_format_code=="rgba8")
			texture_format = DXGI_FORMAT_R8G8B8A8_UINT;
		if (tex_format_code=="rgba16")
			texture_format = DXGI_FORMAT_R16G16B16A16_UINT;
		if (tex_format_code=="rgba32")
			texture_format = DXGI_FORMAT_R32G32B32A32_UINT;
	}

	char const* ps_code = shader_template_data.c_str();

	char new_ps_code[1024];
	::sprintf(new_ps_code,ps_code,filter_code.c_str(), address_u_code.c_str(), address_v_code.c_str(), shader_code.c_str());

	char buffer[1024];
	sprintf(buffer,"shader_%d_",benchmark.get_current_test_index());
	current_test_name = buffer;

	xray::render_dx10::resource_manager::ref().add_shader_code((current_test_name + "simple_shader.vs").c_str(),vs_code,true);
	xray::render_dx10::resource_manager::ref().add_shader_code((current_test_name + "simple_shader.ps").c_str(),new_ps_code,true);
	
	effect_texture_read_shader b;
	m_sh = effect_manager::ref().create_effect( &b);

	if (!m_X)
	{
		m_X = backend::ref().register_constant_host( "x");
		m_Y = backend::ref().register_constant_host( "y");
		m_Z = backend::ref().register_constant_host( "z");
		m_W = backend::ref().register_constant_host( "w");
		
		//const u32 sizeX = 128;

		//u8 arr[sizeX*4][sizeX*4];
		//for (u32 i=0; i<sizeX*4; i++)
		//	for (u32 j=0; j<sizeX*4; j++)
		//		arr[i][j]=i+j;
		//
		//D3D_SUBRESOURCE_DATA data;
		//data.pSysMem = arr;
		//data.SysMemPitch = sizeX*4;
		
		m_texture = xray::render_dx10::resource_manager::ref().create_texture2d("$user$texture",128,128,0,texture_format,D3D_USAGE_DEFAULT,1);
		
		//DELETE_ARRAY(data.pSysMem);
		
		//m_texture = xray::render_dx10::resource_manager::ref().create_texture("detail/detail_beton_det4");
		
		m_texture_list.push_back(m_texture);
		m_texture_host = backend::ref().register_constant_host( "testTexture");
	}
	
	m_random.seed(1000);

	return true;
}

void test_texture_shader_reading::execute	( benchmark& benchmark, u32, u32, gpu_timer& gtimer)
{
	m_sh->apply			( effect_texture_read_shader::solid);
	
	float4 v = float4((float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f);
	
	backend::ref().set_constant( m_X, v);
	backend::ref().set_constant( m_Y, v);
	backend::ref().set_constant( m_Z, v);
	backend::ref().set_constant( m_W, v);
	
	backend::ref().set_ps_textures( &m_texture_list);
	
	benchmark.renderer	( ).render_fullscreen_quad( gtimer);
}

} // namespace graphics_benchmark
} // namespace xray