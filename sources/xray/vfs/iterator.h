////////////////////////////////////////////////////////////////////////////
//	Created		: 04.05.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_VFS_ITERATOR_H_INCLUDED_
#define XRAY_RESOURCES_VFS_ITERATOR_H_INCLUDED_

#include <xray/vfs/api.h>
#include <xray/vfs/platform_configuration.h>
#include <xray/fs/virtual_path_string.h>
#include <xray/fs/native_path_string.h>
#include <xray/vfs/base_node.h>

namespace xray {
namespace vfs {

class	vfs_hashset;

class XRAY_VFS_API vfs_iterator
{
public:
	enum type_enum						{	type_unset, type_recursive, type_non_recursive, 
											type_not_scanned, type_temp_node	};
public:
						vfs_iterator	();
						vfs_iterator	(base_node<> * fat_it, base_node<> * link_target, 
										 vfs_hashset * hashset, type_enum type = type_non_recursive);
						vfs_iterator	(vfs_iterator const & it);

	static vfs_iterator	end				();

	vfs_iterator		find_child		(fs_new::virtual_path_string const & relative_path) const;
	pcstr				get_name		() const;
	fs_new::virtual_path_string		get_virtual_path	() const;
	fs_new::native_path_string		get_physical_path	() const;
	
	u32					get_children_count	() const;	// O(n)
	u32					get_nodes_count	() const;		// O(n)
	
	bool				is_file			() const;
	bool				is_folder		() const;
	bool				is_physical		() const;
	bool				is_archive		() const;
	bool				is_archive_check_overlapped	() const;
	bool				is_replicated	() const;
	bool				is_compressed	() const;
	bool				is_inlined		() const;
	bool				is_link			() const;
	bool				link_target_is_inlined	() const;
	bool				is_expanded		() const;

	u32					get_file_size	() const;
	file_size_type		get_file_offs	() const;
	u32					get_compressed_file_size	() const;
	u32					get_raw_file_size	() const;

	vfs_iterator		children_begin	() const;
	vfs_iterator		children_end	() const;

	bool				is_end			() const { return m_node == NULL; }
	base_node<> *		get_node		() const { return m_node; }
	base_node<> *		get_link_target	() const { return m_link_target; }
	base_node<> *		data_node		() const { return m_link_target ? m_link_target : m_node; }

	u32					get_file_hash	() const;
	bool				get_inline_data	(const_buffer * out_buffer) const;
	operator			bool			() const { return m_node != NULL; }

	vfs_iterator		operator ++		();
	vfs_iterator		operator ++		(int);
	bool				operator ==		(const vfs_iterator & it) const;
	bool				operator !=		(const vfs_iterator & it) const;
	bool				operator <		(const vfs_iterator it)	const;

	void				access_association	(association_callback const & callback);

protected:
	vfs_hashset *		m_hashset;
	base_node<> *		m_node;
	base_node<> *		m_link_target;
	type_enum			m_type;

	friend class		vfs_locked_iterator;
};

void   log_vfs_iterator					(vfs_iterator const & it);

} // namespace vfs
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_FS_ITERATOR_H_INCLUDED_