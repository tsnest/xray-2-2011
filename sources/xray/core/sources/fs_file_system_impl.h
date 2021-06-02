////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_FILE_SYSTEM_IMPL_H_INCLUDED
#define FS_FILE_SYSTEM_IMPL_H_INCLUDED

#include <xray/hash_multiset.h>
#include <xray/intrusive_double_linked_list.h>
#include "fs_file_system_nodes_impl.h"
#include "fs_macros.h"
#include "fs_watcher.h"

namespace xray {
namespace fs   {

#	define USER_ALLOCATOR	::xray::memory::g_fs_allocator
#	include <xray/std_containers.h>
#	undef USER_ALLOCATOR

//-----------------------------------------------------------------------------------
// namespace detail
//-----------------------------------------------------------------------------------

namespace detail
{
	struct	helper_node;
	
	typedef hash_multiset< helper_node, xray::detail::fixed_size_policy<1024*32> > helper_hash_set;

	struct helper_node : public hash_multiset_intrusive_base<helper_node>
	{
		fat_node<> *		original_node;
		file_size_type		pos;
		u32					compressed_size;
		bool				is_compressed;
	};

} // namespace detail

//-----------------------------------------------------------------------------------
// file_system_impl
//-----------------------------------------------------------------------------------

class file_system_impl
{
private:
	typedef	file_system::db_callback			  db_callback;
	
public:
	struct mount_history
	{
		enum mount_type_enum						{ mount_type_db, mount_type_disk };
		path_string									logical_path;
		path_string									physical_path;
		mount_history *								next_mount_history;
		mount_history *								prev_mount_history;
		mount_type_enum								mount_type;
		file_system::watch_directory_bool			watch_directory;
		mount_history	(mount_type_enum mount_type, pcstr logical_path, pcstr physical_path, file_system::watch_directory_bool watch_directory) 
			: mount_type(mount_type), logical_path(logical_path), physical_path(physical_path), next_mount_history(NULL), watch_directory(watch_directory) {}
	};

	struct find_disk_mount_by_logical_path_predicate
	{
		find_disk_mount_by_logical_path_predicate	(buffer_string & file_logical_path) 
										: result(NULL), file_logical_path(&file_logical_path) {;}

		void operator () (mount_history * const history)
		{
			fs::path_string history_item_logical_path_with_slash	=	history->logical_path;
			if ( history_item_logical_path_with_slash.length() )
				history_item_logical_path_with_slash				+=	'/';

			if ( history->mount_type == mount_history::mount_type_disk && 
				 (file_logical_path->find(history_item_logical_path_with_slash.c_str()) == 0 ||
				  history->logical_path == * file_logical_path) )
			{
				R_ASSERT					(!result);
				result					=	history;
			}
		}

		mount_history *						result;
		buffer_string *						file_logical_path;
	}; // find_disk_mount_predicate

	struct find_disk_mount_by_physical_path_predicate
	{
		find_disk_mount_by_physical_path_predicate			(buffer_string & file_physical_path) : 
											result(NULL), file_physical_path(& file_physical_path) {;}

		void operator () (mount_history * const history)
		{
			fs::path_string history_physical_path_with_slash	=	history->physical_path;
			if ( history_physical_path_with_slash.length() )
				history_physical_path_with_slash				+=	'/';

			if ( history->mount_type == mount_history::mount_type_disk && 
				 (file_physical_path->find(history_physical_path_with_slash.c_str()) == 0 ||
				  * file_physical_path == history->physical_path) )
			{
				R_ASSERT					(!result);
				result					=	history;
			}
		}

		mount_history *						result;
		buffer_string *						file_physical_path;
	}; // find_disk_mount_predicate

	struct db_unmount_pred
	{
		db_unmount_pred(file_system_impl * pimpl, fat_node<> const * db_node_root) : pimpl(pimpl), db_node_root(db_node_root) {}
		bool   operator () (fat_node<> * work_node) const
		{
			if ( !work_node->is_db() )
				return false;

			db_record * db	=	pimpl->get_db(work_node);
			ASSERT( db);
	
			return db->root_node	==	db_node_root;
		}

		fat_node<> const *	db_node_root;
		file_system_impl *	pimpl;
	};

	struct history_deleter_predicate
	{
		void operator () (mount_history * mount_history)
		{
			FS_DELETE			(mount_history);
		}
	};

	typedef		intrusive_double_linked_list<	mount_history, 
												mount_history, 
												& mount_history::prev_mount_history, 
												& mount_history::next_mount_history	>	mount_history_container;
public:

						file_system_impl			();
					   ~file_system_impl			();

	void				mount_disk					(pcstr	logical_path, pcstr physical_path, file_system::watch_directory_bool);
	bool				mount_db					(pcstr	logical_path,
													 pcstr	fat_file_path,
													 pcstr	db_file_path,
													 bool	need_replication);
	
	bool				replicate_file				(fat_node<> * work_node, pcbyte data);
	
