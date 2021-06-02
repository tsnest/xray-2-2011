////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MODEL_EDITOR_MEMORY_H_INCLUDED
#define MODEL_EDITOR_MEMORY_H_INCLUDED

#include <xray/buffer_vector.h>
#include <xray/fixed_vector.h>
#include <xray/associative_vector.h>
#include <xray/hash_multiset.h>


namespace xray {
namespace model_editor {

extern memory::doug_lea_allocator_type*				g_allocator;

#define USER_ALLOCATOR				*::xray::model_editor::g_allocator
#include <xray/std_containers.h>
#undef USER_ALLOCATOR

} // namespace model_editor
} // namespace xray

#define NEW( type )                              XRAY_NEW_IMPL(*::xray::model_editor::g_allocator, type )
#define DELETE( pointer )                        XRAY_DELETE_IMPL(*::xray::model_editor::g_allocator, pointer )
#define MALLOC( size, description )              XRAY_MALLOC_IMPL(*::xray::model_editor::g_allocator, size, description )
#define REALLOC( pointer, size, description )    XRAY_REALLOC_IMPL(*::xray::model_editor::g_allocator, pointer, size, description )
#define FREE( pointer )                          XRAY_FREE_IMPL(*::xray::model_editor::g_allocator, pointer )
#define ALLOC( type, count )                     XRAY_ALLOC_IMPL(*::xray::model_editor::g_allocator, type, count )

#endif // #ifndef MODEL_EDITOR_MEMORY_H_INCLUDED