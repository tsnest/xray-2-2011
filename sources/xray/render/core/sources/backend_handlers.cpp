////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/backend_handlers.h>
#include <xray/render/core/shader_constant_defines.h>
#include <xray/render/core/device.h>
#include <xray/render/core/utils.h>
#include <xray/render/core/shader_constant_table.h>
#include <xray/render/core/backend.h>
#include <xray/render/core/res_xs_hw.h>
#include <xray/render/core/res_sampler_list.h>

namespace xray {
namespace render {

template <enum_shader_type shader_type>
void constants_handler<shader_type>::assign( shader_constant_table const * table)
{
	if( m_current && table)
		utils::calc_lists_diff_range( m_current->m_const_buffers, table->m_const_buffers, m_diff_range_start, m_diff_range_end);
	else
	{
		m_diff_range_start = 0;
		m_diff_range_end	= (table==NULL) ? 0 : table->m_const_buffers.size();
	}

	m_current = table;

#ifdef DEBUG
	m_unset_constants.clear();
#endif //DEBUG

	if( m_current)
	{
		// Update hosts
		shader_constant_table::c_table::const_iterator it = m_current->m_table.begin();
		shader_constant_table::c_table::const_iterator end = m_current->m_table.end();

		shader_constant_table::c_buffers const & buffers = m_current->m_const_buffers;

		u32 update_marker = backend::ref().constant_update_counter();
		for( ; it != end; ++it)
		{
			shader_constant_host& host = it->get_host();
//			ASSERT( host );
			host.shader_slot(shader_type)		= it->slot();

			// Mark shader constant host as updated (currently in use) if its source is the same with the 
			// current shader constant being set. In other words if the sources are different then 
			// The shader constant is being marked old to deny shader constant manual setup form backend.
			host.m_update_markers[shader_type]	= (host.source().pointer() == it->source().pointer()) ? update_marker : update_marker-1;

// 			if( host.source().pointer() != it->source().pointer())
// 				LOG_INFO( "The global shader constant binding for \"%s\"was ignored.", host.name.c_str());

			// Gather shader constant bound data.
			u32 buff_ind = it->slot().buffer_index();
			if( buff_ind != slot_dest_buffer_null && it->source().pointer() != NULL) 
			{
				ASSERT( *((u32*)it->source().pointer()) != xray::memory::debug::freed_memory, "The bound object was deleted !");

				buffers[buff_ind]->set( *it);
			}
#ifdef DEBUG
			else if( it->source().pointer() == NULL)
			{
				m_unset_constants.push_back( &host );
			}
#endif // DEBUG
		}

		//gather_data();
	}
}

template <enum_shader_type shader_type>
void constants_handler <shader_type>::gather_data()
{
	shader_constant_table::c_table::const_iterator	it	= m_current->m_table.begin();
	shader_constant_table::c_table::const_iterator	end	= m_current->m_table.end();

	shader_constant_table::c_buffers const & buffers = m_current->m_const_buffers;
	for ( ; it!=end; ++it)
	{
		u32 buff_ind = it->slot().buffer_index();
		if( buff_ind != slot_dest_buffer_null && it->source().pointer() != NULL) 
		{
			ASSERT( *((u32*)it->source().pointer()) != xray::memory::debug::freed_memory, "The bound object was deleted !");

			buffers[buff_ind]->set( *it);
		}
	}
}

template <enum_shader_type shader_type>
void  constants_handler<shader_type>::fill_changes_buffer( ID3DConstantBuffer** buffer, u32& out_num_constants)
{
	u32 const start	= m_diff_range_start;
	u32 const end	= m_diff_range_end;
	u32 const list_size = (m_current.c_ptr() == NULL) ? 0 : m_current->m_const_buffers.size();
	
	out_num_constants = list_size;
	
	for( u32 i = start; i < end; ++i)
	{
		if( i < list_size)
			buffer[i]		= m_current->m_const_buffers[i]->hardware_buffer();
		else
			// Check if we really need to set NULL into unused slots.
			buffer[i]		= 0;
	}
}

template <enum_shader_type shader_type>
void constants_handler<shader_type>::update_buffers()
{
	if( m_current)
	{
		shader_constant_table::c_buffers::const_iterator	it = m_current->m_const_buffers.begin();
		shader_constant_table::c_buffers::const_iterator	end = m_current->m_const_buffers.end();

		for( ; it != end; ++it)
			(*it)->update();
	}
}

template <>
void constants_handler<enum_shader_type_vertex>::apply	()
{
	check_for_unset_constants();
	
	u32 const start	= m_diff_range_start;
	u32 end	= m_diff_range_end;
	
	if( end-start == 0) 
		return;
	
	ID3DConstantBuffer*	tmp_buffer[cb_buffer_max_count];
	
	xray::memory::zero(tmp_buffer);
	
	fill_changes_buffer( tmp_buffer, end);
	end	= m_diff_range_end;

	device::ref().d3d_context()->VSSetConstantBuffers( start, end-start, &tmp_buffer[start]);
	
	m_diff_range_start = m_diff_range_end = 0;
}

#ifdef DEBUG
template <enum_shader_type shader_type>
void	constants_handler<shader_type>::check_for_unset_constants()
{
	//for( u32 i = 0; i< m_unset_constants.size(); ++i)
	//	LOG_ERROR( "The shader constant \"%s\" was not specified for %s!", m_unset_constants[i]->name().c_str(), shader_type_traits<shader_type>::name());
}
#endif // DEBUG

template <>
void constants_handler<enum_shader_type_geometry>::apply	()
{
	check_for_unset_constants();
	
	u32 const start	= m_diff_range_start;
	u32 end	= m_diff_range_end;
	
	if( end-start == 0) 
		return;
	
	ID3DConstantBuffer*	tmp_buffer[cb_buffer_max_count];
	xray::memory::zero(tmp_buffer);
	
	fill_changes_buffer( tmp_buffer, end);
	end	= m_diff_range_end;

	device::ref().d3d_context()->GSSetConstantBuffers( start, end-start, &tmp_buffer[start]);
	
	m_diff_range_start = m_diff_range_end = 0;
}

template <>
void constants_handler<enum_shader_type_pixel>::apply	()
{
	check_for_unset_constants();

	u32 const start	= m_diff_range_start;
	u32 end	= m_diff_range_end;

	if( end-start == 0) 
		return;

	ID3DConstantBuffer*	tmp_buffer[cb_buffer_max_count];
	xray::memory::zero(tmp_buffer);
	fill_changes_buffer( tmp_buffer, end);
	end	= m_diff_range_end;

	device::ref().d3d_context()->PSSetConstantBuffers( start, end-start, &tmp_buffer[start]);

	m_diff_range_start = m_diff_range_end = 0;
}

template class constants_handler<enum_shader_type_vertex>;
template class constants_handler<enum_shader_type_geometry>;
template class constants_handler<enum_shader_type_pixel>;


//////////////////////////////////////////////////////////////////////////
//textures 
template <enum_shader_type shader_type>
void textures_handler<shader_type>::assign( res_texture_list const * list)
{
	u32 start, end;
	if( m_current && list)
		utils::calc_lists_diff_range( *m_current, *list, start, end);
	else
	{
		start	= 0;
		const u32 curr_list_size =	(m_current.c_ptr() == NULL) ? 0 : m_current->size();
		const u32 new_list_size =	(list == NULL) ? 0 : list->size();

		// This is done to be sure that after several assignments the total range will include all that assignment changes.
		end	= math::max( curr_list_size, new_list_size);
	}
	m_diff_range_start	= start;	
	m_diff_range_end	= math::max( m_diff_range_end, end);

	m_current = list;
}

template <enum_shader_type shader_type>
inline bool textures_handler<shader_type>::set_overwrite	( char const * name, res_texture* texture)
{
	if( m_shader == NULL)
	{
		ASSERT( 0, "The texture can be set after the shader.");
		return false;
	}

	texture_slots const & texture_slots = m_shader->data().textures;

	u32 const size = texture_slots.size();
	for( u32 i = 0; i < size; ++i)
	{
		if( texture_slots[i].name != name)
			continue;

		if( m_current)
		{
			if( (*m_current)[i] == texture)
				return false; // if nothing is changed then just return.
			else
			{
				// if overwriting with a new texture then copy the list into a custom one and
				// set the custom as a current. Overwrites into the custom list.
				if(  m_current != &m_custom_list)
					m_custom_list = *m_current;

				m_custom_list.resize( math::max( m_custom_list.size(), i+1), NULL);
			}
		}
		else
			m_custom_list.resize(i+1, NULL);
		
		m_diff_range_start	= math::min(i, m_diff_range_start);
		m_diff_range_end	= math::max(i+1, m_diff_range_end);
		
		m_current = &m_custom_list;
		m_custom_list[i] = texture;
		return true;
	}

//	LOG_WARNING ( "The texture with the specified name isn't used by the set %s.", shader_type_traits<shader_type>::name());
	return false;
}

template <enum_shader_type shader_type>
void  textures_handler<shader_type>::fill_changes_buffer( ID3DShaderResourceView** buffer, u32& out_num_textures)
{
	const u32 start		= m_diff_range_start;
	const u32 end		= m_diff_range_end;
	const u32 list_size = (m_current.c_ptr() == NULL) ? 0 : m_current->size();
	out_num_textures	= list_size;
	
	for( u32 i = start; i < end; ++i )
	{
		if( i < list_size && (*m_current)[i])
			buffer[i]		= (*m_current)[i]->view ();
		else
			buffer[i]		= 0;
	}
}

#ifdef DEBUG
template <enum_shader_type shader_type>
void textures_handler<shader_type>::check_for_unset_textures( )
{
	if( m_shader == NULL)
		return;

	texture_slots const & slots = m_shader->data().textures;
// 	for( u32 i = 0; i < slots.size(); ++i)
// 		if( slots[i].name.length())
// 		{
// 			LOG_INFO(slots[i].name.c_str());
// 		}
	for( u32 i = 0; i < slots.size(); ++i)
		if( slots[i].name != "" && ( (*m_current)[i].c_ptr() == NULL || (*m_current)[i]->view() == NULL))		
		{
			//  Temporary
			//pcstr n = shader_type_traits<shader_type>::name();
//			LOG_ERROR( "Texture \"%s\" was not specified for %s. ", slots[i].name.c_str(), n);
		}
}
#endif // DEBUG

template <>
void textures_handler<enum_shader_type_vertex>::apply	()
{
	xray::memory::zero(m_tmp_buffer);
	
	check_for_unset_textures();
	
	u32 end;
	//u32 end		= 0;
	fill_changes_buffer( m_tmp_buffer, end);
	u32 start	= m_diff_range_start;
	end		= m_diff_range_end;
	
	if( end-start > 0) 
		device::ref().d3d_context()->VSSetShaderResources( start, end-start, &m_tmp_buffer[start]);
	
	m_diff_range_start = m_diff_range_end = 0;
}

template <>
void textures_handler<enum_shader_type_geometry>::apply	()
{
	xray::memory::zero(m_tmp_buffer);
	
	check_for_unset_textures();

	u32 end;
	//u32 end		= 0;
	fill_changes_buffer( m_tmp_buffer, end);
	u32 start	= m_diff_range_start;
	end		= m_diff_range_end;
	
	if( end-start > 0) 
		device::ref().d3d_context()->GSSetShaderResources( start, end-start, &m_tmp_buffer[start]);
	
	m_diff_range_start = m_diff_range_end = 0;
}

template <>
void textures_handler<enum_shader_type_pixel>::apply	()
{
	xray::memory::zero(m_tmp_buffer);
	
	check_for_unset_textures();
	
	u32 end;
	//u32 end		= 0;
	fill_changes_buffer( m_tmp_buffer, end);
	u32 start	= m_diff_range_start;
	end		= m_diff_range_end;

	if( end-start > 0) 
		device::ref().d3d_context()->PSSetShaderResources( start, end-start, &m_tmp_buffer[start]);

	m_diff_range_start = m_diff_range_end = 0;
}

template class textures_handler<enum_shader_type_vertex>;
template class textures_handler<enum_shader_type_geometry>;
template class textures_handler<enum_shader_type_pixel>;


//////////////////////////////////////////////////////////////////////////
/// samplers
template <enum_shader_type shader_type>
void samplers_handler<shader_type>::assign( res_sampler_list const * list)
{
	if( m_current && list)
		utils::calc_lists_diff_range( *m_current, *list, m_diff_range_start, m_diff_range_end);
	else
	{
		m_diff_range_start	= 0;
		m_diff_range_end	= (list == NULL) ? 0 : list->size();
	}

	m_current = list;
}

template <enum_shader_type shader_type>
void  samplers_handler<shader_type>::fill_changes_buffer( ID3DSamplerState ** buffer, u32& out_num_samplers)
{
	const u32 start		= m_diff_range_start;
	const u32 end		= m_diff_range_end;
	const u32 list_size = (m_current.c_ptr() == NULL) ? 0 : m_current->size();
	out_num_samplers	= list_size;
	
	for( u32 i = start; i < end; ++i)
	{
		if( i < list_size)
			buffer[i]		= (*m_current)[i];
		else
			// Check if we really need to set NULL into unused slots.
			buffer[i]		= 0;
	}
}

#ifdef DEBUG
template <enum_shader_type shader_type>
void samplers_handler<shader_type>::check_for_unset_samplers()
{
//	if( m_shader == NULL)
//		return;
//	
//	sampler_slots const & slots = m_shader->data().samplers;
//	for( u32 i = 0; i < slots.size(); ++i)
//		if( slots[i].name != "" && !(*m_current)[i] )		
//			LOG_ERROR( "Sampler \"%s\" was not for %s.", slots[i].name, shader_type_traits<shader_type>::name());
}
#endif // #ifdef DEBUG

template <>
void samplers_handler<enum_shader_type_vertex>::apply	()
{
	xray::memory::zero(m_tmp_buffer);
	
	check_for_unset_samplers();
	u32 end		= 0;
	fill_changes_buffer( m_tmp_buffer, end);
	u32 start	= m_diff_range_start;
	//u32 end		= m_diff_range_end;
	
	if( end-start > 0) 
		device::ref().d3d_context()->VSSetSamplers( start, end-start, &m_tmp_buffer[start]);
	
	m_diff_range_start = m_diff_range_end = 0;
}

template <>
void samplers_handler<enum_shader_type_geometry>::apply	()
{
	xray::memory::zero(m_tmp_buffer);
	
	check_for_unset_samplers();
	
	u32 end		= 0;
	fill_changes_buffer( m_tmp_buffer, end);
	u32 start	= m_diff_range_start;
	//u32 end		= m_diff_range_end;
	
	if( end-start > 0) 
		device::ref().d3d_context()->GSSetSamplers( start, end-start, &m_tmp_buffer[start]);
	
	m_diff_range_start = m_diff_range_end = 0;
}

template <>
void samplers_handler<enum_shader_type_pixel>::apply	()
{
	xray::memory::zero(m_tmp_buffer);
	
	check_for_unset_samplers();
	
	u32 end		= 0;
	fill_changes_buffer( m_tmp_buffer, end);
	u32 start	= m_diff_range_start;
	//u32 end		= m_diff_range_end;
	
	if( end-start > 0) 
		device::ref().d3d_context()->PSSetSamplers( start, end-start, &m_tmp_buffer[start]);
	
	m_diff_range_start = m_diff_range_end = 0;
}

template class samplers_handler<enum_shader_type_vertex>;
template class samplers_handler<enum_shader_type_geometry>;
template class samplers_handler<enum_shader_type_pixel>;


} // namespace render 
} // namespace xray 