	void				commit_replication			(fat_node<> *	work_node);
	bool				do_replication				(pcstr			path, 
													 pcbyte			data, 
													 file_size_type	size, 
													 file_size_type	offs,
													 file_size_type	file_size);

	void				set_replication_dir			(pcstr replication_dir);

	enum				respect_mount_history_bool	{ respect_mount_history_false, respect_mount_history_true } ;

	void				unmount_disk				(pcstr logical_path, pcstr disk_dir_path, respect_mount_history_bool = respect_mount_history_true);
	void				unmount_db					(pcstr logical_path, pcstr fat_file_path);

	bool				unpack						(pcstr		 logical_path, 
													 pcstr		 dest_disk_dir, 
													 db_callback callback);

	bool				save_db						(FILE*		 						fat_file,
													 FILE*		 						db_file,
													 pcstr		 						logical_path, 
													 bool		 						no_duplicates,
													 u32		 						fat_alignment,
													 memory::base_allocator*			alloc,
													 compressor* 						compressor,
													 float		 						compress_smallest_rate,
													 file_system::db_target_platform_enum	db_format,
													 fat_inline_data &					inline_data,
													 db_callback 						callback);

	void				clear						();
	fat_node<> *		get_root					() const { return m_root->cast_node(); }
	fat_node<> *		find_node					(pcstr path, u32 * out_hash) const;
	void				get_disk_path				(fat_node<> const *	work_node, 
													 path_string &		out_path, 
													 bool				replicate = true) const;

	bool				equal_db					(fat_node<> const * it1, fat_node<> const * it2) const;

	bool				operator ==					(file_system_impl const & f) const;

	void				on_disk_file_change			(file_change_info const & info);
	bool				mount_disk_node				(pcstr logical_path, fat_node<> * * out_result_node);
	bool				get_disk_path_to_store_file		(pcstr				logical_path, 
														 buffer_string *	out_physical_path,
														 mount_history * *	out_opt_mount_history = NULL);

	bool				convert_physical_to_logical_path	(buffer_string * out_logical_path, pcstr physical_path, bool assert_on_fail = true);
	bool				update_file_size_in_fat		(pcstr in_logical_path, pcstr in_physical_path);
	void				rename_disk_node			(pcstr in_logical_path, pcstr in_renamed_old_path, pcstr in_renamed_new_path);
	void				rename_disk_node			(pcstr in_logical_path, pcstr in_renamed_old_path, pcstr in_renamed_new_path, u32 in_full_name_of_parent_hash);

	void				erase_disk_node				(pcstr physical_path);

private:
	void				on_mount					(mount_history::mount_type_enum type, pcstr logical_path, pcstr disk_path, file_system::watch_directory_bool);
	void				on_unmount					(mount_history::mount_type_enum type, pcstr logical_path, pcstr disk_path);
	
	enum				folder_watchers_action_enum	{ folder_watchers_action_remove, folder_watchers_action_add };
	void				action_for_folder_watchers	(folder_watchers_action_enum);

	void				add_folder_watchers			();
	void				remove_folder_watchers		();

	void				relink_hidden_nodes_to_parent (fat_node<> const * work_node, u32 hash, fat_node<> * parent);

	void				replicate_path				(pcstr path2replicate, path_string& out_path) const;

	void				fixup_db_node				(pstr flat_buffer, fat_node<> * work_node, 
													 u32 hash, bool reverse_byte_order);

	void				create_root					();

	struct				save_nodes_environment 
	{
						db_callback					callback;
						char *						dest_buffer;
						detail::helper_hash_set *	helper_set;
						detail::helper_node *		helper_nodes;
						FILE *						db_file;
						file_size_type			    dest_buffer_size;
					    u32							cur_offs;
						u32							node_index;
						u32							num_nodes;
						file_system::db_target_platform_enum	db_format;
						memory::base_allocator *	alloc;
						compressor *				compressor;
						float						compress_smallest_rate;
						fat_inline_data *			inline_data;
	};

	struct				db_header
	{
	private:
						char						endian_string[14];
	public:
						u32							num_nodes;
						u32							buffer_size;
						db_header () : num_nodes(0), buffer_size(0) { memory::zero(endian_string); }

						void set_big_endian		();
						void set_little_endian	();
						bool is_big_endian		();
						void reverse_bytes		();
	};

	bool				unpack_node					(save_nodes_environment & env, fat_node<> * cur_node, path_string& dest_dir);

	template <pointer_for_fat_size_enum pointer_for_fat_size>
	fat_node<pointer_for_fat_size> * save_nodes		(save_nodes_environment &	env, 
													 fat_node<> *				cur, 
													 file_size_type				prev_offs, 
													 file_size_type				parent_offs);

	void				save_node_file				(save_nodes_environment &	env, 
													 fat_node<> *				original_node,
													 file_size_type &			out_pos,
													 bool &						is_compressed,
													 mutable_buffer *			out_inlined_data,
													 u32 &						compressed_size,
													 u32 &						out_file_hash);

