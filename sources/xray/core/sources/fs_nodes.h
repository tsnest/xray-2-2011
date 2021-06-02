////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_NODES_H_INCLUDED
#define FS_NODES_H_INCLUDED

#include "fs_file_system.h"
#include <xray/hash_multiset.h>

namespace xray { 
namespace fs   {

//-----------------------------------------------------------------------------------
// fat_node
//-----------------------------------------------------------------------------------

template <platform_pointer_enum pointer_for_fat_size>
class fat_folder_node;

template <platform_pointer_enum pointer_for_fat_size>
class fat_disk_file_node;

template <platform_pointer_enum pointer_for_fat_size>
class fat_disk_folder_node;

template <platform_pointer_enum pointer_for_fat_size>
class fat_db_file_node;

template <platform_pointer_enum pointer_for_fat_size>
class fat_db_compressed_file_node;

template <platform_pointer_enum pointer_for_fat_size>
class fat_db_inline_file_node;

template <platform_pointer_enum pointer_for_fat_size>
class fat_db_inline_compressed_file_node;

template <platform_pointer_enum pointer_for_fat_size>
class fat_db_soft_link_node;

template <platform_pointer_enum pointer_for_fat_size>
class fat_db_hard_link_node;

class fat_disk_node_base;

struct fat_node_info;

#pragma pack(push, 8)

template <platform_pointer_enum pointer_for_fat_size>
class fat_node
{
public:
	typedef	typename platform_to_file_size_type<pointer_for_fat_size>::file_size_type		file_size_type;

public:
						fat_node				(u16 const flags = 0) : next_overlapped(NULL), m_next(NULL), m_prev(NULL), m_parent(NULL), m_flags(flags) {}
					   ~fat_node				() {}

	fat_node *			get_next				(bool skip_erased = true);
	fat_node const *	get_next				(bool skip_erased = true) const { return const_cast<fat_node *>(this)->get_next(skip_erased); }
	fat_node *			get_prev				() const { return m_prev; }
	fat_node *			get_first_child			(bool skip_erased = true);
	fat_node const *	get_first_child			(bool skip_erased = true) const { return const_cast<fat_node *>(this)->get_first_child(skip_erased); }
	fat_node *			get_parent				() { return m_parent; }
	fat_node const *	get_parent				() const { return m_parent; }
	void				set_next				(fat_node * node) { m_next = node; }
	void				set_prev				(fat_node * node) { m_prev = node; }
	void				set_parent				(fat_node * node) { m_parent = node; }
	fat_node *			get_next_overlapped		() { return next_overlapped; }
	fat_node const *	get_next_overlapped		() const { return next_overlapped; }
	void				set_next_overlapped		(fat_node * node) { next_overlapped = node; }

	pcstr				get_name				() const { return m_name; }
	u32					get_num_children		() const;
	u32					get_num_nodes			() const;
	void				get_num_nodes			(u32 & num_folders, u32 & num_files) const;
	u32					get_file_size			() const;
	u32					get_raw_file_size		() const;
	u32					get_compressed_file_size	() const;
	file_size_type		get_file_offs			() const;
	bool				get_hash				(u32 * out_hash) const;
	void				set_hash				(u32 hash);

	void				get_full_path			(path_string & str) const;
	u32					get_max_fat_size		(fat_inline_data const &	inline_data, 
												 fat_node_info *			info,
												 u32 *						out_nodes_count) const;

	u16					get_flags				() const { return m_flags; }
	bool				has_flags				(u32 flags) const { return ((u32)m_flags & flags) == flags; }
	bool				has_flags_follow_link	(u32 flags) const;

	bool				is_folder				() const { return has_flags(file_system::is_folder); }
	bool				is_disk					() const { return has_flags(file_system::is_disk); }
	bool				is_db					() const { return has_flags(file_system::is_db); }
	bool				is_replicated			() const { return has_flags(file_system::is_replicated); }
	bool				is_compressed			() const { return has_flags(file_system::is_compressed); }
	bool				is_inlined				() const { R_ASSERT(!is_link()); return has_flags(file_system::is_inlined); }
	bool				is_sub_fat				() const { return has_flags(file_system::is_sub_fat); }
	bool				is_soft_link			() const { return has_flags(file_system::is_soft_link); }
	bool				is_hard_link			() const { return has_flags(file_system::is_hard_link); }
	bool				is_link					() const { return is_soft_link() || is_hard_link(); }
	bool				is_root_of_sub_fat		() const;

