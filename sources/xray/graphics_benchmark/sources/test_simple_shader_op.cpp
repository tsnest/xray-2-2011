////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "test_simple_shader_op.h"
#include "gpu_timer.h"
#include "benchmark.h"
#include "benchmark_helpers.h"
#include "xray/render/core/effect_descriptor.h"
#include "time.h"


namespace xray {
namespace graphics_benchmark {

using namespace xray::render;
using namespace xray::math;

static std::string current_test_name;

static xray::render::constant_host* g_gvVars[4] = {NULL,NULL,NULL,NULL,};
static xray::render::constant_host* g_gsVars[4] = {NULL,NULL,NULL,NULL,};
static xray::render::constant_host* g_view_dir  = NULL;

class effect_simple_shader: public effect_descriptor
{
public:
	effect_simple_shader () : effect_descriptor() {}
	~effect_simple_shader() {}

	enum techniques{ color_write_enable_stencil, color_write_disable_stencil, color_write_enable_nostencil, color_write_disable_nostencil };
	virtual	void compile(effect_compiler& c, custom_config_value const& config)
	{
		XRAY_UNREFERENCED_PARAMETER	( config );

		std::string name = current_test_name + "simple_shader";
		c.begin_technique(/*color_write_enable_stencil*/)
			.begin_pass	 ( name.c_str(), name.c_str())
				.set_alpha_blend	( false)
				.set_stencil		( true, 0x80, 0xff, 0xff, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_KEEP)
				.color_write_enable(D3D_COLOR_WRITE_ENABLE_ALL)
				.set_depth(true,true)
			.end_pass	 ()
		.end_technique   ();
		
		c.begin_technique(/*color_write_disable_stencil */)
			.begin_pass	 ( name.c_str(), name.c_str())
				.set_alpha_blend	( false)
				.set_stencil		( true, 0x80, 0xff, 0xff, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_KEEP)
				.color_write_enable(D3D_COLOR_WRITE_ENABLE_NONE)
				.set_depth(true,true)
			.end_pass	 ()
		.end_technique   ();

		c.begin_technique(/*color_write_enable_nostencil*/)
			.begin_pass	 ( name.c_str(), name.c_str())
				.set_alpha_blend	( false)
				.set_stencil		( false)
				.color_write_enable(D3D_COLOR_WRITE_ENABLE_ALL)
				.set_depth(true,true)
			.end_pass	 ()
		.end_technique   ();

		c.begin_technique(/*color_write_disable_nostencil */)
			.begin_pass	 ( name.c_str(), name.c_str())
				.set_alpha_blend	( false)
				.set_stencil		( false)
				.color_write_enable(D3D_COLOR_WRITE_ENABLE_NONE)
				.set_depth(true,true)
			.end_pass	 ()
		.end_technique   ();
	}
private:
};

bool test_simple_shader_op::initialize	( benchmark& benchmark, group_to_execute& group, test_to_execute& test, std::map<std::string,std::string>& parameters, std::string& out_comments)
{
	char const* test_name = test.test_name.c_str();
	m_random.seed(1000);

	std::string shader_template_file;
	std::string shader_template_data;

	if (parameters.find("shader_template_file")!=parameters.end())
	{
		shader_template_file = parameters["shader_template_file"];
		bool valid_file = false;
		shader_template_data = file_to_string(shader_template_file.c_str(), valid_file);
		if (!valid_file)
		{
			printf("\test_simple_shader_op::initialize: failed to load template file '%s' in test '%s.%s'",shader_template_file.c_str(),group.group_name.c_str(),test_name);
			return false;
		}
	}
	else
	{
		printf("\ntest_simple_shader_op::initialize: field 'shader_template_file' not found in test '%s.%s'!\n",group.group_name.c_str(),test_name);
		return false;
	}


	std::string shader_code;

	if (parameters.find("code")!=parameters.end())
	{
		shader_code		= parameters["code"];
		out_comments	 = shader_code;
	}

	std::string textures_code;


	u32 const max_textures_count = 16;

	for (u32 i=0; i<max_textures_count; i++)
	{
		char buffer[32];
		::sprintf(buffer,"texture%d",i);
		if (parameters.find(buffer)!=parameters.end())
		{
			char buffer2[64];
			::sprintf(buffer2,"Texture2D %s;",buffer);

			texture_to_shader t2s;
			t2s.name	= buffer;
			t2s.texture	= NULL;

			m_textures.push_back( t2s );

			textures_code += buffer2;
			textures_code += "\n";
		}
	}

	u32 const num_other_texture = 11;
	char const* other_textures[num_other_texture] = {
		"random_texture","random_textureA","random_textureB",
		"random_textureC","random_textureD","lookup_texture","texture_argb8",
		"texture_argb16","black_texture","texture_uint8","texture_uint8_0",
	};
	for (u32 i=0; i<num_other_texture; i++)
	{
		textures_code+= "Texture2D ";
		textures_code+=other_textures[i];
		textures_code+=";\n";
	}

	if (parameters.find("textures_code")!=parameters.end())
		textures_code += parameters["textures_code"];

	// Textures declaration, outs, code.
	char new_ps_code[4096*10];
	::sprintf( new_ps_code, shader_template_data.c_str(), textures_code.c_str(), test.get_ps_outs_code().c_str(), shader_code.c_str() );
	m_ps_code = new_ps_code;

	char buffer[128];
	sprintf(buffer,"shader_%d_",benchmark.get_current_test_index());
	current_test_name = buffer;

	xray::render::resource_manager::ref().add_shader_code((current_test_name + "simple_shader.vs").c_str(),m_vs_code.c_str(),true);
	xray::render::resource_manager::ref().add_shader_code((current_test_name + "simple_shader.ps").c_str(),m_ps_code.c_str(),true);

	m_sh = effect_manager::ref().create_effect<effect_simple_shader>();
	
	if (!g_gvVars[0])
	{
		char const* gv_var_names[] = {"gvX", "gvY", "gvZ", "gvW", "gsX", "gsY", "gsZ", "gsW"};
		for (u32 i=0; i<4; i++)
		{
			g_gvVars[i] = backend::ref().register_constant_host( gv_var_names[i] );
			g_gsVars[i] = backend::ref().register_constant_host( gv_var_names[i+4] );
		}
		g_view_dir = backend::ref().register_constant_host( "m_view_dir" );
	}


	xray::render::texture_slots const& tex_slots = m_sh->get_technique(0)->get_pass(0)->get_ps()->hw_shader()->data().textures;

	std::vector<std::string> used_texture_names;
	for (u32 i=0; i<tex_slots.size(); i++)
		used_texture_names.push_back(tex_slots[i].name.c_str());

	for (u32 i=0; i<used_texture_names.size(); i++)
	{
		std::string const& tex_name = used_texture_names[i];

		//char buffer[128];
		//::sprintf(buffer,"texture%d",i);

		if (tex_name.find("texture")!=-1 && tex_name.length()==8)
		{
			for (u32 j=0; j<m_textures.size(); j++)
				if (m_textures[j].name==tex_name)
				{
					std::string value = parameters[ tex_name ].c_str();

					// Create new texture 1024x1024 with setted format
					if (value.find("DXGI_FORMAT")!=-1)
					{
						std::string user_texture_name = "$user$" + value;
						m_textures[j].texture = xray::render::resource_manager::ref().create_texture2d(user_texture_name.c_str(),1024,1024,0,get_dxgi_format(value),D3D_USAGE_DEFAULT,1);
					}
					else 
						m_textures[j].texture = xray::render::resource_manager::ref().create_texture( value.c_str(), 0, false, false, false );
				}
		}
		else if (tex_name=="random_texture" || tex_name=="random_textureA" || tex_name=="random_textureB"|| tex_name=="random_textureC" || tex_name=="random_textureD")
			add_random_texture(tex_name.c_str(),textures_code,1024,1024);
		else if (tex_name=="lookup_texture")  
		{
			add_lookup_texture(tex_name.c_str(),textures_code,1024,1024);
		}
		else if (tex_name=="texture_argb8")   add_texture(tex_name.c_str(),textures_code,1024,1024,0,DXGI_FORMAT_R8G8B8A8_UNORM);
		else if (tex_name=="texture_argb16")  add_texture(tex_name.c_str(),textures_code,1024,1024,0,DXGI_FORMAT_R16G16B16A16_FLOAT);
		else if (tex_name=="texture_uint8")   add_random_texture_uint8(tex_name.c_str(),textures_code,1024,1024);
		else if (tex_name=="texture_uint8_0")   add_random_texture_uint8(tex_name.c_str(),textures_code,1024,1024);
		else if (tex_name=="black_texture")   add_black_texture(tex_name.c_str(),textures_code,1024,1024);
	}

	return true;
}

void test_simple_shader_op::finalize(benchmark&)
{
	
}

void test_simple_shader_op::execute	(benchmark&, u32, u32, test_to_execute& test, gpu_timer&)
{
	if (test.is_color_write_enable)
	{
		if (test.is_stencil_enable)
			m_sh->apply			( effect_simple_shader::color_write_enable_stencil);
		else
			m_sh->apply			( effect_simple_shader::color_write_enable_nostencil);
	}
	else
	{
		if (test.is_stencil_enable)
			m_sh->apply			( effect_simple_shader::color_write_disable_stencil);
		else
			m_sh->apply			( effect_simple_shader::color_write_disable_nostencil);
	}
	
	
	float4 v[] = {
				float4((float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f),
				float4((float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f),
				float4((float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f),
				float4((float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f,(float)m_random.random(128)/128.0f), };
	
	for ( u32 i=0; i<4; i++)
	{
		backend::ref().set_ps_constant( g_gvVars[i], v[i]);
		backend::ref().set_ps_constant( g_gsVars[i], v[i].x);
		//backend::ref().set_vs_constant( g_gvVars[i], v[i]);
		//backend::ref().set_vs_constant( g_gsVars[i], v[i].x);
	}
	
	float4 view_pos = float4(2.0f,2.0f,2.0f,1.0f);
	float4 view_dir = -view_pos.normalize();
	backend::ref().set_ps_constant( g_view_dir, view_dir);
	//backend::ref().set_vs_constant( g_view_dir, view_dir);
	
	
	for (u32 i=0; i<m_textures.size(); i++)
		if (m_textures[i].texture)
			backend::ref().set_ps_texture(m_textures[i].name.c_str(),&*m_textures[i].texture);
}

void test_simple_shader_op::add_texture( char const* name, std::string&, u32 size_x, u32 size_y, D3D_SUBRESOURCE_DATA* data, DXGI_FORMAT format)
{
	std::string user_texture_name = "$user$";
	user_texture_name += name;

	texture_to_shader t2s;
	t2s.name	= name;
	t2s.texture = xray::render::resource_manager::ref().create_texture2d(user_texture_name.c_str(),size_x,size_y,data,format,D3D_USAGE_DEFAULT,1);

	m_textures.push_back( t2s );
}

void test_simple_shader_op::add_random_texture	( char const* name, std::string& out_textures_delc, u32 size_x, u32 size_y)
{
	u32* arr = NEW_ARRAY(u32,size_x*size_y);
	xray::math::random32 rnd(1000);

	u32* arr_tmp = arr;
	for (u32 i=0; i<size_x*size_y; i++)
	{
		u32	  uX = rnd.random(255),
			uY = rnd.random(255),
			uZ = rnd.random(255),
			uW = rnd.random(255);

		*arr_tmp++ = color(uX,uY,uZ,uW).get_d3dcolor();
	}

	D3D_SUBRESOURCE_DATA data;
	data.pSysMem			= arr;
	data.SysMemPitch		= size_x*sizeof(u32);
	data.SysMemSlicePitch	= data.SysMemPitch*size_y;

	add_texture(name,out_textures_delc,size_x,size_y,&data,DXGI_FORMAT_R8G8B8A8_UNORM);		

	DELETE_ARRAY(arr);
}

void test_simple_shader_op::add_random_texture_uint8	( char const* name, std::string& out_textures_delc, u32 size_x, u32 size_y)
{
	u32* arr = NEW_ARRAY(u32,size_x*size_y);
	xray::math::random32 rnd(1000);

	u32* arr_tmp = arr;
	for (u32 i=0; i<size_x*size_y; i++)
	{
		u32	uX = rnd.random(255),
			uY = rnd.random(255),
			uZ = rnd.random(255),
			uW = rnd.random(255);

		*arr_tmp++ = color(uX,uY,uZ,uW).get_d3dcolor();
	}

	D3D_SUBRESOURCE_DATA data;
	data.pSysMem			= arr;
	data.SysMemPitch		= size_x*sizeof(u32);
	data.SysMemSlicePitch	= data.SysMemPitch*size_y;

	add_texture(name,out_textures_delc,size_x,size_y,&data,DXGI_FORMAT_R8G8B8A8_UINT);		

	DELETE_ARRAY(arr);
}

void test_simple_shader_op::add_black_texture	( char const* name, std::string& out_textures_delc, u32 size_x, u32 size_y)
{
	u32* arr = NEW_ARRAY(u32,size_x*size_y);
	xray::math::random32 rnd(1000);

	u32* arr_tmp = arr;
	for (u32 i=0; i<size_x*size_y; i++)
		*arr_tmp++ = 0;//color(zero,zero,zero,zero).get_d3dcolor();

	D3D_SUBRESOURCE_DATA data;
	data.pSysMem			= arr; data.SysMemPitch		= size_x*sizeof(u32); data.SysMemSlicePitch	= data.SysMemPitch*size_y;
	add_texture(name,out_textures_delc,size_x,size_y,&data,DXGI_FORMAT_R8G8B8A8_UNORM);		
	DELETE_ARRAY(arr);
}

void test_simple_shader_op::add_lookup_texture	( char const* name, std::string& out_textures_delc, u32 size_x, u32 size_y)
{
	s32* arr = NEW_ARRAY(s32,size_x*size_y);
	xray::math::random32 rnd(1000);

	s32* arr_tmp = arr;

	for (u32 i=0; i<size_x*size_y; i++)
	{
		u32	uX = rnd.random(255),
			uY = rnd.random(255),
			uZ = rnd.random(255),
			uW = rnd.random(255);

		*arr_tmp++ = color(uX,uY,uZ,uW).get_d3dcolor();
	}
	D3D_SUBRESOURCE_DATA data;
	data.pSysMem			= arr; data.SysMemPitch		= size_x*sizeof(s32); data.SysMemSlicePitch	= data.SysMemPitch*size_y;
	add_texture(name,out_textures_delc,size_x,size_y,&data,DXGI_FORMAT_R8G8B8A8_SNORM);		
	DELETE_ARRAY(arr);
}

} // namespace graphics_benchmark
} // namespace xray