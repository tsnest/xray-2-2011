////////////////////////////////////////////////////////////////////////////
//	Created		: 24.05.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_MANAGER_INLINE_H_INCLUDED
#define RESOURCE_MANAGER_INLINE_H_INCLUDED

namespace xray {
namespace render {

template< int size >
inline res_geometry* resource_manager::create_geometry( D3D_INPUT_ELEMENT_DESC const (&decl)[size], u32 vertex_stride, untyped_buffer& vb, untyped_buffer& ib)
{
	return create_geometry( (D3D_INPUT_ELEMENT_DESC*)&decl, size, vertex_stride, vb, ib);
}



} // namespace render
} // namespace xray

#endif // #ifndef RESOURCE_MANAGER_INLINE_H_INCLUDED