	void								set_associated	(resources::resource_base * resource);
	bool								try_clean_associated (u32 allow_reference_count);
	resources::resource_base * &		ref_associated_ptr							();
	resources::managed_resource_ptr		get_associated_managed_resource_ptr			();
	resources::unmanaged_resource_ptr	get_associated_unmanaged_resource_ptr		();
	resources::query_result *			get_associated_query_result					();

	bool								is_associated								() const;
	bool								is_associated_with							(resources::resource_base * resource_base) const;

	bool								set_inline_data			(const_buffer const & buffer);
	bool								get_inline_data			(const_buffer * out_buffer) const;

	fat_folder_node<pointer_for_fat_size> *						cast_folder						();
	fat_disk_folder_node<pointer_for_fat_size> *				cast_disk_folder				();
	fat_disk_file_node<pointer_for_fat_size> *					cast_disk_file					();
	fat_db_file_node<pointer_for_fat_size> *					cast_db_file					();
	fat_db_compressed_file_node<pointer_for_fat_size> *			cast_db_compressed_file			();
	fat_db_inline_file_node<pointer_for_fat_size> *				cast_db_inline_file				();
	fat_db_inline_compressed_file_node<pointer_for_fat_size> *	cast_db_inline_compressed_file	();
	fat_db_soft_link_node<pointer_for_fat_size> *				cast_db_soft_link_node			();
	fat_db_hard_link_node<pointer_for_fat_size> *				cast_db_hard_link_node			();
	fat_disk_node_base *										cast_disk_node_base				();

	fat_folder_node<pointer_for_fat_size> const *				cast_folder			() const { return const_cast<fat_node *>(this)->cast_folder(); }
	fat_disk_folder_node<pointer_for_fat_size> const *			cast_disk_folder	() const { return const_cast<fat_node *>(this)->cast_disk_folder(); }
	fat_disk_file_node<pointer_for_fat_size> const *			cast_disk_file		() const { return const_cast<fat_node *>(this)->cast_disk_file(); }
	fat_db_file_node<pointer_for_fat_size> const *				cast_db_file		() const { return const_cast<fat_node *>(this)->cast_db_file(); }
	fat_db_compressed_file_node<pointer_for_fat_size> const *	cast_db_compressed_file	() const { return const_cast<fat_node *>(this)->cast_db_compressed_file(); }
	fat_db_inline_file_node<pointer_for_fat_size> const *		cast_db_inline_file		() const { return const_cast<fat_node *>(this)->cast_db_inline_file(); }
	fat_db_inline_compressed_file_node<pointer_for_fat_size> const *	cast_db_inline_compressed_file	() const { return const_cast<fat_node *>(this)->cast_db_inline_compressed_file(); }
	fat_db_soft_link_node<pointer_for_fat_size> const *			cast_db_soft_link_node	() const { return const_cast<fat_node *>(this)->cast_db_soft_link_node(); }
	fat_db_hard_link_node<pointer_for_fat_size> const *			cast_db_hard_link_node	() const { return const_cast<fat_node *>(this)->cast_db_hard_link_node(); }
	fat_disk_node_base const *									cast_disk_node_base		() const { return const_cast<fat_node *>(this)->cast_disk_node_base(); }

	fat_folder_node<pointer_for_fat_size> *						cast_folder_if_not_link	() { if ( is_link() ) return NULL; return cast_folder(); }
	fat_folder_node<pointer_for_fat_size> const *				cast_folder_if_not_link	() const { if ( is_link() ) return NULL; return cast_folder(); }

	bool								operator ==			(fat_node const & n) const;
	bool								same_path			(fat_node const * n) const;

	void								reverse_bytes_for_real_class	();
	void								reverse_bytes		();

	// very special 4-bit counter, suited for interlocked inc/dec
	threading::atomic32_type &			ref_flags_as_32bit	() const;

	u32									get_sizeof			() const;
	fat_node<pointer_for_fat_size> *	find_referenced_link_node	() const;
	void								find_link_target_path		(path_string * out_path) const;

	typedef	typename platform_pointer<fat_node, pointer_for_fat_size>::type	pointer_to_node;

private:
	resources::resource_base *			get_associated_unsecure		() const;
	u32									get_max_fat_size_impl		(fat_inline_data const &	inline_data, 
																	 fat_node_info *			info,
																	 fat_node_info *			root_info,
																	 u32 *						out_nodes_count) const;

