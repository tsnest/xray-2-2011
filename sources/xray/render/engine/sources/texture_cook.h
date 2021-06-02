////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_COOK_H_INCLUDED
#define TEXTURE_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>
#include <xray/resources_fs.h>

namespace xray
{
	namespace resources
	{
		class cook;
		class queries_result;
		class query_result;
		class unmanaged_resource;
	}

	namespace render
	{
		struct texture_cook_data_block
		{
			texture_cook_data_block(resources::unmanaged_resource_ptr options, resources::managed_resource_ptr buffer, fs::path_string dest_path);				
			
			resources::unmanaged_resource_ptr	m_options;
			resources::managed_resource_ptr				m_buffer;
			fs::path_string						m_dest_path;
		};

		struct texture_data_resource : public resources::unmanaged_resource
		{
		public:
			texture_data_resource(mutable_buffer raw_data);
			~texture_data_resource();

			//virtual	void						recalculate_memory_usage_impl ( ) { m_memory_usage_self.unmanaged = get_size(); }
			
			mutable_buffer	m_buffer;
		};//struct sound_object_resource

		struct texture_cook_wrapper_data
		{
			texture_cook_wrapper_data():
				source_info_time		(0),
				converted_info_time		(0),
				source_options_info_time(0),
				query_result			(NULL),
				source_ready		(false),
				converted_ready	(false)
			{ }

			u32 source_info_time;
			u32 converted_info_time;
			u32 source_options_info_time;

			fs::path_string						resource_path;
			resources::query_result_for_cook*	query_result;

			bool source_ready;
			bool converted_ready;
		};

		struct texture_cook_wrapper : public resources::translate_query_cook
		{
		public:
			texture_cook_wrapper	();
	
			virtual void			translate_query				(resources::query_result & parent);
			virtual void			delete_resource				(resources::unmanaged_resource * resource);
			
			void					on_fs_iterator_ready_source		(texture_cook_wrapper_data* data, xray::resources::fs_iterator fs_it);
			void					on_fs_iterator_ready_converted	(texture_cook_wrapper_data* data, xray::resources::fs_iterator fs_it);
			void					on_fs_iterators_ready			(texture_cook_wrapper_data* data);

			void					request_convertion			(fs::path_string* resource_path, resources::query_result_for_cook* parent);
			void					on_raw_data_loaded			(resources::queries_result& result);
			void					on_texture_converted		(resources::queries_result& result);

		}; // struct texture_cooker

		struct texture_cook : public resources::unmanaged_cook
		{
		public:
			texture_cook 		();
	
//			virtual	void			translate_request			(pcstr	request, resources::class_id res_class, fs::path_string& new_request, resources::class_id& new_res_class) const;
			
			virtual	mutable_buffer			allocate_resource		(resources::query_result_for_cook &	in_query, 
																	 const_buffer						raw_file_data, 
																	 bool								file_exist)
			{
				XRAY_UNREFERENCED_PARAMETERS	(& in_query, & raw_file_data, file_exist);
				return							mutable_buffer::zero();
			}

			virtual void					deallocate_resource		(pvoid) {}

			virtual void					destroy_resource		(resources::unmanaged_resource *	resource);

			virtual void					create_resource			(resources::query_result_for_cook &	in_out_query, 
												 					 const_buffer						raw_file_data, 
																	 mutable_buffer						in_out_unmanaged_resource_buffer); 
		}; // struct texture_cooker

		struct texture_converter_cook : public resources::unmanaged_cook
		{
		public:
			texture_converter_cook();

			virtual	mutable_buffer			allocate_resource		(resources::query_result_for_cook &	in_query, 
																	 const_buffer						raw_file_data, 
																	 bool								file_exist)
			{
				XRAY_UNREFERENCED_PARAMETERS						(& in_query, & raw_file_data, file_exist);
				return												mutable_buffer::zero();
			}

			virtual void					deallocate_resource		(pvoid) {}
	
			virtual void					create_resource			(resources::query_result_for_cook &	in_out_query, 
												 					 const_buffer						raw_file_data, 
																	 mutable_buffer						in_out_unmanaged_resource_buffer); 
			virtual void					destroy_resource		(resources::unmanaged_resource *	resource);
		}; // struct texture_cooker
	} // namespace render
} // namespace xray

#endif // #ifndef TEXTURE_COOK_H_INCLUDED