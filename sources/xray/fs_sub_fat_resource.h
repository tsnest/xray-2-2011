////////////////////////////////////////////////////////////////////////////
//	Created		: 11.10.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_FS_SUB_FAT_RESOURCE_H_INCLUDED
#define XRAY_FS_SUB_FAT_RESOURCE_H_INCLUDED

#include <xray/fs_platform_configuration.h>
#include <xray/resources_resource.h>
#include <xray/resources_fs.h>
#include <xray/intrusive_double_linked_list.h>
#include <boost/intrusive/set.hpp>

namespace xray {
namespace resources {
class	sub_fat_pin_fs_iterator;
} // namespace resources

namespace fs {

typedef boost::intrusive::set_member_hook< boost::intrusive::link_mode<boost::intrusive::auto_unlink> >		sub_fat_resource_tree_member_hook;

static const pcstr	big_endian_string		=	"big-endian";
static const pcstr	little_endian_string	=	"little-endian";

struct	fat_header
{
private:
	char	endian_string[14];
public:
	u32		num_nodes;
	u32		buffer_size;

			fat_header			() : num_nodes(0), buffer_size(0) { memory::zero(endian_string); }

	void 	set_big_endian		();
	void 	set_little_endian	();
	bool 	is_big_endian		();
	void 	reverse_bytes		();
};

class sub_fat_resource;

class sub_fat_resource_base
{
public:
	sub_fat_resource *		m_next_child;
	sub_fat_resource *		m_prev_child;

	sub_fat_resource_base	() : m_next_child(NULL), m_prev_child(NULL) {}
};

enum	{	cook_sub_fat_and_all_levels_of_children	=	1,
			cook_sub_fat_and_one_level_of_children	=	2	};

class sub_fat_resource :	public sub_fat_resource_base, 
							public resources::unmanaged_resource
{
public:
	typedef resources::resource_ptr<sub_fat_resource, resources::unmanaged_intrusive_base>	pointer;

public:
	sub_fat_resource	(fat_header const & in_header, u32 raw_buffer_size) 
		: next(NULL), raw_buffer_size(raw_buffer_size), root_node(NULL), sub_fat_node(NULL), is_automatically_mounted(false)
	{
		raw_buffer						=	(pstr)this + sizeof(sub_fat_resource);
		header							=	(fat_header *)raw_buffer;
		* header						=	in_header;
		schedule_unmount_on_dying		=	true;
		is_mounted						=	false;
	}

	sub_fat_resource_tree_member_hook	tree_hook;
	fat_header *						header;
	pointer								next;
	path_string							archive_physical_path;
	path_string							fat_physical_path;
	path_string							virtual_path;
	pstr								raw_buffer;
	u32									raw_buffer_size;
	fat_node<> *						root_node;
	fat_node<> *						sub_fat_node;
	pointer								parent_db;
	bool								schedule_unmount_on_dying;
	bool								is_mounted;
	bool								is_automatically_mounted;

	intrusive_double_linked_list<	sub_fat_resource_base, 
									sub_fat_resource *, 									
									& sub_fat_resource_base::m_prev_child, 
									& sub_fat_resource_base::m_next_child, 
									threading::single_threading_policy >	
										children;

	pstr			nodes_buffer		() const { return raw_buffer + sizeof(fat_header); }
	u32				nodes_buffer_size	() const { return raw_buffer_size - sizeof(fat_header); }
	void			remove_from_parent	();
	void			add_child			(sub_fat_resource * child);
	void			remove_child		(sub_fat_resource * child);
	
	bool			contains			(fat_node<> const * work_node) const;
	bool			contains_sibling	(sub_fat_resource * sibling) const;
	u32				change_reference_count_change_may_destroy_this	(int change);
};

typedef sub_fat_resource::pointer		sub_fat_resource_ptr;
typedef	resources::child_resource_ptr	<sub_fat_resource, resources::unmanaged_intrusive_base>	sub_fat_child_resource_ptr;

typedef	boost::function< void ( resources::fs_iterator iterator ) >	find_callback;
template class XRAY_CORE_API boost::function< void ( resources::fs_iterator ) >;

struct XRAY_CORE_API find_results_struct
{
	enum type_enum		{ type_no_pin, type_pin };
	find_results_struct (type_enum type) : type(type) {}
	
	bool		is_no_pin	() const { return type == type_no_pin; }
	bool		is_pin		() const { return type == type_pin; }

	// no pin case:
	sub_fat_resource_ptr				sub_fat;
	sub_fat_resource_ptr				sub_fat_of_link;
	// pin case:
	resources::sub_fat_pin_fs_iterator	pin_iterator;
	// shared:
	find_callback						callback;
	type_enum							type;
};

class compare_sub_fat_resource
{
	public: bool	operator () (sub_fat_resource const & left, sub_fat_resource const & right) const { return & left < & right; }
};

typedef boost::intrusive::member_hook
<
	sub_fat_resource, 
	sub_fat_resource_tree_member_hook,
	& sub_fat_resource::tree_hook 
> sub_fat_resource_tree_helper_option;

typedef	boost::intrusive::multiset< sub_fat_resource, 
							  	 	sub_fat_resource_tree_helper_option, 
							  	 	boost::intrusive::compare< compare_sub_fat_resource >,
							  	 	boost::intrusive::constant_time_size<false> >	sub_fat_resource_tree_type;
} // namespace fs
} // namespace xray

#endif // #ifndef XRAY_FS_SUB_FAT_RESOURCE_H_INCLUDED