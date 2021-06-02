////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_FILE_SYSTEM_IMPL_H_INCLUDED
#define FS_FILE_SYSTEM_IMPL_H_INCLUDED

#include <xray/hash_multiset.h>
#include <xray/intrusive_double_linked_list.h>
#include <xray/fs_sub_fat_resource.h>

#include "fs_impl_saving_environment.h"
#include "fs_impl_finding_environment.h"
#include "fs_macros.h"
#include "fs_watcher_internal.h"

namespace xray {
namespace fs   {

#	define USER_ALLOCATOR	::xray::memory::g_fs_allocator
#	include <xray/std_containers.h>
#	include <xray/unique_ptr.h>
#	undef USER_ALLOCATOR

//-----------------------------------------------------------------------------------
// file_system_impl
//-----------------------------------------------------------------------------------

class file_system_impl
{
private:
	typedef	file_system::db_callback					db_callback;
	typedef file_system::watch_directory_bool			watch_directory_bool;
	typedef file_system::recursive_bool				recursive_bool;

public:
	struct mount_history
	{
		enum mount_type_enum						{ mount_type_db, mount_type_disk };
		path_string									logical_path;
		path_string									physical_path;
		mount_history *								next_mount_history;
		mount_history *								prev_mount_history;
		mount_type_enum								mount_type;
		watch_directory_bool			watch_directory;
		mount_history	(mount_type_enum mount_type, pcstr logical_path, pcstr physical_path, watch_directory_bool watch_directory) 
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
											best_history_item(NULL), file_physical_path(& file_physical_path) {;}

		void operator () (mount_history * const history)
		{
			fs::path_string history_physical_path_with_slash	=	history->physical_path;
			if ( history_physical_path_with_slash.length() )
				history_physical_path_with_slash				+=	'/';

			if ( history->mount_type == mount_history::mount_type_disk && 
				 (file_physical_path->find(history_physical_path_with_slash.c_str()) == 0 ||
				  * file_physical_path == history->physical_path) )
			{
				if ( !best_history_item || history->physical_path.length() > best_history_item->physical_path.length() )
				{
					best_history_item	=	history;
				}
			}
		}

		mount_history *						best_history_item;
		buffer_string *						file_physical_path;
	}; // find_disk_mount_predicate