	void								get_associated_ptr_helper	(resources::managed_resource_ptr *	result_managed, 
																     resources::unmanaged_resource_ptr *	result_unmanaged,
																     resources::query_result * *			result_query_result);
	void								lock_associated		() const;
	void								unlock_associated	() const;

public: 
	pointer_to_node						next_overlapped; // for hashset
private:
	pointer_to_node						m_next;
	pointer_to_node						m_prev;
	pointer_to_node						m_parent;

	// this ordering is very important and 2 bytes after m_flags must be allocated
	// only interlocked operations are allowed on m_flags
	u16									m_flags;

#pragma warning (push)
#pragma warning (disable:4200)
	char								m_name[];
#pragma warning (pop)

	friend								class				file_system_impl;
	friend								class				file_system_iterator;
	
	template <platform_pointer_enum pointer_for_fat_size1>
	friend								class				fat_folder_node;
	template <platform_pointer_enum pointer_for_fat_size1>
	friend								class				fat_disk_file_node;
	template <platform_pointer_enum pointer_for_fat_size1>
	friend								class				fat_db_file_node;
	template <platform_pointer_enum pointer_for_fat_size1>
	friend								class				fat_node;

	template <platform_pointer_enum target_pointer_size>
	friend								struct				save_nodes_helper;
	template <platform_pointer_enum target_pointer_size>
	friend								class				save_db_impl;
}; // class fat_node

#pragma pack(pop)

//-----------------------------------------------------------------------------------
// detail::node_hash and detail::node_cmp
//-----------------------------------------------------------------------------------

typedef hash_multiset< fat_node<>, fat_node<>::pointer_to_node, & fat_node<>::next_overlapped, detail::fixed_size_policy<1024*32> > node_hash_set;

//-----------------------------------------------------------------------------------
// fat_folder_node
//-----------------------------------------------------------------------------------

template <platform_pointer_enum pointer_for_fat_size = platform_pointer_default>
class fat_folder_node
{
public:
	typedef		fat_node<pointer_for_fat_size>		node_type;

public:
								fat_folder_node			() : m_first_child(NULL) {}

	void						transfer_children		(fat_folder_node * dest, bool no_duplicate_names);

	node_type *					get_first_child 		(bool skip_erased = true);		
	node_type const *			get_first_child 		(bool skip_erased = true) const { return ((fat_folder_node<pointer_for_fat_size> *)this)->get_first_child(skip_erased); }
	bool						find_child				(node_type const * seeking_child) const;
	node_type *					find_child				(pcstr name);
	node_type const *			find_child				(pcstr name) const 
														{ return const_cast<fat_folder_node *>(this)->find_child(name); }

	void						prepend_child			(node_type * child);
	void						unlink_child			(node_type * child, bool zero_childs_parent);

	node_type *					cast_node				() { return & m_base; }
	node_type const *			cast_node				() const { return const_cast<fat_folder_node *>(this)->cast_node(); }
	
	fat_disk_folder_node<pointer_for_fat_size> *	
								cast_disk_folder_node	() { return cast_node()->cast_disk_folder(); }

	void						reverse_bytes			();
	void						swap_child_with_next_child	(node_type * child);

public:
	typename platform_pointer<node_type, pointer_for_fat_size>::type	m_first_child;
	node_type					m_base;
}; // fat_folder_node

//-----------------------------------------------------------------------------------
// fat_disk_node_base
//-----------------------------------------------------------------------------------

class fat_disk_node_base
{
public:
	enum	flags_enum 	{	flag_scanned				=	1 << 0, 
							flag_scanned_recursively	=	1 << 1, 
							flag_erased					=	1 << 2,		
							flag_just_created			=	1 << 3, };
public:

			fat_disk_node_base		() : m_node_lock_count(0), m_flags(0) {}
			~fat_disk_node_base		() { R_ASSERT(!m_node_lock_count); }
	void	set_node_is_locked		(bool lock);
	bool	is_scanned				(u32 recursively) const { return m_flags.has((flag_scanned | (recursively ? flag_scanned_recursively : 0))); }
	void	set_is_scanned			(u32 recursively) { m_flags.set(flag_scanned | (recursively ? flag_scanned_recursively : 0)); }
	bool	is_erased				() const { return m_flags.has(flag_erased); }
	void	set_is_erased			() { m_flags.set(flag_erased); }

