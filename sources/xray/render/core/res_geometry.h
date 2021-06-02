////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_GEOMETRY_H_INCLUDED
#define RES_GEOMETRY_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>
#include <xray/render/core/res_declaration.h>

#ifndef MASTER_GOLD
#	include <xray/render/core/untyped_buffer.h>
#endif // #ifndef MASTER_GOLD

namespace xray {
namespace render {

class res_declaration;
typedef xray::intrusive_ptr<res_declaration, resource_intrusive_base, threading::single_threading_policy>	ref_declaration;

class untyped_buffer;
typedef	intrusive_ptr<
	untyped_buffer const,
	resource_intrusive_base const,
	threading::single_threading_policy
> untyped_buffer_const_ptr;

typedef	intrusive_ptr<
	untyped_buffer,
	resource_intrusive_base,
	threading::single_threading_policy
> untyped_buffer_ptr;

class res_geometry : public resource_intrusive_base {
	friend class resource_intrusive_base;
	friend class resource_manager;

	res_geometry( untyped_buffer& vb, untyped_buffer& ib, res_declaration * dcl, u32 stride):
		m_vb(&vb), m_ib(&ib), m_dcl(dcl), m_vb_stride(stride), m_is_registered(false)
	{
		ASSERT( m_vb->type() == enum_buffer_type_vertex);
		ASSERT( m_ib->type() == enum_buffer_type_index);
	}
	
	~res_geometry();
	void destroy_impl() const;

public:
	
	bool equal(const res_geometry& other) const;
	void apply();
	inline bool	is_registered	() const{ return m_is_registered; }
	inline void	mark_registered	()		{ R_ASSERT( !m_is_registered); m_is_registered = true; }
	
	u32	get_stride() {return m_vb_stride;}

	untyped_buffer_const_ptr			vertex_buffer() { return &*m_vb;}
	untyped_buffer_const_ptr			intex_buffer() { return &*m_ib;}

private:
	untyped_buffer_ptr		m_vb;			//vb;
	untyped_buffer_ptr		m_ib;			//ib;
	ref_declaration			m_dcl;			//dcl;
	u32						m_vb_stride;	//vb_stride;
	bool					m_is_registered;
};
typedef	intrusive_ptr<res_geometry, resource_intrusive_base, threading::single_threading_policy>	ref_geometry;

} // namespace render
} // namespace xray

#endif // #ifndef RES_GEOMETRY_H_INCLUDED