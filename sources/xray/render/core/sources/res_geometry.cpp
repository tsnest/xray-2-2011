////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/backend.h>
#include <xray/render/core/res_declaration.h>

namespace xray {
namespace render {

res_geometry::~res_geometry()
{

}

void res_geometry::destroy_impl() const
{
	resource_manager::ref().release( this );
}

bool res_geometry::equal(const res_geometry& other) const
{
	return m_vb==other.m_vb && m_ib==other.m_ib &&
		m_dcl==other.m_dcl && m_vb_stride==other.m_vb_stride;
}

void res_geometry::apply()
{
	backend::ref().set_declaration( m_dcl.c_ptr());
	backend::ref().set_vb( m_vb.c_ptr(), m_vb_stride);
	backend::ref().set_ib( m_ib.c_ptr());
}


} // namespace render
} // namespace xray
