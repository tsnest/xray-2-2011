////////////////////////////////////////////////////////////////////////////
//	Created		: 28.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_UNMANAGED_RESOURCE_H_INCLUDED
#define XRAY_RESOURCES_UNMANAGED_RESOURCE_H_INCLUDED

#include <xray/resources_resource_base.h>
#include <xray/resources_intrusive_base.h>
#include <xray/resources_managed_resource.h>
#include <xray/resources_resource_ptr.h>

namespace xray {
namespace resources {

class cook_base;

class XRAY_CORE_API unmanaged_resource :	public resource_base, 
											public unmanaged_intrusive_base
{
public:
							unmanaged_resource		(u32 quality_levels_count = 1);
	virtual				   ~unmanaged_resource 		();

	void					set_creation_source		(creation_source_enum creation_source, pcstr request_path, memory_usage_type const & memory_usage);
	managed_resource_ptr	raw_resource_ptr		() const { return m_raw_resource_ptr; }

	void					set_deleter_object		(cook_base * cook, u32 deallocation_thread_id);
	bool					has_deleter_object		() const { return !!m_deleter; }

	u32						deallocate_thread_id	() const { R_ASSERT(m_deallocation_thread_id); return m_deallocation_thread_id; }
	void					set_no_delete			() { m_flags.set(flag_no_delete); }

	void					set_sub_fat_resource	(vfs_sub_fat_resource * sub_fat);
	vfs_sub_fat_resource *	get_sub_fat				() const { return m_sub_fat.c_ptr(); }

#ifdef MASTER_GOLD
	pcstr					request_path			() const { return "<unknown>"; }
#else
	pcstr					request_path			() const { return m_request_path.c_str(); }
	fs::path_string			m_request_path;
#endif
	
protected:
							unmanaged_resource		(resource_flags_enum flags, u32 quality_levels_count);

private:
	void					constructor_impl		();
	unmanaged_resource *	get_next_delay_delete	() const { return m_next_delay_delete; }
	cook_base *				get_deleter_object		() const { return m_deleter; }
	void					late_set_fat_it			(vfs::vfs_iterator it);
	virtual log_string_type	log_string				() const;

	void					set_as_inlined_in_fat	();
	bool					is_inlined_in_fat		() const { return m_inlined_in_fat; }
	void					unset_as_inlined_in_fat	();

	void					set_delay_delete		() { m_flags.set(flag_delay_delete); }
	bool					is_delay_delete			() const { return m_flags.has(flag_delay_delete); }
	bool					is_no_delete			() const { return m_flags.has(flag_no_delete); }

private:
	enum	flag_enum		{	flag_delay_delete	=	1 << 0,	
								flag_no_delete		=	1 << 1, };

private:
	vfs_sub_fat_child_resource_ptr	m_sub_fat;
	
	cook_base *						m_deleter;
	managed_resource_ptr			m_raw_resource_ptr;
	union {
	unmanaged_resource *			m_next_delay_delete;
	unmanaged_resource *			m_next_delay_deallocate;
	};
	unmanaged_resource *			m_next_in_global_list;
	unmanaged_resource *			m_prev_in_global_list;
	unmanaged_resource *			m_next_in_global_delay_delete_list;
	unmanaged_resource *			m_prev_in_global_delay_delete_list;
	u32								m_deallocation_thread_id;
	flags_type<flag_enum>			m_flags;

	bool							m_inlined_in_fat;

	friend class					resources_manager;
	friend class					thread_local_data;
	friend class					unmanaged_intrusive_base;
	friend class					base_of_intrusive_base;
	friend class					resource_base;
	friend class					query_result;
	friend class					query_result_for_cook;
	friend class					queries_result;
	friend class					game_resources_manager;
	friend class					cook_base;
	friend class					debug_state;
};

class unmanaged_resource_buffer
{
public:
									unmanaged_resource_buffer	(resource_base::creation_source_enum creation_source, pcstr request_name, memory_usage_type memory_usage, class_id_enum class_id, query_result * destruction_observer, bool is_delay_delete, bool is_delay_deallocate);

	resources::class_id_enum		get_class_id					() const { return m_class_id; }
	memory_usage_type				get_memory_usage				() const { return m_memory_usage; }
	bool							is_delay_deallocate				() const { return m_is_delay_deallocate; }
	bool							is_delay_delete					() const { return m_is_delay_delete; }
	
#ifdef MASTER_GOLD
	pcstr							request_name					() const { return ""; }
#else // #ifdef MASTER_GOLD
	pcstr							request_name					() const { return m_request_name.c_str(); }
#endif
	
	query_result *					get_destruction_observer		() const { return m_destruction_observer; }

public:
	unmanaged_resource_buffer *		m_next_to_deallocate;
	unmanaged_resource_buffer *		m_next_in_global_delay_delete_list;
	unmanaged_resource_buffer *		m_prev_in_global_delay_delete_list;

private:
#ifndef MASTER_GOLD
	fs_new::virtual_path_string		m_request_name;
#endif
	resource_base::creation_source_enum	m_creation_source;

	memory_usage_type				m_memory_usage;
	resources::class_id_enum		m_class_id;
	bool							m_is_delay_deallocate;
	bool							m_is_delay_delete;
	query_result *					m_destruction_observer;
};

class XRAY_CORE_API positional_unmanaged_resource : public unmanaged_resource
{
public:
	positional_unmanaged_resource(u32 quality_levels_count = 1) : unmanaged_resource(resource_flag_is_positional_unmanaged, quality_levels_count) 
	{
		matrix[0] = matrix[1] = NULL;
	}

	math::float4x4 const &	current_matrix				( ) const { return matrix_storage[0]; }
protected:
	math::float4x4 &		current_matrix_for_write	( ) { return matrix_storage[0]; }
private:
	math::float4x4					matrix_storage[2];
	math::float4x4 *				matrix[2];
};

typedef	resource_ptr		<unmanaged_resource, unmanaged_intrusive_base>	unmanaged_resource_ptr;
typedef	child_resource_ptr	<unmanaged_resource, unmanaged_intrusive_base>	child_unmanaged_resource_ptr;

namespace resources_detail {
	typedef resources::query_callback	query_callback;

	#pragma warning( push )
	#pragma warning( disable : 4231 )
	template class XRAY_CORE_API xray::resources::child_resource_ptr <	xray::resources::unmanaged_resource, 	
																		xray::resources::unmanaged_intrusive_base	>;	

	template class XRAY_CORE_API xray::resources::resource_ptr < xray::resources::unmanaged_resource, 	
																 xray::resources::unmanaged_intrusive_base>;	

	template class XRAY_CORE_API xray::intrusive_ptr< xray::resources::unmanaged_resource, 	
													  xray::resources::unmanaged_intrusive_base,
													  xray::threading::simple_lock>;	
		
	template class XRAY_CORE_API boost::function< void ( xray::resources::queries_result& data ) >;

	#pragma warning( pop )
}

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_UNMANAGED_RESOURCE_H_INCLUDED
