////////////////////////////////////////////////////////////////////////////
//	Created		: 28.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_NAME_REGISTRY_ITEM_H_INCLUDED
#define XRAY_RESOURCES_NAME_REGISTRY_ITEM_H_INCLUDED

#include <xray/hash_multiset.h>

namespace xray {
namespace resources {

class resource_base;

class XRAY_CORE_API name_registry_entry
{
public:
	name_registry_entry			() : class_id(unknown_data_class), associated(NULL), name(NULL), next_to_delete(NULL) {}
	class_id_enum				class_id;
	resource_base *				associated;
	pcstr						name;
	name_registry_entry *		next_in_hashset;


	name_registry_entry *		next_to_delete; // used by resource_manager queue for deletion
};

namespace detail {
struct name_registry_hash	{ u32 operator () (name_registry_entry & entry) const; };
struct name_registry_equal	{ bool operator () (name_registry_entry const & left, name_registry_entry const & right) const; };
} // namespace detail

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_NAME_REGISTRY_ITEM_H_INCLUDED