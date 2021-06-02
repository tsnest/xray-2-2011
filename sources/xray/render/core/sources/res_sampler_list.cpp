////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/res_sampler_list.h>
#include <xray/render/core/resource_manager.h>

namespace xray {
namespace render {

res_sampler_list::res_sampler_list	( sampler_slots const & slots) :
	m_is_registered	( false )
{
	u32 const slots_size = slots.size();
	for (u32 i = 0; i < slots_size; ++i)
	{
		if( slots[i].slot_id != enum_slot_ind_null)
		{
			m_samplers.resize(i+1, ref_sampler_state (NULL));
			m_samplers[i] = slots[i].state;
		}
	}
}

void res_sampler_list::destroy_impl () const
{
	resource_manager::ref().release( this );
}

bool res_sampler_list::equal(const res_sampler_list& base) const
{
	if (m_samplers.size() != base.m_samplers.size())
		return false;

	for (u32 cmp = 0, count = m_samplers.size(); cmp < count; ++cmp)
	{
		if (m_samplers[cmp] != base.m_samplers[cmp])	return false;
	}

	return true;
}

bool res_sampler_list::equal( sampler_slots const & base) const
{
	u32 const base_size = base.size();
	for (u32 i = 0, count = m_samplers.size(); i < base_size; ++i)
	{
		if( i < count)
		{
			if (m_samplers[i] != base[i].state)	
				return false;
		}
		else if( base[i].slot_id != enum_slot_ind_null)
			return false;
	}

	return true;
}


// res_sampler_list::res_sampler_list()
// {
// //	ZeroMemory( &m_samplers, sizeof(m_samplers));
// }


} // namespace render
} // namespace xray
