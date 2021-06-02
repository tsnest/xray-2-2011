////////////////////////////////////////////////////////////////////////////
//	Created 	: 08.10.2008
//	Author		: Sergey Chechin, Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_COOK_CLASSES_H_INCLUDED
#define XRAY_RESOURCES_COOK_CLASSES_H_INCLUDED

#include <xray/resources_cook_base.h>
#include <xray/fs_path_string.h>
#include <xray/resources_unmanaged_allocator.h>

namespace xray {
namespace resources {

class XRAY_CORE_API managed_cook : public cook_base
{
public:
		typedef fastdelegate::FastDelegate< void	(resources::query_result_for_cook &	in_out_query, 
												 	 resources::managed_resource_ptr 	out_resource) >
												 	 create_resource_if_no_file_delegate_type;
public:
		managed_cook			(class_id_enum	resource_class,
								 reuse_enum		reuse_type,
								 u32 			creation_thread_id	=	use_cook_thread_id,
								 enum_flags<flags_enum> flags	=	0);
	
		virtual create_resource_if_no_file_delegate_type	get_create_resource_if_no_file_delegate	() { return NULL; }

		virtual	void	translate_request_path	(pcstr request, fs_new::virtual_path_string & new_request) const { new_request = request; }

		// raw_file_data can be NULL, when no file is found
		virtual	u32		calculate_resource_size	(const_buffer in_raw_file_data, bool file_exist) = 0;

		virtual void	create_resource			(resources::query_result_for_cook &	in_out_query, 
												 const_buffer						raw_file_data,
												 resources::managed_resource_ptr	out_resource) = 0;
		virtual	void	destroy_resource		(resources::managed_resource *		dying_resource) = 0;

private: // are not used in this cook:
		virtual void	deallocate_resource		(pvoid) { FATAL("unexpected"); }
}; // managed_cook

class XRAY_CORE_API inplace_managed_cook : public cook_base
{
public:
		typedef fastdelegate::FastDelegate< void	(resources::query_result_for_cook &	in_out_query, 
													 resources::managed_resource_ptr 			out_resource,
													 u32 &								out_final_resource_size) >
													 create_resource_if_no_file_delegate_type;
public:
		inplace_managed_cook	(class_id_enum	resource_class,
								 reuse_enum		reuse_type,
								 u32 			creation_thread_id	=	use_cook_thread_id, 
								 enum_flags<flags_enum> flags				=	0);

		virtual create_resource_if_no_file_delegate_type	get_create_resource_if_no_file_delegate	() { return NULL; }

		virtual	void	translate_request_path	(pcstr request, fs_new::virtual_path_string & new_request) const { new_request = request; }

		virtual	u32		calculate_resource_size	(u32 file_size, u32 & out_offset_to_file, bool file_exist) { R_ASSERT_U(file_exist); out_offset_to_file = 0; return file_size; }

		virtual void	create_resource			(resources::query_result_for_cook &	in_out_query,
												 resources::managed_resource_ptr	in_out_resource,
												 u32								raw_file_size,
												 u32 &								out_final_resource_size) = 0;

		virtual	void	destroy_resource		(resources::managed_resource *				dying_resource) = 0;

private: // are not used in this cook:
		virtual void	deallocate_resource		(pvoid) { FATAL("unexpected"); }
}; // inplace_managed_cook

class XRAY_CORE_API unmanaged_cook : public cook_base
{
public:
	unmanaged_cook	(class_id_enum	resource_class,
					 reuse_enum		reuse_type,
					 u32 			creation_thread_id		=	use_cook_thread_id,
					 u32 			allocate_thread_id		=	use_current_thread_id, 
					 enum_flags<flags_enum> flags			=	0);

	virtual	void			translate_request_path	(pcstr request, fs_new::virtual_path_string & new_request) const { new_request = request; }

	typedef fastdelegate::FastDelegate< void	(resources::query_result_for_cook &	in_out_query, 
												 mutable_buffer						in_out_unmanaged_resource_buffer) >
												 create_resource_if_no_file_delegate_type;
	
	virtual create_resource_if_no_file_delegate_type	get_create_resource_if_no_file_delegate	() { return NULL; }

	virtual	mutable_buffer	allocate_resource		(resources::query_result_for_cook &	in_query, 
													 const_buffer						raw_file_data, 
													 bool								file_exist) = 0;
	virtual void			deallocate_resource		(pvoid buffer) = 0;

	virtual void			create_resource			(resources::query_result_for_cook &	in_out_query, 
										 			 const_buffer						raw_file_data, 
													 mutable_buffer						in_out_unmanaged_resource_buffer) = 0; 
	virtual void			destroy_resource		(resources::unmanaged_resource *	resource) = 0;

}; // unmanaged_cook

class XRAY_CORE_API inplace_unmanaged_cook : public cook_base
{
public:
	enum inplace_allocate_data_enum {	inplace_allocate_data_inline_data,
										inplace_allocate_data_user_creation_data,	};
	typedef fastdelegate::FastDelegate< void	(resources::query_result_for_cook &  in_out_query,
												 mutable_buffer						 in_out_unmanaged_resource_buffer) >
												 create_resource_if_no_file_delegate_type;

	typedef create_resource_if_no_file_delegate_type	create_resource_inplace_delegate_type;
public:
	inplace_unmanaged_cook	(class_id_enum	resource_class,
							 reuse_enum		reuse_type,
					 		 u32 			creation_thread_id		=	use_cook_thread_id,
					 		 u32 			allocate_thread_id		=	use_current_thread_id,
							 enum_flags<flags_enum> flags					=	0);

	virtual create_resource_if_no_file_delegate_type	get_create_resource_if_no_file_delegate				() { return NULL; }

	virtual create_resource_inplace_delegate_type		get_create_resource_inplace_in_inline_fat_delegate		() { return NULL; }
	virtual create_resource_inplace_delegate_type		get_create_resource_inplace_in_creation_data_delegate	() { return NULL; }

	virtual	void			translate_request_path	(pcstr request, fs_new::virtual_path_string & new_request) const { new_request = request; }
	
	virtual	mutable_buffer	allocate_resource		(resources::query_result_for_cook &	in_query, 
													 u32								file_size, 
													 u32 &								out_offset_to_file, 
													 bool								file_exist) = 0;
	virtual void			deallocate_resource		(pvoid buffer) = 0;

	virtual void			create_resource			(resources::query_result_for_cook & in_out_query, 
													 mutable_buffer						in_out_unmanaged_resource_buffer) = 0;

	virtual void			destroy_resource		(resources::unmanaged_resource *	resource) = 0;

}; // class inplace_unmanaged_cook

class XRAY_CORE_API translate_query_cook : public cook_base
{
public:
							translate_query_cook	(class_id_enum resource_class, reuse_enum reuse_type, u32 translate_query_thread, enum_flags<flags_enum> flags = 0);

	virtual	void			translate_request_path	(pcstr request, fs_new::virtual_path_string & new_request) const { new_request = request; }
	virtual	void			translate_query			(resources::query_result_for_cook &	parent) = 0;
	virtual void			delete_resource			(resources::resource_base * resource) = 0;

private: // are not used in this cook:
	virtual void			deallocate_resource		(pvoid) { FATAL("unexpected"); }
}; // class translate_query_cook


} // namespace resources
} // xray

#endif // #ifndef XRAY_RESOURCES_COOK_CLASSES_H_INCLUDED