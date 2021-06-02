////////////////////////////////////////////////////////////////////////////
//	Created		: 24.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_FS_ITERATOR_H_INCLUDED_
#define XRAY_RESOURCES_FS_ITERATOR_H_INCLUDED_

#include <xray/fs_platform_configuration.h>
#include <xray/vfs/locked_iterator.h>

#if 0

namespace xray {
namespace fs {
	class	file_system_impl;
} // namespace fs
namespace resources {

class					fs_iterator;

template < typename object_type, typename base_type >
class					resource_ptr;
typedef	resource_ptr<managed_resource, managed_intrusive_base>			managed_resource_ptr;
typedef	resource_ptr<unmanaged_resource, unmanaged_intrusive_base>		unmanaged_resource_ptr;

class					query_result;



class XRAY_CORE_API fs_iterator
{
public:
	enum type_enum				{	type_unset, type_recursive, type_non_recursive, type_not_scanned, type_temp_node	};
public:
								fs_iterator			()	: m_fat_node(NULL), m_link_target(NULL), m_type(type_unset) {}
								fs_iterator			(fs::fat_node<> * fat_it, fs::fat_node<> * link_target, type_enum type = type_non_recursive)
														: m_fat_node(fat_it), m_link_target(link_target), m_type(type) {}
								fs_iterator			(fs_iterator const & it) : m_fat_node(it.m_fat_node), m_link_target(it.m_link_target), m_type(it.m_type) {}

	static fs_iterator			end					();

	fs_iterator					find_child			(pcstr relative_path);	
	pcstr						get_name			() const;
	void						get_full_path		(fs::path_string &) const;
	fs::path_string				get_full_path		() const;
	void						get_disk_path		(fs::path_string &) const;
	
	u32							get_num_children	() const; // O(n)
	u32							get_num_nodes		() const; // O(n)
	bool						is_folder			() const;

	bool						is_disk				() const;
	bool						is_db				() const;
	bool						is_replicated		() const;
	bool						is_compressed		() const;
	bool						is_inlined			() const;
	bool						is_inlined_follow_link	()	const;

	u32							get_file_size		() const;
	file_size_type				get_file_offs		() const;
	u32							get_compressed_file_size	() const;
	u32							get_raw_file_size	() const;

	fs_iterator					children_begin		() const;
	fs_iterator					children_end		() const;

	bool						is_end				() const { return m_fat_node == NULL; }
	fs::fat_node<> *			get_fat_node		() const { return m_fat_node; }
	fs::fat_node<> *			get_link_target		() const { return m_link_target; }

	bool						get_hash			(u32 * out_hash) const;
								operator bool		() const { return m_fat_node != NULL; }

	fs_iterator					operator ++			();
	fs_iterator					operator ++			(int);
	bool						operator ==			(const fs_iterator & it) const;
	bool						operator !=			(const fs_iterator & it) const;
	bool						operator <			(const fs_iterator it)	const;

	void						set_is_locked		(bool lock);

private:
	void								set_associated							(resources::resource_base * resource);
	bool  								is_associated							() const;
	bool  								is_associated_with						(resources::resource_base * resource) const;
	resources::managed_resource_ptr		get_associated_managed_resource_ptr		() const;
	resources::unmanaged_resource_ptr	get_associated_unmanaged_resource_ptr	() const;
	resources::query_result *			get_associated_query_result				() const;
	bool								get_inline_data							(const_buffer * out_buffer) const;
	bool								try_clean_associated	(u32 reference_count) const;

	fs::fat_node<> *					follow_link_node						() const { return m_link_target ? m_link_target : m_fat_node; }
protected:
	fs::fat_node<> *		m_fat_node;
	fs::fat_node<> *		m_link_target;
	type_enum				m_type;

	friend	class			fs_task;
	friend	class			::xray::fs::file_system_impl;
	friend	class			query_result;
	friend	class			resource_base;
	friend	class			resources_manager;
	friend	class			base_of_intrusive_base;
	friend	class			sub_fat_pin_fs_iterator;

	//friend  void XRAY_CORE_API	query_fs_iterator	(pcstr, query_vfs_iterator_callback, memory::base_allocator*, recursive_bool, query_result_for_cook *, bool);
};

class XRAY_CORE_API sub_fat_pin_fs_iterator : public fs_iterator
{
public:
			sub_fat_pin_fs_iterator		() { }
			~sub_fat_pin_fs_iterator	();	

	void	grab						(sub_fat_pin_fs_iterator & other);
	void	clear_without_unpin			();
	void	assign						(fs::fat_node<> * node, fs::fat_node<> * link_target, type_enum type = type_non_recursive);
	void	assign						(fs_iterator it);
	void	assign_no_pin				(fs_iterator it);

private:
	void	operator =					(fs_iterator)						{ NOT_IMPLEMENTED(); }
			sub_fat_pin_fs_iterator		(sub_fat_pin_fs_iterator const &)	{ NOT_IMPLEMENTED(); }

	void	assign_no_pin				(fs::fat_node<> * node, fs::fat_node<> * link_target, type_enum type);
	void	assign_impl					(fs::fat_node<> * node, fs::fat_node<> * link_target, type_enum type, bool do_pin);
	void	unpin_sub_fats_if_needed	();

	friend	class	::xray::fs::file_system_impl;
};

} // namespace resources
} // namespace xray

#endif // #if 0

#endif // #ifndef XRAY_RESOURCES_FS_ITERATOR_H_INCLUDED_
