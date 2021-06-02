////////////////////////////////////////////////////////////////////////////
//	Created		: 28.01.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_MATERIAL_BASE_H_INCLUDED
#define EFFECT_MATERIAL_BASE_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

struct shader_include_getter;
struct shader_configuration;

class effect_material_base: public effect_descriptor
{
public:
	void compile_begin		(pcstr vertex_shader_name, 
							 pcstr pixel_shader_name, 
							 effect_compiler& compiler, 
							 shader_configuration* shader_config, 
							 custom_config_value const& config);
	
	void compile_begin		(pcstr vertex_shader_name, 
							 pcstr geometry_shader_name, 
							 pcstr pixel_shader_name, 
							 effect_compiler& compiler, 
							 shader_configuration* shader_config, 
							 custom_config_value const& config);
	
	void compile_end		(effect_compiler& compiler);
	
}; // class effect_material_base

} // namespace render 
} // namespace xray 

#endif // #ifndef EFFECT_MATERIAL_BASE_H_INCLUDED