#ifndef UI_MEMORY_H_INCLUDED
#define UI_MEMORY_H_INCLUDED

#include <xray/ui/api.h>

namespace xray {
namespace ui {

extern allocator_type*						g_allocator;

#define USER_ALLOCATOR						*::xray::ui::g_allocator
#include <xray/std_containers.h>
#include <xray/unique_ptr.h>
#undef USER_ALLOCATOR

} // namespace ui
} // namespace xray

#define NEW(type)							XRAY_NEW_IMPL		(	*::xray::ui::g_allocator, type)
#define DELETE(pointer)						XRAY_DELETE_IMPL	(	*::xray::ui::g_allocator, pointer)
#define MALLOC(size, description)			XRAY_MALLOC_IMPL	(	*::xray::ui::g_allocator, size, description)
#define REALLOC(pointer, size, description)	XRAY_REALLOC_IMPL	(	*::xray::ui::g_allocator, pointer, size, description)
#define FREE(pointer)						XRAY_FREE_IMPL		(	*::xray::ui::g_allocator, pointer)
#define ALLOC(type, count)					XRAY_ALLOC_IMPL		(	*::xray::ui::g_allocator, type, count)

#endif // #ifndef UI_MEMORY_H_INCLUDED