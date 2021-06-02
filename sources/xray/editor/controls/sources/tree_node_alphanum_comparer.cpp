////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tree_node_alphanum_comparer.h"
#include "tree_node.h"
#include "tree_node.h"
#include "alphanum_comparator.h"

using System::Windows::Forms::TreeNode;

namespace xray {
namespace editor {
namespace controls {

void		tree_node_alphanum_comparer::in_constructor		()
{
	m_node_comparer = gcnew AlphanumComparator();
}

Int32		tree_node_alphanum_comparer::Compare			(Object^ x, Object^y)
{
		tree_node^ node_x = safe_cast<tree_node^>(x);
		tree_node^ node_y = safe_cast<tree_node^>(y);

		if( node_x->m_node_type == node_y->m_node_type )
			return m_node_comparer->Compare(safe_cast<TreeNode^>(x)->Text, safe_cast<TreeNode^>(y)->Text);

		if( node_x->m_node_type == tree_node_type::group_item )
			return -1;

		if( node_y->m_node_type == tree_node_type::group_item )
			return 1;

		return m_node_comparer->Compare(safe_cast<TreeNode^>(x)->Text, safe_cast<TreeNode^>(y)->Text);
}

} // namespace controls
} // namespace editor
} // namespace xray