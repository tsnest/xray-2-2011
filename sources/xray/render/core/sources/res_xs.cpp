////////////////////////////////////////////////////////////////////////////
//	Created		: 09.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/res_xs.h>
#include <xray/render/core/backend.h>
#include <xray/render/core/xs_descriptor.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/res_sampler_list.h>

namespace xray {
namespace render {

template <typename shader_data>
res_xs <shader_data>::res_xs( xs_descriptor<shader_data> const & binder) :
	m_is_registered	( false )
{
	m_hardware_shader = binder.hardware_shader();
	m_constants = resource_manager::ref().create_const_table	( binder.data().constants);
	m_textures	= resource_manager::ref().create_texture_list	( binder.data().textures);
	m_samplers	= resource_manager::ref().create_sampler_list	( binder.data().samplers);
}

template<typename shader_data>
bool res_xs<shader_data>::equal	( xs_descriptor<shader_data> const & binder) const
{
	return (	m_hardware_shader == binder.hardware_shader()
		&& m_constants->equal( binder.data().constants)
		&& m_textures->equal( binder.data().textures)
		&& m_samplers->equal( binder.data().samplers));
}

template<typename shader_data>
void res_xs<shader_data>::destroy_impl	() const
{
	resource_manager::ref().release( this );
}

//////////////////////////////////////////////////////////////////////////
template<>
void res_xs<ps_data>::apply() const
{
	backend::ref().set_ps			( &*m_hardware_shader);
	backend::ref().set_ps_constants ( &*m_constants);
	backend::ref().set_ps_textures	( &*m_textures);
	backend::ref().set_ps_samplers	( &*m_samplers);
}

void res_xs<gs_data>::apply() const
{
#pragma comment (XRAY_TODO("IronNick to IronNick: check shaders not here"))

	if (m_hardware_shader)
	{
		backend::ref().set_gs			( &*m_hardware_shader);
		backend::ref().set_gs_constants ( &*m_constants);
		backend::ref().set_gs_textures	( &*m_textures);
		backend::ref().set_gs_samplers	( &*m_samplers);
	}
	else
	{
		backend::ref().set_gs			( NULL );
		backend::ref().set_gs_constants ( NULL );
		backend::ref().set_gs_textures	( NULL );
		backend::ref().set_gs_samplers	( NULL );
	}
}

void res_xs<vs_data>::apply() const
{
	// setting shader
	// maybe setting input layout
	// setting constants
	// setting textures
	// setting sampler states

	backend::ref().set_vs			( &*m_hardware_shader);
	backend::ref().set_vs_constants ( &*m_constants);
	backend::ref().set_vs_textures	( &*m_textures);
	backend::ref().set_vs_samplers	( &*m_samplers);
}

// Specialization definitions
template class res_xs<vs_data>; 
template class res_xs<gs_data>; 
template class res_xs<ps_data>; 

} // namespace render
} // namespace xray
