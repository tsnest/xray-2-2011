////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_ANIMATION_CONTAINERS_H_INCLUDED
#define XRAY_ANIMATION_ANIMATION_CONTAINERS_H_INCLUDED

#include <xray/animation/api.h>

namespace xray {
namespace animation {

extern allocator_type*							g_allocator;

#define USER_ALLOCATOR							*::xray::animation::g_allocator
#include <xray/std_containers.h>
#undef USER_ALLOCATOR

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_ANIMATION_CONTAINERS_H_INCLUDED