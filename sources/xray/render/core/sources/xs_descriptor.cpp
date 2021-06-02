////////////////////////////////////////////////////////////////////////////
//	Created		: 13.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/xs_descriptor.h>

namespace xray {
namespace render {

template <typename shader_data>
xs_descriptor <shader_data>::xs_descriptor()
{

}

template <typename shader_data>
void xs_descriptor <shader_data>::reset( res_xs_hw<shader_data> * xs_hw )
{
	m_hardware_shader		= 	xs_hw;

	if( m_hardware_shader)
		m_shader_data	=	xs_hw->data();
}


template<typename shader_data>
bool xs_descriptor<shader_data>::set_sampler	( char const * name, res_sampler_state * state)
{
	u32 const size = m_shader_data.samplers.size();
	for( u32 i = 0; i< size; ++i)
	{
		if( m_shader_data.samplers[i].name != name)
			continue;

		m_shader_data.samplers[i].state = state;
		return true;
	}

	return false;
}

template<typename shader_data>
bool xs_descriptor<shader_data>::set_texture	( char const * name, res_texture * texture)
{
	u32 const size = m_shader_data.textures.size();
	for( u32 i = 0; i< size; ++i)
	{
		if( m_shader_data.textures[i].name != name)
			continue;

		m_shader_data.textures[i].texture = texture;
		return true;
	}

	return false;
}
template<typename shader_data>
bool xs_descriptor<shader_data>::use_texture	( char const * name)
{
	u32 const size = m_shader_data.textures.size();
	for( u32 i = 0; i< size; ++i)
		if( m_shader_data.textures[i].name == name)
			return true;

	return false;
}

template<typename shader_data>
bool xs_descriptor<shader_data>::use_sampler	( char const * name)
{
	u32 const size = m_shader_data.samplers.size();
	for( u32 i = 0; i< size; ++i)
		if( m_shader_data.samplers[i].name == name)
			return true;

	return false;
}

// Specialization definitions
template class xs_descriptor<vs_data>; 
template class xs_descriptor<gs_data>; 
template class xs_descriptor<ps_data>; 


} // namespace render
} // namespace xray
