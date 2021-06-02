////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_FILE_SYSTEM_H_INCLUDED
#define FS_FILE_SYSTEM_H_INCLUDED

#include <xray/fs_path.h>
#include <xray/fs_platform_configuration.h>
#include <xray/fs_sub_fat_resource.h>
#include <xray/resources_resource.h>
#include <xray/compressor.h>

namespace xray {

namespace resources 
{
	class 	resources_manager;
	class 	device_manager;
	class 	query_result;
	class 	resource_allocator;
	class 	managed_allocator;	
	class 	resource_base;
	class	fs_iterator;
} // namespace resources

namespace fs {

bool				initialize				( pcstr replication_folder );
void				finalize				();
void				initialize_watcher		();
void    			flush_replications		();
void XRAY_CORE_API	set_allocator_thread_id	(u32 thread_id);

typedef				boost::function< void ( resources::resource_base *	resource ) >
					on_resource_leaked_callback;
typedef				boost::function< void ( ) >
					on_unmount_started_callback;
 
void XRAY_CORE_API	set_on_resource_leaked_callback	(on_resource_leaked_callback);
void XRAY_CORE_API	set_on_unmount_started_callback	(on_unmount_started_callback);

//-----------------------------------------------------------------------------------
// fat_inline_data
//-----------------------------------------------------------------------------------

struct XRAY_CORE_API fat_inline_data
{
public:
	struct item
	{
							item	() : max_size(0), current_size(0), compression_rate(0), allow_compression_in_db(true) {}
		fixed_string<32>	extension;
		u32					max_size;
		u32					current_size;
		float				compression_rate; // 0 - no compression, 1 - compress all
		bool				allow_compression_in_db;
		enum				{ no_limit = u32(-1) };
	};

	typedef	buffer_vector<item>	container;

public:
	fat_inline_data (container * items) : m_items(items), m_highest_compression_rate(0) {}

	void	push_back				(item const & item);

	bool	find_by_extension 		(item * * out_item, pcstr extension);
	bool	find_by_extension 		(item const * * out_item, pcstr extension) const;

	void	begin_fitting			();

	bool	try_fit_for_inlining	(pcstr file_name, u32 file_size, u32 compressed_size);
	u32		total_size_for_extensions_with_limited_size () const;

	float	highest_compression_rate () const { return m_highest_compression_rate; }
	
private:
	float					m_highest_compression_rate;
	buffer_vector<item> *	m_items;
};

enum	find_enum				{	find_file_only								=	1 << 0,
									find_no_sub_fat_pin							=	1 << 1,
									find_sub_fat								=	1 << 2,
									find_recursively							=	1 << 3,
									find_sync									=	1 << 4,	};

//-----------------------------------------------------------------------------------
// file_system
//-----------------------------------------------------------------------------------

class XRAY_CORE_API file_system : public core::noncopyable {
public:
	// Warning: taking upper bytes for flags
	enum {	is_folder				=	1 << 15, 
			is_disk					=	1 << 14, 
			is_db					=	1 << 13,
			is_replicated			=	1 << 12,	
			is_compressed			=	1 << 11, 
			is_inlined				=	1 << 10,	
			is_sub_fat				=	1 << 9,	
			is_hard_link			=	1 << 8,
			is_soft_link			=	1 << 7,
			is_locked				=	1 << 00,	};	// special value flags = 1 OK!

	enum	db_target_platform_enum		{ db_target_platform_unset, db_target_platform_pc, db_target_platform_xbox360, db_target_platform_ps3, };
	static  bool   is_big_endian_format	(db_target_platform_enum const db_format) { return db_format == db_target_platform_xbox360 || db_format == db_target_platform_ps3; };

public:
	typedef	fastdelegate::FastDelegate< void (const u32 num_saved, u32 num_whole, pcstr name, u32 flags) >::BaseType			db_callback;
	
	enum				watch_directory_bool	{ 	watch_directory_false, watch_directory_true	};
	enum				recursive_bool		{ 	recursive_false, recursive_true	};

public:
						file_system			();
					   ~file_system			();

