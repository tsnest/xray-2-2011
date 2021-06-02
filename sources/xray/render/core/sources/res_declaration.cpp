////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/res_declaration.h>
#include <xray/render/core/resource_manager.h>

namespace xray {
namespace render {

signature_layout_pair::signature_layout_pair( res_declaration const* decl, res_signature const * signature):
signature	(signature)
{
	input_layout = resource_manager::ref().create_input_layout( decl, signature);
}

res_declaration::res_declaration( D3D_INPUT_ELEMENT_DESC const* decl, u32 count):
dcl_code( decl, decl+count),
m_is_registered( false )
{
// 	CHECK_RESULT(device::ref().device()->CreateVertexDeclaration(decl, &m_dcl));
// 	u32 dcl_size = D3DXGetDeclLength(decl)+1;
//	m_dcl_code.assign();
}

void res_declaration::destroy_impl() const
{
	resource_manager::ref().release( this );
}

res_declaration::~res_declaration()
{	
	//	Release vertex layout
//	safe_release(m_dcl);
}

} // namespace render 
} // namespace xray 
