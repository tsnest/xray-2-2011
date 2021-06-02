////////////////////////////////////////////////////////////////////////////
//	Created 	: 08.10.2008
//	Author		: Sergey Chechin, Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_COOK_H_INCLUDED
#define XRAY_RESOURCES_COOK_H_INCLUDED

#include <xray/resources_classes.h>
#include <xray/fs/virtual_path_string.h>
#include <xray/fs/synchronous_device_interface.h>

namespace xray {
namespace resources {

class unmanaged_resource;
class query_result_for_cook;
class unmanaged_cook;
class managed_cook;
class inplace_unmanaged_cook;
class inplace_managed_cook;	
class translate_query_cook;	

class XRAY_CORE_API cook_base
{
public:
	enum flags_enum					{ 	flag_destroy_in_any_thread					=	1 << 0,	
										flag_create_allocates_destroy_deallocates	=	1 << 1,		
										flag_no_deallocate							=	1 << 2,		
										flag_last									=	1 << 3, };
public:
	static u32  const					thread_id_unset					=	(u32)-1;
	static u32	const 					use_cook_thread_id				=	(u32)-2;
	static u32	const 					use_current_thread_id			=	(u32)-3;
	static u32	const 					use_resource_manager_thread_id	=	(u32)-4;
	static u32	const 					use_any_thread_id				=	(u32)-5;

	enum reuse_enum					{	reuse_false, reuse_true, reuse_raw	};

	enum result_enum				{	result_undefined,	// do not return this from cook
										result_error, 
										result_postponed,
										result_success, 
										result_requery,
										result_out_of_memory	};

protected:
	enum internal_flags_enum		{	internal_flag_does_translate_query		=	flag_last << 0,
										internal_flag_does_inplace_cook			=	flag_last << 1,
										internal_flag_creates_managed_resource	=	flag_last << 2,
									};

public:
			cook_base					(class_id_enum				resource_class, 
										 reuse_enum					reuse_type,
										 enum_flags<flags_enum>		flags,
										 u32 						creation_thread_id,
										 u32						allocate_thread_id);

	virtual	void						translate_request_path		(pcstr request, fs_new::virtual_path_string & new_request) const { new_request = request; }

	virtual u32							calculate_quality_levels_count	(query_result_for_cook const *) { NOT_IMPLEMENTED(return 1); }

	virtual float						satisfaction_with			(u32 quality_level, math::float4x4 const & user_matrix, u32 users_count) { XRAY_UNREFERENCED_PARAMETERS(quality_level, &user_matrix, users_count); NOT_IMPLEMENTED(return 1); }
	virtual bool						cache_by_game_resources_manager () { return true; }

			class_id_enum				get_class_id				() const;
			bool						has_flags					(flags_enum	flag) const { return m_flags.has(flag); }

			fs_new::synchronous_device_interface &	get_synchronous_device	() const;

			unmanaged_cook *			cast_unmanaged_cook			();
			managed_cook *				cast_managed_cook			();
			inplace_unmanaged_cook *	cast_inplace_unmanaged_cook	();
			inplace_managed_cook *		cast_inplace_managed_cook	();
			translate_query_cook *		cast_translate_query_cook	();
	
	static	cook_base *					find_cook					(class_id_enum);
	static	unmanaged_cook *			find_unmanaged_cook			(class_id_enum);
	static	managed_cook *				find_managed_cook			(class_id_enum);
	static	inplace_unmanaged_cook *	find_inplace_unmanaged_cook	(class_id_enum);
	static	inplace_managed_cook *		find_inplace_managed_cook	(class_id_enum);
	static	translate_query_cook *		find_translate_query_cook	(class_id_enum);

			bool						cooks_managed_resource		() const;
			bool						cooks_unmanaged_resource	() const { return !cooks_managed_resource(); }

	static	bool						cooks_managed_resource		(class_id_enum class_id);
	static	bool						cooks_unmanaged_resource	(class_id_enum class_id) { return !cooks_managed_resource(class_id); }

			bool						cooks_inplace				() const;
	static	bool						cooks_inplace				(class_id_enum class_id);

			bool						does_create_resource				() const;
	static	bool						does_create_resource				(class_id_enum);

			bool						does_create_resource_if_no_file		();
	static	bool						does_create_resource_if_no_file		(class_id_enum);

			bool						destroy_in_any_thread		() const;
	static	bool						destroy_in_any_thread		(class_id_enum);

			bool						does_deallocate				() const;
	
			reuse_enum					reuse_type					() const;
	static	reuse_enum					reuse_type					(class_id_enum);

			u32							cook_users_count			() const;

	virtual bool						allow_sync_load_from_inline	() const { return true; }
	static  bool						allow_sync_load_from_inline	(class_id_enum);
			void						call_destroy_resource		(unmanaged_resource * resource);
			void						call_destroy_resource		(managed_resource * resource);
	virtual void						deallocate_resource			(pvoid buffer) = 0;

			void						to_string					(buffer_string * out_string);
			u32							allocate_thread_id			();
	static	u32							allocate_thread_id			(class_id_enum);
			u32							translate_thread_id			() { return allocate_thread_id(); }
	static	u32							translate_thread_id			(class_id_enum id) { return allocate_thread_id(id); }
			u32							creation_thread_id			();

			void						register_object_to_delete	(unmanaged_resource * resource, u32 deallocation_thread_id);
			void						update_class_id				(managed_resource_ptr resource, class_id_enum class_id) { resource->set_class_id(class_id); }

	virtual								~cook_base					() {}

protected:
			void						set_flags					(flags_enum flags) { m_flags.set(flags); }
			bool						has_flags					(internal_flags_enum flags) const { return m_flags.has(flags); }

			template <class T>
			pinned_ptr_mutable<T>		pin_for_write				(managed_resource_ptr resource) { return pinned_ptr_mutable<T>(resource); }
private:
			cook_base *					get_next					() const		{ return m_next; }
			void						set_next					(cook_base * next)	{ m_next = next; }

			void						change_cook_users_count		(int change)	{ m_cook_users_count.change_cook_users_count(change);	}

private:
	class XRAY_CORE_API cook_users_count_functionality
	{
	public:
										cook_users_count_functionality	() : m_count(0) {}
		u32								cook_users_count				() const	{ return m_count; }
		void							change_cook_users_count			(int change);
	private:
		threading::atomic32_type		m_count;
	};

private:
	cook_users_count_functionality		m_cook_users_count;

	class_id_enum						m_class_id;
	reuse_enum							m_reuse_type;
	threading::thread_id_type			m_creation_thread_id;
	union {
	threading::thread_id_type			m_allocate_thread_id;
	threading::thread_id_type			m_translate_thread_id;
	};
	flags_type<flags_enum>				m_flags;

	cook_base *							m_next;

	friend class						resources_manager;
	friend class						unmanaged_resource;
	friend class						query_result;

}; // cook_base

XRAY_CORE_API void					register_cook			(cook_base *	processor);
XRAY_CORE_API cook_base *			unregister_cook			(class_id_enum	resource_class);

pcstr   convert_cook_reuse_value_to_string					(cook_base::reuse_enum value);

} // namespace xray
} // namespace resources

#endif // #ifndef XRAY_RESOURCES_POST_PROCESSOR_H_INCLUDED