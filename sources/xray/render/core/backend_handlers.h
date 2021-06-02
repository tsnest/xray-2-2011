////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BACKEND_LAYERS_H_INCLUDED
#define BACKEND_LAYERS_H_INCLUDED

#include <xray/render/core/shader_defines.h>
#include <xray/render/core/res_texture_list.h>
#include <xray/render/core/res_texture.h>

namespace xray {
namespace render {

class shader_constant_table;
class resource_intrusive_base;

typedef intrusive_ptr<
	shader_constant_table const,
	resource_intrusive_base const,
	threading::single_threading_policy
> shader_constant_table_const_ptr;

enum enum_shader_type;
class shader_constant_host;
class shader_constant_table;

template <typename shader_data>
class res_xs_hw;

class res_texture_list;
class res_texture;
class res_sampler_list;

typedef intrusive_ptr<res_texture_list const, resource_intrusive_base const, threading::single_threading_policy> ref_texture_list_const;
typedef intrusive_ptr<res_sampler_list const, resource_intrusive_base const, threading::single_threading_policy> ref_sampler_list_const;

//////////////////////////////////////////////////////////////////////////
// constants_handler
template <enum_shader_type shader_type>
class constants_handler
{
	constants_handler() : m_diff_range_start (0), m_diff_range_end (0) {}
	friend class backend;

// 	shader_constant const * get( LPCSTR const name)	{ return m_current->get(name);}		// slow search ???
// 	shader_constant const * get( shared_string& name)	{ return m_current->get(name);}		// fast search ???

	template <typename T>
	void set_constant( shader_constant_host const & c, T const & arg);

	template <typename T>
	void set_constant_array( shader_constant_host const & c, T const * arg, u32 array_size);

	void			assign			( shader_constant_table const * ctable);
	void			gather_data		();
	void			update_buffers	();

	void			apply			();
	
	struct  change_ragne
	{
		change_ragne(): start(0), end(0){}

		u32 start;
		u32 end;
	};

	void  fill_changes_buffer( ID3DConstantBuffer** buffer, u32& out_num_constants);
	u32							m_diff_range_start;
	u32							m_diff_range_end;
	shader_constant_table_const_ptr	m_current;

#ifdef DEBUG
	render::vector<shader_constant_host*>		m_unset_constants;
	void							check_for_unset_constants();
#else
	void							check_for_unset_constants(){}
#endif
};


//////////////////////////////////////////////////////////////////////////
// Backend texture handler
template <enum_shader_type shader_type>
class textures_handler : boost::noncopyable
{
	friend	class backend;
	typedef typename shader_type_traits<shader_type>::shader_data	shader_data;

	textures_handler( res_xs_hw<shader_data> const * const & shader ) : m_diff_range_start (0), m_diff_range_end (0), m_shader (shader) {}

	void			assign			( res_texture_list const * ctable);
	bool			set_overwrite	( char const * name, res_texture* texture);

	void			apply			();

	void			fill_changes_buffer( ID3DShaderResourceView** buffer, u32& out_num_textures);


	ref_texture_list_const		m_current;

	u32							m_diff_range_start;
	u32							m_diff_range_end;
	ID3DShaderResourceView*		m_tmp_buffer[D3D_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
	res_xs_hw<shader_data> const * const &	m_shader;
	res_texture_list			m_custom_list;

#ifdef DEBUG
	void					check_for_unset_textures( );
#else
	void					check_for_unset_textures( ){}
#endif

};

//////////////////////////////////////////////////////////////////////////
// Backend sampler handler
template <enum_shader_type shader_type>
class samplers_handler : boost::noncopyable
{
	friend class backend;
	typedef typename shader_type_traits<shader_type>::shader_data	shader_data;

	samplers_handler( res_xs_hw<shader_data> const * const & shader) : m_diff_range_start (0), m_diff_range_end (0), m_shader (shader) {}

	void			assign		( res_sampler_list const* ctable);

	void			apply		();

	void			fill_changes_buffer( ID3DSamplerState ** buffer, u32& out_num_samplers);

	u32						m_diff_range_start;
	u32						m_diff_range_end;
	ID3DSamplerState*		m_tmp_buffer[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];
	ref_sampler_list_const	m_current;

	res_xs_hw<shader_data> const * const &	m_shader;

#ifdef DEBUG
	void					check_for_unset_samplers();
#else
	void					check_for_unset_samplers(){}
#endif
};


// template <typename resource, hw_resource>
// class resource_handler
// {
// 	friend class backend;
// 
// 	void			assign		( resource& ctable);
// 	void			apply		();
// 
// 	void			fill_changes_buffer( hw_resource** buffer);
// 
// 	u32				m_diff_range_start;
// 	u32				m_diff_range_end;
// 	shader_constant_table_ptr	m_current;
// };


} // namespace render
} // namespace xray

#include "backend_handlers_inline.h"

#endif // #ifndef BACKEND_LAYERS_H_INCLUDED