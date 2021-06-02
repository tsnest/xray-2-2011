////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_selector_item_comparator.h"

using namespace System;

namespace xray {
namespace editor {

Int32 stage_selector_item_comparator::Compare(Object^ x, Object^ y)
{
	Int32 order_id_x	= safe_cast<Int32>( safe_cast<controls::tree_node^>(x)->Tag );
	Int32 order_id_y	= safe_cast<Int32>( safe_cast<controls::tree_node^>(y)->Tag );
	
	return order_id_x.CompareTo(order_id_y);
}

} // namespace editor
} // namespace xray