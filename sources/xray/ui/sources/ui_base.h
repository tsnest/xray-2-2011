////////////////////////////////////////////////////////////////////////////
//	Created		: 08.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef UI_BASE_H_INCLUDED
#define UI_BASE_H_INCLUDED

namespace xray {
namespace ui {

class ui_base :private boost::noncopyable
{
public:
	ui_base	( memory::base_allocator& a ):m_allocator(a){};

	memory::base_allocator&		allocator() const		{ return m_allocator;}
private:
	memory::base_allocator&		m_allocator;
}; // class ui_base

} // namespace ui
} // namespace xray

#endif // #ifndef UI_BASE_H_INCLUDED