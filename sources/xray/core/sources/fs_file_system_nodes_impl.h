////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_FILE_SYSTEM_NODES_IMPL_H_INCLUDED
#define FS_FILE_SYSTEM_NODES_IMPL_H_INCLUDED

#include "fs_file_system.h"
#include <xray/hash_multiset.h>

namespace xray {

namespace fs   {

//-----------------------------------------------------------------------------------
// detail::node_hash and detail::node_cmp
//-----------------------------------------------------------------------------------

typedef hash_multiset< fat_node<>, detail::fixed_size_policy<1024*32> > node_hash_set;

//-----------------------------------------------------------------------------------
// fat_node
//-----------------------------------------------------------------------------------

template <pointer_for_fat_size_enum pointer_for_fat_size>
class fat_folder_node;

template <pointer_for_fat_size_enum pointer_for_fat_size>
class fat_disk_file_node;

template <pointer_for_fat_size_enum pointer_for_fat_size>
class fat_disk_folder_node;

template <pointer_for_fat_size_enum pointer_for_fat_size>
class fat_db_file_node;

template <pointer_for_fat_size_enum pointer_for_fat_size>
class fat_db_compressed_file_node;

template <pointer_for_fat_size_enum pointer_for_fat_size>
class fat_db_inline_file_node;

template <pointer_for_fat_size_enum pointer_for_fat_size>
class fat_db_inline_compressed_file_node;

template <class T, pointer_for_fat_size_enum  pointer_for_fat_size >
struct pointer_for_fat_select_helper
{
	typedef T *		type;
};

template <class T>
struct pointer_for_fat_select_helper<T,pointer_for_fat_64bit>
{
	struct helper
	{
		union
		{
			T *		pointer;
			u64		max_storage;
		};

					helper				() { max_storage = 0; }
					helper				(T * const in_pointer)	{ max_storage = 0; pointer = in_pointer; }
		void		operator =			(T * const in_pointer)	{ pointer = in_pointer; }
		T *			operator ->			()						{ return pointer; }
		T const *	operator ->			() const				{ return pointer; }
		T &			operator *			()						{ return * pointer; }
		T const &	operator *			() const				{ return * pointer; }

					operator T * &		()						{ return pointer; }
					operator T *		() const				{ return pointer; }
	};

	typedef helper	type;
};

template <class T, pointer_for_fat_size_enum pointer_for_fat_size>
struct pointer_for_fat
{
	typedef typename pointer_for_fat_select_helper<T,pointer_for_fat_size>::type	type;
};

template <pointer_for_fat_size_enum pointer_for_fat_size>
class fat_node_intrusive_base
{
private:
	typename pointer_for_fat<fat_node<pointer_for_fat_size>, pointer_for_fat_size>::type	m_next_hash_node;
public:
	fat_node_intrusive_base() : m_next_hash_node(NULL)		{}
	void								set_next_hash_node (fat_node<pointer_for_fat_size> * node)	{ m_next_hash_node = node; }
	fat_node<pointer_for_fat_size> *	get_next_hash_node () { return (fat_node<pointer_for_fat_size> *) m_next_hash_node; }
};

template <pointer_for_fat_size_enum pointer_for_fat_size>
class fat_node : public fat_node_intrusive_base<pointer_for_fat_size>
{
public:
	typedef	typename platform_to_file_size_type<pointer_for_fat_size>::file_size_type		file_size_type;

public:
						fat_node				() : m_next(NULL), m_prev(NULL), m_parent(NULL) {}
					   ~fat_node				() {}

	fat_node *			get_next				()       { return m_next; }
	fat_node const *	get_next				() const { return const_cast<fat_node *>(this)->get_next(); }
	fat_node *			get_first_child			();
	fat_node const *	get_first_child			() const { return const_cast<fat_node *>(this)->get_first_child(); }

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
	u32					get_max_fat_size		(fat_inline_data const & inline_data) const;

	int					get_flags				() const { return m_flags; }
	bool				has_flags				(u32 flags) const { return ((u32)m_flags & flags) != 0; }

