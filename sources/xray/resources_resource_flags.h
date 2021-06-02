////////////////////////////////////////////////////////////////////////////
//	Created		: 09.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_RESOURCE_FLAGS_H_INCLUDED
#define XRAY_RESOURCES_RESOURCE_FLAGS_H_INCLUDED

#include <xray/vfs/association.h>

namespace xray {
namespace resources {

enum	resource_flags_enum	{	resource_flag_is_managed_resource			=	1 << 0,
								resource_flag_is_query_result				=	1 << 1,
								resource_flag_is_unmanaged_resource			=	1 << 2,
								resource_flag_is_positional_unmanaged		=	1 << 3,
								resource_flag_needs_cook					=	1 << 4,
								resource_flag_in_grm_observed_list			=	1 << 5,
								resource_flag_deassociated_from_fat_it		=	1 << 6,
								resource_flag_in_grm_increase_quality_tree	=	1 << 7,
								resource_flag_is_quality_resource			=	1 << 8,
								resource_flag_in_grm_increase_quality_queue	=	1 << 9,
								resource_flag_in_grm_release				=	1 << 10,
								resource_flag_is_dying						=	1 << 11,
							};

class	positional_unmanaged_resource;
class 	query_result;
class	unmanaged_resource;
class 	managed_resource;

typedef fixed_string<512>			log_string_type;

class XRAY_CORE_API resource_flags : public vfs::vfs_association
{
public:
									resource_flags						(resource_flags_enum flags);
	virtual							~resource_flags						() {}

	managed_resource *				cast_managed_resource				();
	unmanaged_resource *			cast_unmanaged_resource 			();
	query_result *					cast_query_result					();
	query_result const *			cast_query_result					() const;
	base_of_intrusive_base *		cast_base_of_intrusive_base			();
	base_of_intrusive_base const *	cast_base_of_intrusive_base			() const { return ((resource_flags *)this)->cast_base_of_intrusive_base(); }
	positional_unmanaged_resource *	cast_positional_unmanaged_resource	();
	positional_unmanaged_resource const *	cast_positional_unmanaged_resource	() const { return const_cast<resource_flags *>(this)->cast_positional_unmanaged_resource(); }
	resource_base *					cast_resource_base					();
	resource_base const *			cast_resource_base					() const;

	bool							is_pinned_by_grm					() const { return cast_base_of_intrusive_base()->is_captured(); }

	virtual log_string_type			log_string							() const { return ""; }

	void							set_flags							(resource_flags_enum const flags) { m_flags.set(flags); }
	void							unset_flags							(resource_flags_enum const flags) { m_flags.unset(flags); }
	bool							has_flags							(resource_flags_enum const flags) const { return m_flags.has(flags); }
private:
	flags_type<	resource_flags_enum, threading::simple_lock	>	m_flags;
};

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_RESOURCE_FLAGS_H_INCLUDED