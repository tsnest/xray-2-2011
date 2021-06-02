////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEST_SIMPLE_SHADER_OP_H_INCLUDED
#define TEST_SIMPLE_SHADER_OP_H_INCLUDED

#include "benchmark_test.h"
#include "xray/math_randoms_generator.h"

namespace xray {
namespace graphics_benchmark {




struct texture_to_shader
{
	std::string							name;
	//xray::render::constant_host*	host;
	xray::render::ref_texture		texture;
};


class test_simple_shader_op: public benchmark_test
{
public:
	virtual bool initialize	( benchmark&, group_to_execute&, test_to_execute&, std::map<std::string,std::string>& parameters, std::string&);
	virtual void execute	( benchmark&, u32, u32, test_to_execute&, gpu_timer&);
	virtual void finalize   ( benchmark&);

protected:
	void	add_texture			( char const* name, std::string& out_textures_delc, u32 size_x, u32 size_y, D3D_SUBRESOURCE_DATA* data, DXGI_FORMAT format);
	void	add_random_texture	( char const* name, std::string& out_textures_delc, u32 size_x, u32 size_y);
	void	add_random_texture_uint8	( char const* name, std::string& out_textures_delc, u32 size_x, u32 size_y);
	void	add_black_texture	( char const* name, std::string& out_textures_delc, u32 size_x, u32 size_y);
	void	add_lookup_texture	( char const* name, std::string& out_textures_delc, u32 size_x, u32 size_y);
	
	xray::render::res_texture_list		m_texture_list;
	std::vector<texture_to_shader>			m_textures;

	std::string								m_vs_code;
	std::string								m_ps_code;
	
	xray::render::ref_effect			m_sh;

	xray::math::random32					m_random;


}; // class test_simple_shader_op

} // namespace graphics_benchmark
} // namespace xray

#endif // #ifndef TEST_SIMPLE_SHADER_OP_H_INCLUDED