	bool				is_folder				() const { return has_flags(file_system::is_folder); }
	bool				is_disk					() const { return has_flags(file_system::is_disk); }
	bool				is_db					() const { return has_flags(file_system::is_db); }
	bool				is_replicated			() const { return has_flags(file_system::is_replicated); }
	bool				is_compressed			() const { return has_flags(file_system::is_compressed); }
	bool				is_inlined				() const { return has_flags(file_system::is_inlined); }

	void								set_associated	(resources::resource_base * resource);
	bool								try_clean_associated_if_zero_reference_resource ();
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

	fat_folder_node<pointer_for_fat_size> const *				cast_folder			() const { return const_cast<fat_node *>(this)->cast_folder(); }
	fat_disk_folder_node<pointer_for_fat_size> const *			cast_disk_folder	() const { return const_cast<fat_node *>(this)->cast_disk_folder(); }
	fat_disk_file_node<pointer_for_fat_size> const *			cast_disk_file		() const { return const_cast<fat_node *>(this)->cast_disk_file(); }
	fat_db_file_node<pointer_for_fat_size> const *				cast_db_file		() const { return const_cast<fat_node *>(this)->cast_db_file(); }
	fat_db_compressed_file_node<pointer_for_fat_size> const *	cast_db_compressed_file	() const { return const_cast<fat_node *>(this)->cast_db_compressed_file(); }
	fat_db_inline_file_node<pointer_for_fat_size> const *				cast_db_inline_file		() const { return const_cast<fat_node *>(this)->cast_db_inline_file(); }
	fat_db_inline_compressed_file_node<pointer_for_fat_size> const *	cast_db_inline_compressed_file	() const { return const_cast<fat_node *>(this)->cast_db_inline_compressed_file(); }

	bool								operator ==			(fat_node const & n) const;
	bool								same_path			(fat_node const * n) const;

	void								reverse_bytes_for_real_class	();
	void								reverse_bytes		();

	// very special 4-bit counter, suited for interlocked inc/dec
	threading::atomic32_type &			ref_flags_as_32bit	() const;

private:
	resources::resource_base *			get_associated_unsecure		() const;
	u32									get_max_fat_size_impl		(fat_inline_data const & inline_data) const;
	void								get_associated_ptr_helper	(resources::managed_resource_ptr *	result_managed, 
																     resources::unmanaged_resource_ptr *	result_unmanaged,
																     resources::query_result * *			result_query_result);
	void								lock_associated		() const;
	void								unlock_associated	() const;


	typedef	typename pointer_for_fat<fat_node, pointer_for_fat_size>::type	pointer_to_node;

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
	friend								class				file_system::iterator;
	
	template <pointer_for_fat_size_enum pointer_for_fat_size1>
	friend								class				fat_folder_node;
	template <pointer_for_fat_size_enum pointer_for_fat_size1>
	friend								class				fat_disk_file_node;
	template <pointer_for_fat_size_enum pointer_for_fat_size1>
	friend								class				fat_db_file_node;
	template <pointer_for_fat_size_enum pointer_for_fat_size1>
	friend								class				fat_node;

	template <pointer_for_fat_size_enum target_pointer_size>
	friend								struct				save_nodes_helper;

}; // class fat_node

//-----------------------------------------------------------------------------------
// fat_folder_node
//-----------------------------------------------------------------------------------

template <pointer_for_fat_size_enum pointer_for_fat_size = pointer_for_fat_size_platform_default>
class fat_folder_node
{
public:
	typedef		fat_node<pointer_for_fat_size>		node_type;

public:
								fat_folder_node			() : m_first_child(NULL) {}

	void						transfer_children		(fat_folder_node * dest, bool no_duplicate_names);

	node_type *					get_first_child 		()		 { return m_first_child; }
	node_type const *			get_first_child 		() const { return m_first_child; }
	bool						find_child				(node_type const * seeking_child) const;
	node_type *					find_child				(pcstr name);
	node_type const *			find_child				(pcstr name) const 
														{ return const_cast<fat_folder_node *>(this)->find_child(name); }

	void						prepend_child			(node_type * child);
	void						unlink_child			(node_type * child);

	node_type *					cast_node				() { return & m_base; }
	node_type const *			cast_node				() const { return const_cast<fat_folder_node *>(this)->cast_node(); }

