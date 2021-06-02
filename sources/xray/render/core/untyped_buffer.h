////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_UNTYPED_BUFFER_H_INCLUDED
#define XRAY_RENDER_CORE_UNTYPED_BUFFER_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>

namespace xray {
namespace render {

enum enum_buffer_type {
	enum_buffer_type_vertex,
	enum_buffer_type_index,
}; // enum enum_buffer_type

class untyped_buffer : public resource_intrusive_base {
public:
						untyped_buffer	( u32 size, pcvoid data, enum_buffer_type type, bool dynamic );
						~untyped_buffer	( );
			void		destroy_impl	( ) const;

public:
	inline void*		map				( D3D_MAP mode );
	inline void			unmap			( );
	inline	ID3DBuffer*	hardware_buffer	( ) const		{ return m_hardware_buffer; }
	inline	u32			type			( ) const		{ return m_type; }
	inline	u32			size			( ) const		{ return m_size; }

private:
	ID3DBuffer*			m_hardware_buffer;
	u32					m_size;
	enum_buffer_type	m_type;
}; // class untyped_buffer

typedef	intrusive_ptr<
	untyped_buffer,
	resource_intrusive_base,
	threading::single_threading_policy
> untyped_buffer_ptr;

typedef	intrusive_ptr<
	untyped_buffer const,
	resource_intrusive_base const,
	threading::single_threading_policy
> untyped_buffer_const_ptr;

} // namespace render
} // namespace xray

#include <xray/render/core/untyped_buffer_inline.h>

#endif // #ifndef XRAY_RENDER_CORE_UNTYPED_BUFFER_H_INCLUDED