	void	set_just_created		() { m_flags.set(flag_just_created); }
	void	unset_just_created		() { m_flags.unset(flag_just_created); }
	bool	is_just_created			() const { return m_flags.has(flag_just_created); }

private:
 	threading::atomic32_type		m_node_lock_count;	// node is locked during loading, until callback is called
	flags_type<flags_enum, threading::multi_threading_policy>	m_flags;
};

//-----------------------------------------------------------------------------------
// fat_disk_folder_node
//-----------------------------------------------------------------------------------

template <platform_pointer_enum pointer_for_fat_size = platform_pointer_default>
class fat_disk_folder_node : public fat_disk_node_base
{
public:
	fat_disk_folder_node	() : m_disk_path(NULL) {}

	fat_folder_node<pointer_for_fat_size> *		cast_folder_node	()	{ return & m_folder; }
	fat_node<pointer_for_fat_size> *			cast_node			()	{ return & m_folder.m_base; }
	pcstr										disk_path			() const { return m_disk_path; }

public:
	pstr										m_disk_path; // pointing to the end of the object, no need to free
	fat_folder_node<pointer_for_fat_size>		m_folder;
};

//-----------------------------------------------------------------------------------
// fat_db_file_node_base
//-----------------------------------------------------------------------------------

template <platform_pointer_enum pointer_for_fat_size = platform_pointer_default>
class fat_db_file_node_base
{
public:
	typedef	typename platform_to_file_size_type<pointer_for_fat_size>::file_size_type		file_size_type;
public:
										fat_db_file_node_base() : m_resource_base(NULL), m_pos(0), m_size(0), m_hash(0) {}

	u32									size			() const { return m_size; }
	file_size_type						offs_in_db		() const { return m_pos; }
	void								reverse_bytes	();
	void								set_hash		(u32 hash) { m_hash = hash; }
	u32									hash			() const { return m_hash; }

public:
	typename platform_pointer<resources::resource_base, pointer_for_fat_size>::type	m_resource_base;
	file_size_type										m_pos;
	u32													m_size;
	u32													m_hash;
};

//-----------------------------------------------------------------------------------
// fat_db_ref_node_base
//-----------------------------------------------------------------------------------

template <platform_pointer_enum pointer_for_fat_size = platform_pointer_default>
class fat_db_soft_link_node
{
public:
										fat_db_soft_link_node() : m_relative_path_to_referenced(NULL), m_base(file_system::is_soft_link) {}
	void								reverse_bytes	();
	fat_node<pointer_for_fat_size> *	cast_node		() { return & m_base; }
	fat_node<pointer_for_fat_size> const *	cast_node	() const { return & m_base; }
	pcstr								relative_path_to_referenced	() const { return m_relative_path_to_referenced; }
	void								absolute_path_to_referenced	(path_string * out_path) const;

public:
	typename platform_pointer<char, pointer_for_fat_size>::type						m_relative_path_to_referenced;
	fat_node<pointer_for_fat_size>		m_base;
};

template <platform_pointer_enum pointer_for_fat_size = platform_pointer_default>
class fat_db_hard_link_node
{
public:									
	typedef	fat_node<pointer_for_fat_size>	node_type;
public:									
	fat_db_hard_link_node() : m_reference_node(NULL), m_base(file_system::is_hard_link) {}
	void								reverse_bytes	();
	fat_node<pointer_for_fat_size> *	cast_node		() { return & m_base; }

	fat_node<pointer_for_fat_size> *	reference_node	() const { return m_reference_node; }

public:
	typename platform_pointer<node_type, pointer_for_fat_size>::type				m_reference_node;
	node_type							m_base;
};

//-----------------------------------------------------------------------------------
// fat_db_file_node
//-----------------------------------------------------------------------------------

template <platform_pointer_enum pointer_for_fat_size = platform_pointer_default>
class fat_db_file_node : public fat_db_file_node_base<pointer_for_fat_size>
{
public:
	void								reverse_bytes	();
	fat_node<pointer_for_fat_size> *	cast_node		()		{ return & m_base; }

public:
	fat_node<pointer_for_fat_size>		m_base;
}; // class fat_db_file_node

//-----------------------------------------------------------------------------------
// fat_db_compressed_file_node
//-----------------------------------------------------------------------------------

template <platform_pointer_enum pointer_for_fat_size = platform_pointer_default>
class fat_db_compressed_file_node : public fat_db_file_node_base<pointer_for_fat_size>
{
public:
										fat_db_compressed_file_node() :	m_uncompressed_size(0) {}
	void								reverse_bytes	();
	fat_node<pointer_for_fat_size> *	cast_node		()		{ return & m_base; }

public:
	u32									m_uncompressed_size;
	fat_node<pointer_for_fat_size>		m_base;

}; // class fat_db_compressed_file_node

//-----------------------------------------------------------------------------------
// fat_inline_base
//-----------------------------------------------------------------------------------

template <platform_pointer_enum pointer_for_fat_size = platform_pointer_default>
class fat_inline_base
{
public:
										fat_inline_base () : m_inlined_data(0), m_inlined_size(0) {}

