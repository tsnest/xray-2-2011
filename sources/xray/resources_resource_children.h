////////////////////////////////////////////////////////////////////////////
//	Created		: 09.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_RESOURCE_CHILDREN_H_INCLUDED
#define XRAY_RESOURCES_RESOURCE_CHILDREN_H_INCLUDED

#include <xray/resources_resource_link.h>
#include <xray/resources_resource_flags.h>
#include <xray/resources_resource_reconstruction_info.h>
#include <xray/type_uid_object.h>

namespace xray {
namespace resources {

class XRAY_CORE_API resource_children : public resource_flags,
										public resource_reconstruction_info,
										public uid_object<resource_children>
{
protected:	
								resource_children		(resource_flags_enum flags) : resource_flags(flags) {}

	void						link_parent_resource	(resource_base * parent, u32 quality = resource_link::not_a_quality_link);
	virtual void				link_child_resource		(resource_base * child,  u32 quality = resource_link::not_a_quality_link);
	void						unlink_parent_resource	(resource_base * parent);
	virtual void				unlink_child_resource	(resource_base * child);
	void						unlink_from_children	();
	void						unlink_from_parents		();

	u32							get_parents_count		() const { return m_parent_resources.size(); }
	resource_link_list &		get_parents				() { return m_parent_resources; }
	resource_link_list const &	get_parents				() const { return m_parent_resources; }
	resource_link_list &		get_children			() { return m_children_resources; }
	bool						has_child				(resource_base * child);

protected:
	resource_link_list			m_children_resources;
	resource_link_list			m_parent_resources;

	friend class				resource_reconstruction_info;
};

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_RESOURCE_CHILDREN_H_INCLUDED