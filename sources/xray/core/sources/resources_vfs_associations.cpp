////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_vfs_associations.h"
#include <xray/vfs/association.h>

namespace xray {
namespace resources {

resource_base *	  cast_resource_base	(vfs::vfs_association * const association)
{
	if ( !association )
		return								NULL;

	if ( association->type == resource_flag_is_managed_resource )
		return								static_cast_checked<managed_resource *>(association);
	if ( association->type == resource_flag_is_unmanaged_resource )
		return								static_cast_checked<unmanaged_resource *>(association);
	if ( association->type == resource_flag_is_query_result )
		return								static_cast<query_result *>(association);

	NOT_IMPLEMENTED							(return NULL);
}

struct association_callback_helper
{
	association_callback_helper						() 
		: resource(NULL), associated(false), query(NULL), reference_count(0) {}

	void set_associated					(vfs::vfs_association * & association)
	{
		association					=	resource;
	}

	void is_associated					(vfs::vfs_association * & association)
	{
		if ( resource )
			associated					=	(association == resource);
		else
			associated					=	!!resource;
	}

	void get_unmanaged					(vfs::vfs_association * & association)
	{
		if ( (association) && (association)->type == resource_flag_is_unmanaged_resource )
			unmanaged					=	static_cast_checked<unmanaged_resource *>(association);
	}

	void get_managed					(vfs::vfs_association * & association)
	{
		if ( (association) && (association)->type == resource_flag_is_managed_resource )
			managed						=	static_cast_checked<managed_resource *>(association);
	}

	void get_query						(vfs::vfs_association * & association)
	{
		if ( (association) && (association)->type == resource_flag_is_query_result )
			query						=	static_cast<query_result *>(association);
	}

	void try_clean						(vfs::vfs_association * & association)
	{
		R_ASSERT							(association);

		resource_base * const base		=	cast_resource_base(association);
		R_ASSERT							(base);

		base_of_intrusive_base * ref_counted_base	=	base->cast_base_of_intrusive_base();
		if ( (u32)ref_counted_base->get_reference_count() <= reference_count )
		{
			association					=	NULL;
			cleaned						=	true;
		}
	}

	managed_resource_ptr				managed;
	unmanaged_resource_ptr				unmanaged;
	query_result *						query;
	resource_base *						resource;
	u32									reference_count;
	bool								cleaned;
	bool								associated;
};

void   set_associated					(vfs::vfs_iterator it, resource_base * resource)
{
	association_callback_helper				helper;
	helper.resource						=	resource;
	it.access_association					(boost::bind(& association_callback_helper::set_associated, & helper, _1));
}

bool   is_associated					(vfs::vfs_iterator it)
{
	association_callback_helper				helper;
	it.access_association					(boost::bind(& association_callback_helper::is_associated, & helper, _1));
	return									helper.associated;
}

bool   is_associated_with				(vfs::vfs_iterator it, resource_base * resource)
{
	association_callback_helper				helper;
	helper.resource						=	resource;
	it.access_association					(boost::bind(& association_callback_helper::is_associated, & helper, _1));
	return									helper.associated;
}

managed_resource_ptr	get_associated_managed_resource_ptr (vfs::vfs_iterator it)
{
	association_callback_helper				helper;
	it.access_association					(boost::bind(& association_callback_helper::get_managed, & helper, _1));
	return									helper.managed;
}

unmanaged_resource_ptr	get_associated_unmanaged_resource_ptr (vfs::vfs_iterator it)
{
	association_callback_helper				helper;
	it.access_association					(boost::bind(& association_callback_helper::get_unmanaged, & helper, _1));
	return									helper.unmanaged;
}

query_result *   get_associated_query_result (vfs::vfs_iterator it)
{
	association_callback_helper				helper;
	it.access_association					(boost::bind(& association_callback_helper::get_query, & helper, _1));
	return									helper.query;
}

bool   try_clean_associated				(vfs::vfs_iterator it, u32 allow_reference_count)
{
	association_callback_helper				helper;
	helper.reference_count				=	allow_reference_count;	
	it.access_association					(boost::bind(& association_callback_helper::try_clean, & helper, _1));
	return									helper.cleaned;
}

} // namespace resources 
} // namespace xray
