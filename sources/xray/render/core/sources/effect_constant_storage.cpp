////////////////////////////////////////////////////////////////////////////
//	Created		: 20.09.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/effect_constant_storage.h>

namespace xray {
namespace render {


bool constant_data_predicate( data_indexer const& left, data_indexer const& right )
{
	return left.class_id < right.class_id || ( left.class_id == right.class_id && left.data_ptr < right.data_ptr);
}

effect_constant_storage::effect_constant_storage():
	m_constant_buffer(0)
{
	//m_constant_buffer = NEW(fixed_constant_data_buffer);
}

effect_constant_storage::~effect_constant_storage()
{
	clear();
}

bool effect_constant_storage::is_equal(u32 const* a_ptr, u32 const* b_ptr, u32 const num_comparision)
{
	for (u32 i=0; i<num_comparision; ++i, ++a_ptr, ++b_ptr)
		if( *a_ptr != *b_ptr)
			return false;
	return true;
}

void effect_constant_storage::clear()
{
	fixed_constants_data_buffer* next_buffer = m_constant_buffer;
	while (next_buffer)
	{
		fixed_constants_data_buffer* to_delete = next_buffer;
		next_buffer = next_buffer->next;
		DELETE(to_delete);
	}
	m_constant_buffer = 0;
}

} // namespace render
} // namespace xray
