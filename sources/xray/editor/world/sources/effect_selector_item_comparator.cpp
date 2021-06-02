////////////////////////////////////////////////////////////////////////////
//	Created		: 04.08.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_selector_item_comparator.h"

using System::Windows::Forms::TreeNode;
using namespace System;

namespace xray{
namespace editor{

int		effect_selector_item_comparator::Compare		(Object^ x, Object^ y)
{
	System::String^ x_str = safe_cast<TreeNode^>(x)->Text;
	System::String^ y_str = safe_cast<TreeNode^>(y)->Text;
	if(x_str == "none") return -1;
	else if(y_str == "none") return 1;
	else return m_alphanum_comparer->Compare(x, y);
}

} // namespace editor
} // namespace xray