	void				mount_disk			(pcstr logical_path, 
											 pcstr disk_dir_path, 
											 watch_directory_bool watch_directory = watch_directory_true,
											 recursive_bool is_recursive = recursive_false);

	resources::fs_iterator	continue_mount_disk	(pcstr fat_dir, recursive_bool);
	
	bool				mount_db			(pcstr logical_path, 
											 pcstr fat_file_path, 
											 pcstr db_file_path,
											 bool  need_replication);

	void				unmount_disk		(pcstr logical_path, pcstr disk_dir_path);
	void				unmount_db			(pcstr logical_path, pcstr disk_path);

	bool				save_db				(pcstr 		 				fat_file_path, 
											 pcstr 		 				db_file_path,
											 bool		 				no_duplicates,
											 u32		 				fat_alignment,
											 memory::base_allocator *	alloc,
											 compressor * 				compressor,
											 float		 				compress_smallest_rate,
											 db_target_platform_enum	db_format,
											 fat_inline_data &			inline_data,
											 u32						fat_part_max_size,
											 db_callback 				callback);

	bool				unpack				(pcstr fat_dir, pcstr dest_dir, db_callback);

	void				set_replication_dir (pcstr replication_dir);

	void				commit_replication	(fat_node<> * work_node);

	void				replicate_path		(pcstr path2replicate, path_string& out_path) const;
	void				get_disk_path		(fat_node<> * node, path_string & out_path) const;
	bool				equal_db			(fat_node<> * left, fat_node<> * right) const;

	bool				replicate_file		(fat_node<> * node, pcbyte data);

	bool				operator ==			(file_system const & f) const;

	bool				try_find_sync			(resources::sub_fat_pin_fs_iterator *	out_pin_iterator,													 
												 pcstr const							path_to_find,
												 enum_flags<find_enum>					find_flags = 0);

	bool				try_find_sync_no_pin	(resources::fs_iterator *				out_iterator,
												 find_results_struct *					find_results,
												 pcstr const							path_to_find,
												 enum_flags<find_enum>					find_flags = 0);

	void				find_async				(find_results_struct *					find_results,
									 			 pcstr									path_to_find,
									 			 enum_flags<find_enum>					find_flags = 0);
	
 	fat_node<> *		create_temp_disk_node				(memory::base_allocator * alloc, pcstr disk_path);
	void				destroy_temp_disk_node				(memory::base_allocator * alloc, fat_node<> * it);
	signalling_bool		get_disk_path_to_store_file			(pcstr logical_path, buffer_string * out_disk_path, mount_filter const & filter = NULL);
	bool				mount_disk_node_by_logical_path		(pcstr logical_path, resources::fs_iterator * out_iterator = NULL, bool assert_on_fail = true);
	bool				mount_disk_node_by_physical_path	(pcstr physical_path, resources::fs_iterator * out_iterator = NULL);
	bool				convert_physical_to_logical_path	(path_string * out_logical_path, pcstr physical_path);

	void				unmount_disk_node					(pcstr physical_path);
	void				mark_disk_node_as_erased			(pcstr physical_path);

	bool				update_file_size_in_fat				(pcstr physical_path, bool do_resource_deassociation);
	bool				rename_disk_node					(pcstr old_physical_path, pcstr new_physical_path, bool do_resource_deassociation);

	void				set_on_resource_leaked_callback		(on_resource_leaked_callback);
	void				set_on_unmount_started_callback		(on_unmount_started_callback);
	void				clear				();

	file_system_impl *	impl				() const { return m_impl; }
private:

	file_system_impl *	m_impl;

	template <platform_pointer_enum pointer_for_fat_size>
	friend class		fat_node;
	friend class		::xray::fs::file_system_impl;
	friend class		::xray::resources::fs_iterator;
	friend void			initialize_watcher ();
};

//-----------------------------------------------------------------------------------
// file_system global object
//-----------------------------------------------------------------------------------

extern xray::uninitialized_reference<file_system>	g_fat;

} // namespace fs
} // namespace xray

#endif // FS_FILE_SYSTEM_H_INCLUDED