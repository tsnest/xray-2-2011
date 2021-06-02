////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/res_signature.h>
#include "com_utils.h"
#include <xray/render/core/resource_manager.h>

namespace xray {
namespace render {

res_signature::~res_signature	()
{
	safe_release(m_signature);
}

void res_signature::destroy_impl() const
{
	resource_manager::ref().release( this );
}

} // namespace render
} // namespace xray