	void				free_node					(fat_node<> *, bool free_children);
	bool				mount_db_impl				(pcstr	logical_path,
													 pcstr	fat_file_path,
													 pcstr	db_file_path,
													 pcstr	replicated_fat_path,
													 pcstr	replicated_db_path,
													 bool	open_replicated,
													 bool	need_replication);

	void				mount_disk_folder			(fat_folder_node<> *	work_folder, 
													 pcstr					physical_path, 
													 u32					hash);

	bool				mount_disk_file				(fat_folder_node<> *	work_folder, 
													 pcstr					physical_path, 
													 u32					hash);

	template <class UnmountPredicate>
	void				unmount_node				(fat_node<> *		work_node,
													 u32				hash, 
													 const 
													 UnmountPredicate&	unmount_pred);

	fat_folder_node<> *	find_or_create_folder		(fat_folder_node<> *	parent, 
													 pcstr					name, 
													 u32					name_len, 
													 u32					hash);

	fat_folder_node<> *	find_or_create_folder		(pcstr path, u32 * out_hash);

	fat_folder_node<> *	create_folder_node			(u32				hash, 
													 pcstr				name, 
													 u32 const			name_len);

	fat_disk_folder_node<> *   create_disk_folder_node (u32				hash, 
														pcstr			name, 
														u32 const		name_len,
														pcstr			full_name, 
														u32 const		full_name_len);

	fat_disk_file_node<> *	create_disk_file_node	(u32						hash, 
													 pcstr						name, 
													 u32						name_len, 
													 pcstr						full_name,
													 u32						full_name_len,
													 u32						file_size,
													 memory::base_allocator *	alloc = & memory::g_fs_allocator,
													 bool						add_to_hash_table = true);

	void				destroy_temp_disk_node		(memory::base_allocator *	alloc,
													 fat_disk_file_node<> *		fat_node);

	bool				is_main_fat					() const;

	struct db_record
	{	
		path_string									db_path;
		path_string									fat_path;
		path_string									logical_path;
		char *										flat_buffer;
		file_size_type								flat_buffer_size;
		fat_node<> *								root_node;
		
		bool			contains					(fat_node<> const * work_node) const;
		void			destroy						();
	};

	struct find_db_by_ptr
	{
		find_db_by_ptr(fat_node<> const * db_node_start) : db_node_start(db_node_start) {}
		bool operator ()(db_record const & f)
		{
			return reinterpret_cast<size_t>(db_node_start) < reinterpret_cast<size_t>(f.root_node);
		}

		fat_node<> const* db_node_start;
	}; // struct find_db_by_ptr

	struct find_node_in_db
	{
		find_node_in_db		(fat_node<> const * work_node) : work_node(work_node) {}
		bool operator ()	(const db_record & db) const { return db.contains(work_node); }
		fat_node<> const *	work_node;
	};

	struct action_predicate
	{
		action_predicate(folder_watchers_action_enum const folder_watchers_action) : m_folder_watchers_action(folder_watchers_action) {}
		void operator () (mount_history * const history)
		{
			if ( history->mount_type == mount_history::mount_type_disk && 
				history->watch_directory == file_system::watch_directory_true )
			{
				if ( m_folder_watchers_action == folder_watchers_action_remove )
					quit_watching_physical_directory	(history->physical_path.c_str());
				else
					watch_physical_directory			(history->physical_path.c_str());
			}
		}

		folder_watchers_action_enum		m_folder_watchers_action;
	}; // remove_folder_watchers_predicate

	db_record *			get_db						(fat_node<> const * work_node);
	db_record const *	get_db						(fat_node<> const * work_node) const { return const_cast<file_system_impl *>(this)->get_db(work_node); }

	void				actualize_db				(db_record & db);
	void				actualize_node				(fat_node<> * work_node, u32 hash, fat_folder_node<> * parent);

	struct find_db_by_path
	{
		find_db_by_path(pcstr logical_path, pcstr fat_path) 
						: logical_path(logical_path), fat_path(fat_path) {}

		bool operator ()(const db_record& db) const
		{
			return db.logical_path == logical_path && db.fat_path == fat_path;
		}

		pcstr		logical_path;
		pcstr		fat_path;
	};

	void				set_on_resource_leaked_callback (on_resource_leaked_callback callback) { m_on_resource_leaked_callback	=	callback; }

	typedef				fixed_vector<db_record, 32>	db_data;

	db_data											m_db_data;
	fat_folder_node<> *								m_root;
	node_hash_set									m_hash_set;
	path_string										m_replication_dir;
	mount_history_container							m_mount_history;
	threading::mutex								m_mount_mutex;

	on_resource_leaked_callback						m_on_resource_leaked_callback;

	friend				class						iterator;
	friend				class						file_system;

}; // class fs_impl

} // namespace fs 
} // namespace xray 

#endif // FS_FILE_SYSTEM_IMPL_H_INCLUDED