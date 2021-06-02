////////////////////////////////////////////////////////////////////////////
//	Created		: 14.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/res_texture_list.h>
#include <xray/render/core/resource_manager.h>

namespace xray {
namespace render {

res_texture_list::res_texture_list	( texture_slots const & slots) :
	m_is_registered	( false )
{
	u32 const size = slots.size();
	for (u32 i = 0; i < size; ++i)
	{
		if( slots[i].slot_id != enum_slot_ind_null)
		{
			m_container.resize(i+1, ref_texture(NULL));
			m_container[i] = slots[i].texture;
		}
	}
}

void res_texture_list::destroy_impl	() const
{
	resource_manager::ref().release( this );
}

bool res_texture_list::equal(const res_texture_list& base) const
{
	if( m_container.size() != base.m_container.size())
		return false;

	for (u32 cmp = 0, size = m_container.size(); cmp < size; ++cmp)
	{
		if (m_container[cmp] != base.m_container[cmp])	return false;
	}

	return true;
}

bool res_texture_list::equal( texture_slots const & base) const
{
	u32 const base_size = base.size();
	if( base_size != m_container.size())
		return false;

	for (u32 i = 0; i < base_size; ++i)
		if (m_container[i] != base[i].texture)	
			return false;

	return true;
}

} // namespace render
} // namespace xray

