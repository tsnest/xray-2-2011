#include "pch.h"
#include <xray/resources_cook_classes.h>

namespace xray {
namespace resources {

managed_cook::managed_cook	(class_id_enum resource_class, reuse_enum reuse_type, u32 creation_thread_id, enum_flags<flags_enum> flags) 
:	cook_base(resource_class, reuse_type,	flags | 
											(flags_enum)internal_flag_creates_managed_resource | 
											flag_destroy_in_any_thread, creation_thread_id, thread_id_unset)
{
}

inplace_managed_cook::inplace_managed_cook	(class_id_enum resource_class, reuse_enum reuse_type, u32 creation_thread_id, enum_flags<flags_enum> flags)
:	cook_base(resource_class, reuse_type,	flags |
											(flags_enum)internal_flag_creates_managed_resource | 
											(flags_enum)internal_flag_does_inplace_cook | 
											flag_destroy_in_any_thread, creation_thread_id, thread_id_unset)
{
	R_ASSERT								(reuse_type != reuse_raw);
}

unmanaged_cook::unmanaged_cook (class_id_enum resource_class, reuse_enum reuse_type, u32 creation_thread_id, u32 allocate_thread_id, enum_flags<flags_enum> flags)
:	cook_base(resource_class, reuse_type, flags, creation_thread_id, allocate_thread_id)
{
}

inplace_unmanaged_cook::inplace_unmanaged_cook (class_id_enum resource_class, reuse_enum reuse_type, 
												u32 creation_thread_id, u32 allocate_thread_id, enum_flags<flags_enum> flags)
:	cook_base(resource_class, reuse_type, flags | (flags_enum)internal_flag_does_inplace_cook, creation_thread_id, allocate_thread_id)
{
	R_ASSERT								(reuse_type != reuse_raw);
}

translate_query_cook::translate_query_cook (class_id_enum resource_class, reuse_enum reuse_type, u32 translate_query_thread, enum_flags<flags_enum> flags)
:	cook_base(resource_class, reuse_type, flags | (flags_enum)internal_flag_does_translate_query, thread_id_unset, translate_query_thread)
{
}

} // namespace resources
} // namespace xray
