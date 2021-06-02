////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/res_input_layout.h>
#include <xray/render/core/res_signature.h>
#include <xray/render/core/device.h>
#include <xray/render/core/res_declaration.h>
#include "com_utils.h"
#include <xray/render/core/resource_manager.h>

namespace xray {
namespace render {

res_input_layout::res_input_layout( res_declaration const * decl, res_signature const * signature):
m_declaration	( decl ),
m_signature		( signature ),
m_is_registered	( false )
{
	m_hw_input_layout = NULL;
	ID3DBlob * hw_singiture = signature->hw_signature();
	device::ref().d3d_device()->CreateInputLayout( &m_declaration->dcl_code[0], m_declaration->dcl_code.size(), hw_singiture ->GetBufferPointer(), hw_singiture->GetBufferSize(), &m_hw_input_layout);
	ASSERT( m_hw_input_layout);
}

res_input_layout::~res_input_layout	()
{
	safe_release( m_hw_input_layout);
}

void res_input_layout::destroy_impl() const
{
	resource_manager::ref().release( this );
}


} // namespace render
} // namespace xray