	void						reverse_bytes			();

public:
	typename pointer_for_fat<node_type, pointer_for_fat_size>::type	m_first_child;
	node_type					m_base;
}; // fat_folder_node

//-----------------------------------------------------------------------------------
// fat_node_lock
//-----------------------------------------------------------------------------------

class fat_node_lock
{
public:
			fat_node_lock		() : m_node_lock_count(0) { ; }
			~fat_node_lock		() { R_ASSERT(!m_node_lock_count); }
	void	set_node_is_locked	(bool lock);
private:
 	threading::atomic32_type	m_node_lock_count;	// node is locked during loading, until callback is called
};

//-----------------------------------------------------------------------------------
// fat_disk_folder_node
//-----------------------------------------------------------------------------------

template <pointer_for_fat_size_enum pointer_for_fat_size = pointer_for_fat_size_platform_default>
class fat_disk_folder_node : public fat_node_lock
{
public:
	fat_disk_folder_node	() : m_disk_path(NULL) {}

	fat_folder_node<pointer_for_fat_size> *		cast_folder_node	()	{ return & m_folder; }
	fat_node<pointer_for_fat_size> *			cast_node			()	{ return & m_folder.m_base; }

public:
	pstr										m_disk_path; // pointing to the end of the object, no need to free
	fat_folder_node<pointer_for_fat_size>		m_folder;
};

//-----------------------------------------------------------------------------------
// fat_db_file_node_base
//-----------------------------------------------------------------------------------

template <pointer_for_fat_size_enum pointer_for_fat_size = pointer_for_fat_size_platform_default>
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
	typename pointer_for_fat<resources::resource_base, pointer_for_fat_size>::type	m_resource_base;
	file_size_type										m_pos;
	u32													m_size;
	u32													m_hash;
};

//-----------------------------------------------------------------------------------
// fat_db_file_node
//-----------------------------------------------------------------------------------

template <pointer_for_fat_size_enum pointer_for_fat_size = pointer_for_fat_size_platform_default>
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

template <pointer_for_fat_size_enum pointer_for_fat_size = pointer_for_fat_size_platform_default>
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

template <pointer_for_fat_size_enum pointer_for_fat_size = pointer_for_fat_size_platform_default>
class fat_inline_base
{
public:
										fat_inline_base () : m_inlined_data(0), m_inlined_size(0) {}

	void								set_inlined_data(const_buffer const & buffer) 
	{ 
		m_inlined_data		=	(pcstr)buffer.c_ptr(); 
		m_inlined_size		=	(u32)buffer.size(); 
	}
	const_buffer						get_inlined_data() const;
	void								reverse_bytes	();

public:
	typename pointer_for_fat<char const, pointer_for_fat_size>::type	m_inlined_data;
	u32																	m_inlined_size;

}; // class fat_inline_base

//-----------------------------------------------------------------------------------
// fat_db_inline_file_node
//-----------------------------------------------------------------------------------

template <pointer_for_fat_size_enum pointer_for_fat_size = pointer_for_fat_size_platform_default>
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

template <pointer_for_fat_size_enum pointer_for_fat_size = pointer_for_fat_size_platform_default>
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

template <pointer_for_fat_size_enum pointer_for_fat_size = pointer_for_fat_size_platform_default>
class fat_disk_file_node : public fat_node_lock
{
public:
	fat_disk_file_node   									() : m_resource_base(NULL), m_disk_path(NULL), m_size(0) {}

	pcstr								disk_path			() const	{ return m_disk_path; }
	u32									size				() const	{ return (u32)m_size; }
	fat_node<pointer_for_fat_size> *	cast_node 			()			{ return & m_base; }

public:
	resources::resource_base *			m_resource_base;
	pstr								m_disk_path;	// pointing to the end of the object, no need to free
	threading::atomic32_type			m_size;
	fat_node<pointer_for_fat_size>		m_base;
}; // class fat_disk_node

namespace detail {
inline
int   bits_to_flags_byte		() { return	platform::big_endian() ? 16 : 0; }
} // namespace detail

void   set_fat_node_is_locked	(fat_node<> * node, bool lock);

} // namespace fs 
} // namespace xray 

#endif // FS_FILE_SYSTEM_NODES_IMPL_H_INCLUDED