	void								set_inlined_data(const_buffer const & buffer) 
	{ 
		m_inlined_data				=	(pcstr)buffer.c_ptr(); 
		m_inlined_size				=	(u32)buffer.size(); 
	}
	const_buffer						get_inlined_data() const;
	void								reverse_bytes	();

public:
	typename platform_pointer<char const, pointer_for_fat_size>::type	m_inlined_data;
	u32																	m_inlined_size;

}; // class fat_inline_base

//-----------------------------------------------------------------------------------
// fat_db_inline_file_node
//-----------------------------------------------------------------------------------

template <platform_pointer_enum pointer_for_fat_size = platform_pointer_default>
class fat_db_inline_file_node : public fat_db_file_node_base<pointer_for_fat_size>, 
								public fat_inline_base<pointer_for_fat_size>
{
public:
										fat_db_inline_file_node () {}

	void								reverse_bytes	();
	fat_node<pointer_for_fat_size> *	cast_node		()		{ return & m_base; }

public:
	fat_node<pointer_for_fat_size>		m_base;

}; // class fat_db_inline_file_node

//-----------------------------------------------------------------------------------
// fat_db_inline_compressed_file_node
//-----------------------------------------------------------------------------------

template <platform_pointer_enum pointer_for_fat_size = platform_pointer_default>
class fat_db_inline_compressed_file_node :	public fat_db_file_node_base<pointer_for_fat_size>, 
											public fat_inline_base<pointer_for_fat_size>
{
public:
										fat_db_inline_compressed_file_node() :	m_uncompressed_size(0) {}
	void								reverse_bytes	();
	fat_node<pointer_for_fat_size> *	cast_node		()	{ return & m_base; }

public:
	u32									m_uncompressed_size;
	fat_node<pointer_for_fat_size>		m_base;

}; // class fat_db_inline_compressed_file_node


//-----------------------------------------------------------------------------------
// fat_disk_node
//-----------------------------------------------------------------------------------

template <platform_pointer_enum pointer_for_fat_size = platform_pointer_default>
class fat_disk_file_node : public fat_disk_node_base
{
public:
	fat_disk_file_node   									() : m_resource_base(NULL), m_disk_path(NULL), m_size(0), m_is_db(2) {}

	pcstr								disk_path			() const	{ return m_disk_path; }
	u32									size				() const	{ return (u32)m_size; }
	fat_node<pointer_for_fat_size> *	cast_node 			()			{ return & m_base; }
	bool								checked_is_db		() const	{ return m_is_db != 2; }
	void								set_checked_is_db	(bool is_db){ m_is_db = (u8)is_db; }
	bool								is_db_file			() const	{ ASSERT(checked_is_db()); return m_is_db == 1; }

public:
	resources::resource_base *			m_resource_base;
	pstr								m_disk_path;	// pointing to the end of the object, no need to free
	threading::atomic32_type			m_size;
	u8									m_is_db;
	fat_node<pointer_for_fat_size>		m_base;
}; // class fat_disk_node

namespace detail {
inline
int		bits_to_flags_byte				() { return	platform::big_endian() ? 16 : 0; }
} // namespace detail

template <platform_pointer_enum pointer_for_fat_size>
bool   is_erased_node (fat_node<pointer_for_fat_size> * node)
{	
	if ( node->is_link() )
		return								false;
	if ( fat_disk_node_base * base = node->cast_disk_node_base() )
		return								base->is_erased();

	return									false;
}

void	set_fat_node_is_locked			(fat_node<> * node, bool lock);

} // namespace fs 
} // namespace xray 

#endif // FS_NODES_H_INCLUDED