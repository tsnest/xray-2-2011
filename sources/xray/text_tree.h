////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.08.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_CORE_TEXT_TREE_H_INCLUDED
#define XRAY_CORE_TEXT_TREE_H_INCLUDED

#include <xray/text_tree_item.h>

namespace xray {
namespace strings {

class XRAY_CORE_API text_tree 
{
public:
							text_tree		(pvoid buffer, u32 const buffer_size, pcstr root_name=NULL);
							~text_tree		();
	inline	text_tree_item&	root			() { return m_root; }
			void			clear			();
			void			break_page		();

private:
	text_tree_item			m_root;
	allocator_type			m_allocator;
};

} // namespace strings
} // namespace xray

#endif // #define XRAY_CORE_TEXT_TREE_H_INCLUDED