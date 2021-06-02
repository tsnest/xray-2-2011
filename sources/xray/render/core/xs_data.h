////////////////////////////////////////////////////////////////////////////
//	Created		: 13.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XS_DATA_H_INCLUDED
#define XS_DATA_H_INCLUDED

#include <xray/render/core/shader_constant_table.h>
#include <xray/render/core/texture_slot.h>
#include <xray/render/core/sampler_slot.h>
#include <xray/render/core/res_signature.h>

namespace xray {
namespace render {

class res_signature;
typedef xray::intrusive_ptr<res_signature const, res_signature const, threading::single_threading_policy>	signature_const_ptr;

struct shader_data_base
{
	shader_data_base():
		instruction_count(0)
	{}
	u16 instruction_count;
};

//////////////////////////////////////////////////////////////////////////
// vs_data
struct vs_data: public shader_data_base
{
	vs_data() : hardware_shader(NULL)	{}

	typedef   ID3DVertexShader				hw_interface;
	enum { type = enum_shader_type_vertex };

	ID3DVertexShader*		hardware_shader;

	shader_constant_table	constants;
	sampler_slots			samplers;
	texture_slots			textures;

	signature_const_ptr		signature;	
};

//////////////////////////////////////////////////////////////////////////
// gs_data
struct gs_data: public shader_data_base
{
	gs_data() : hardware_shader(NULL)	{}

	typedef   ID3DGeometryShader			hw_interface;
	enum { type = enum_shader_type_geometry };

	ID3DGeometryShader*		hardware_shader;

	shader_constant_table	constants;
	sampler_slots			samplers;
	texture_slots			textures;
};


//////////////////////////////////////////////////////////////////////////
// ps_data
struct ps_data: public shader_data_base
{
	ps_data() : hardware_shader(NULL)	{}

	typedef   ID3DPixelShader				hw_interface;
	enum { type = enum_shader_type_pixel };

	ID3DPixelShader*		hardware_shader;

	shader_constant_table		constants;
	sampler_slots			samplers;
	texture_slots			textures;
};


} // namespace render
} // namespace xray

#endif // #ifndef XS_DATA_H_INCLUDED