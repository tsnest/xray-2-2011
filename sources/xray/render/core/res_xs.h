////////////////////////////////////////////////////////////////////////////
//	Created		: 09.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_XS_H_INCLUDED
#define RES_XS_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>

namespace xray {
namespace render {

class res_texture;

template<typename shader_data>
class res_xs_hw;

template <typename shader_data>
class xs_descriptor;

class shader_constant_table;
typedef intrusive_ptr<
	shader_constant_table,
	resource_intrusive_base,
	threading::single_threading_policy
> shader_constant_table_ptr;

class res_texture_list;
typedef intrusive_ptr<res_texture_list, resource_intrusive_base, threading::single_threading_policy> ref_texture_list;

class res_sampler_list;
typedef intrusive_ptr<res_sampler_list, resource_intrusive_base, threading::single_threading_policy> ref_sampler_list;

template < typename shader_data >
class res_xs : public resource_intrusive_base {
	typedef xray::intrusive_ptr<res_xs_hw<shader_data>, resource_intrusive_base, threading::single_threading_policy>	ref_xs_hw;

	friend class resource_intrusive_base;
	friend class resource_manager;
	friend class res_effect;
	res_xs		( xs_descriptor<shader_data> const & binder);
	void destroy_impl	() const;

public:
	u32 id	() const { return m_id; }
	inline void set_id( u32 const id ) { m_id = id; }
 
	bool equal	( xs_descriptor<shader_data> const & binder) const;
	void apply	() const;

	inline bool	is_registered	() const{ return m_is_registered; }
	inline void	mark_registered	()		{ R_ASSERT( !m_is_registered); m_is_registered = true; }


	res_xs_hw<shader_data> const *	hardware_shader	()	{ return &*m_hardware_shader; }

private:
	ref_xs_hw			m_hardware_shader;
	shader_constant_table_ptr	m_constants;
	ref_texture_list	m_textures;
	ref_sampler_list	m_samplers;
	u32					m_id;
	bool				m_is_registered;
}; // class res_xs

struct vs_data;
typedef res_xs< vs_data >		res_vs;
typedef xray::intrusive_ptr< res_vs, resource_intrusive_base, threading::single_threading_policy >	ref_vs;

struct gs_data;
typedef res_xs< gs_data >		res_gs;
typedef xray::intrusive_ptr< res_gs, resource_intrusive_base, threading::single_threading_policy >	ref_gs;

struct ps_data;
typedef res_xs< ps_data >		res_ps;
typedef xray::intrusive_ptr< res_ps, resource_intrusive_base, threading::single_threading_policy >	ref_ps;

} // namespace render
} // namespace xray

#endif // #ifndef RES_XS_H_INCLUDED