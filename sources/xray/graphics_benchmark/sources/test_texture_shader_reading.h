////////////////////////////////////////////////////////////////////////////
//	Created		: 15.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEST_TEXTURE_SHADER_READING_H_INCLUDED
#define TEST_TEXTURE_SHADER_READING_H_INCLUDED

#include "benchmark_test.h"
#include "xray/math_randoms_generator.h"

namespace xray {
namespace graphics_benchmark {

////////////////////////////////////////////////////////////////////////////
class test_texture_shader_reading: public benchmark_test, public creator_base<test_texture_shader_reading>
{
public:
	virtual bool initialize	( benchmark&, char const*, char const*);
	virtual bool initialize	( benchmark&, char const*, std::map<std::string,std::string>& parameters, std::string&);
	virtual void execute	( benchmark&, u32, u32, gpu_timer&);
private:
	std::string m_ps_code;
	xray::render_dx10::ref_effect	 m_sh;

	xray::render_dx10::constant_host*			m_X;
	xray::render_dx10::constant_host*			m_Y;
	xray::render_dx10::constant_host*			m_Z;
	xray::render_dx10::constant_host*			m_W;
	xray::render_dx10::res_texture*				m_texture;
	xray::render_dx10::res_texture_list			m_texture_list;
	xray::render_dx10::constant_host*			m_texture_host;

	xray::math::random32						m_random;
}; // class test_texture_shader_reading

} // namespace graphics_benchmark
} // namespace xray

#endif // #ifndef TEST_TEXTURE_SHADER_READING_H_INCLUDED