	struct db_unmount_pred
	{
		db_unmount_pred(file_system_impl * pimpl, fat_node<> const * db_node_root) : pimpl(pimpl), db_node_root(db_node_root) {}
		bool   operator () (fat_node<> * work_node) const
		{
			if ( !work_node->is_db() )
				return false;

			sub_fat_resource * db	=	pimpl->get_db(work_node);
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
												mount_history *, 
												& mount_history::prev_mount_history, 
												& mount_history::next_mount_history	>	mount_history_container;
public:

						file_system_impl			();
					   ~file_system_impl			();

	void				mount_disk					(pcstr logical_path, 
													 pcstr physical_path, 
													 watch_directory_bool,
													 recursive_bool recursive);

	bool				mount_db					(pcstr	logical_path,
													 pcstr	fat_file_path,
													 pcstr	db_file_path,
													 bool	need_replication,
													 bool	store_in_mount_history		=	true,
													 bool	is_automatically_mounted	=	false);
	
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
	void				unmount_root_db				(sub_fat_resource * root_db);
	void				unmount_db					(pcstr logical_path, pcstr disk_path, bool store_in_mount_history = true);
	void					unmount_sub_dbs			(sub_fat_resource * sub_fat, bool store_in_mount_history);

	bool				unpack						(pcstr		 			logical_path, 
													 pcstr		 			dest_disk_dir, 
													 db_callback 			callback);

	bool				save_db						(saving_environment &	env, 
													 fat_node_info *	info_tree);

	void				clear						();
	fat_node<> *		get_root					() const { return m_root->cast_node(); }


	bool				try_find_sync				(resources::sub_fat_pin_fs_iterator *	out_pin_iterator,													 
													 pcstr									path_to_find,
													 enum_flags<find_enum>					find_flags);

	bool				try_find_sync_no_pin		(resources::fs_iterator *		out_iterator,
													 find_results_struct *			find_results,
													 pcstr							path_to_find,
													 enum_flags<find_enum>			find_flags);

	void				find_async					(find_results_struct *			find_results,
													 pcstr							path_to_find,
													 enum_flags<find_enum>			find_flags);

	void				find_async_no_pin			(find_results_struct *			find_results,
													 pcstr							path_to_find,
													 enum_flags<find_enum>			find_flags);
	void					find_async_across_mount_disk	(find_env &				env);

 	void					find_async_query_sub_fat(find_env &						env);
 	void						on_sub_fat_loaded	(resources::queries_result &	results,
 													 pcstr							path_to_find,
 													 enum_flags<find_enum>			find_flags,
 													 find_results_struct *			find_results);

	void				find_async_pin				(find_results_struct *			find_results,
									 				 pcstr							path_to_find,
									 				 enum_flags<find_enum>			find_flags);

	fat_node<> *		find_node_no_mount			(pcstr path, u32 * out_hash = NULL) const;

	void				get_disk_path				(fat_node<> const *	work_node, 
													 path_string &		out_path, 
													 bool				replicate = true) const;

	bool				equal_db					(fat_node<> const * it1, fat_node<> const * it2) const;

	bool				operator ==					(file_system_impl const & f) const;

	void				on_disk_file_change			(file_change_info const & info);
	bool				mount_disk_node				(pcstr logical_path, pcstr physical_path, resources::fs_iterator * out_result_itbool, bool assert_on_fail = true);

	bool				get_disk_path_to_store_file		(pcstr					logical_path, 
														 buffer_string *		out_physical_path,
														 mount_history * *		out_opt_mount_history = NULL,
														 bool					assert_on_fail = true,
														 mount_filter const &	filter = NULL);

	path_info::type_enum   get_physical_path_info_by_logical_path (path_info * out_path_info, pcstr logical_path);

	bool				convert_physical_to_logical_path	(buffer_string * out_logical_path, pcstr physical_path, bool assert_on_fail = true);
	bool				update_file_size_in_fat		(pcstr in_logical_path, pcstr in_physical_path, bool do_resource_deassociation);
	bool				update_file_size_impl		(pcstr in_physical_path, fat_node<> * node, bool do_resource_deassociation);
	void				rename_disk_node			(pcstr in_logical_path, bool do_resource_deassociation, pcstr in_renamed_old_path, pcstr in_renamed_new_path);
	void				rename_disk_node			(pcstr in_logical_path, bool do_resource_deassociation, pcstr in_renamed_old_path, pcstr in_renamed_new_path, u32 in_full_name_of_parent_hash);

	void				unmount_disk_node			(pcstr physical_path);
	void				mark_disk_node_as_erased	(pcstr physical_path);
	sub_fat_resource *	get_db						(fat_node<> const * work_node);
	void				mount_sub_fat_resource		(sub_fat_resource_ptr new_db);
	
	fat_node<> *		continue_mount_disk			(fat_node<> * const opt_node, pcstr const opt_logical_path, recursive_bool);
	fat_node<> *			continue_mount_disk_db					(fat_disk_file_node<> * disk_file, pcstr logical_path);
	void					continue_mount_disk_folder_recursively	(fat_disk_folder_node<> * work_folder, u32 hash);

private:
	void				on_mount					(mount_history::mount_type_enum type, pcstr logical_path, pcstr disk_path, watch_directory_bool);
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

	bool				unpack_node					(saving_environment & env, fat_node<> * cur_node, path_string& dest_dir);

	void				free_node					(fat_node<> *, bool free_children);
	bool				mount_db_impl				(pcstr	logical_path,
													 pcstr	fat_file_path,
													 pcstr	db_file_path,
													 pcstr	replicated_fat_path,
													 pcstr	replicated_db_path,
													 bool	open_replicated,
													 bool	need_replication,
													 bool	is_automatically_mounted);

	sub_fat_resource *  find_sub_fat_parent			(sub_fat_resource *		sub_fat);
	void				mount_sub_fat_resource_impl	(sub_fat_resource *		new_db);

	void				mount_disk_folder			(fat_folder_node<> *	work_folder, 
													 pcstr					physical_path, 
													 u32					hash,
													 recursive_bool);
	void					mount_overlapped		(fat_folder_node<> *	work_folder, 
										   			 pcstr 					physical_path, 
													 u32					hash,
													 recursive_bool		recursive);
	void					mount_disk_folder_impl	(fat_folder_node<> *	work_folder, 
													 pcstr					physical_path, 
													 u32					hash,
													 recursive_bool);
	bool					physical_node_mounted	(u32					hash,
													 pcstr					physical_path);


	bool				mount_disk_file				(fat_folder_node<> *	work_folder, 
													 pcstr					physical_path, 
													 u32					hash);

	template <class UnmountPredicate>
	void				unmount_node				(fat_node<> *		work_node,
													 u32				hash, 
													 const 
													 UnmountPredicate&	unmount_pred);
	void					unmount_automatic_db	(fat_node<> *		freeing_node, 
													 u32				hash, 
													 fat_disk_file_node<> * disk_file);

	void					deassociate_last_link_if_needed (fat_node<> * work_node);

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

	intrusive_list<sub_fat_resource, sub_fat_resource_ptr, & sub_fat_resource::next, threading::mutex>	
		m_root_dbs;

	sub_fat_resource_tree_type	m_dbs_tree;

	struct find_node_in_db
	{
		find_node_in_db		(fat_node<> const * work_node) : work_node(work_node) {}
		bool operator ()	(sub_fat_resource * db) const { return db->contains(work_node); }
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

	sub_fat_resource const *	get_db				(fat_node<> const * work_node) const { return const_cast<file_system_impl *>(this)->get_db(work_node); }
	sub_fat_resource *			get_db				(pcstr logical_path, pcstr disk_path);

	void				actualize_db				(sub_fat_resource * db);
	void				actualize_node				(fat_node<> * work_node, u32 hash, fat_folder_node<> * parent);

	struct find_by_db_path
	{
						find_by_db_path	(pcstr archive_physical_path, pcstr virtual_path) 
							: archive_physical_path(archive_physical_path), virtual_path(virtual_path) { ; }

		bool			operator ()		(sub_fat_resource const & db) const { return (* this) (& db); }
		bool			operator ()		(sub_fat_resource const * db) const 
		{ 
			if ( virtual_path && db->virtual_path != virtual_path )
				return					false;

			return		(db->archive_physical_path == archive_physical_path); 
		}
		pcstr			archive_physical_path;
		pcstr			virtual_path;
	};

	void				set_on_resource_leaked_callback (on_resource_leaked_callback callback) { m_on_resource_leaked_callback	=	callback; }
	void				set_on_unmount_started_callback	(on_unmount_started_callback callback) { m_on_unmount_started_callback	=	callback; }

	fat_folder_node<> *								m_root;
	node_hash_set									m_hash_set;
	path_string										m_replication_dir;
	mount_history_container							m_mount_history;
	threading::mutex								m_mount_mutex;

	on_resource_leaked_callback						m_on_resource_leaked_callback;
	on_unmount_started_callback						m_on_unmount_started_callback;

	friend				class						file_system_iterator;
	friend				class						iterator;
	friend				class						file_system;
	template <platform_pointer_enum pointer_for_fat_size>
	friend				class						save_db_impl;
	
	template <platform_pointer_enum pointer_for_fat_size>
	friend				class						fat_node;

}; // class fs_impl

struct disk_unmount_pred
{	
	disk_unmount_pred(pcstr disk_dir) : disk_dir(disk_dir) {}

	bool   operator () (fat_node<> * work_node) const	
	{		
		if ( !work_node->is_disk() )			
			return					false;		
		
		pcstr node_disk_dir			=	work_node->is_folder() ? work_node->cast_disk_folder()->m_disk_path : 	
																 work_node->cast_disk_file()->m_disk_path;		
		return strstr(node_disk_dir, disk_dir) == node_disk_dir;	
	}	

	pcstr disk_dir;
};

} // namespace fs 
} // namespace xray 

#endif // FS_FILE_SYSTEM_IMPL_H_INCLUDED