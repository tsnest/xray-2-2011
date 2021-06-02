////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_COOKER_NEW_H_INCLUDED
#define TEXTURE_COOKER_NEW_H_INCLUDED

namespace xray
{
	namespace resources
	{
		class cook;
		class queries_result;
		class query_result;
		class unmanaged_resource;
	}

	namespace editor
	{
		struct texture_cooker_data_block
		{
			texture_cooker_data_block(resources::unmanaged_resource_ptr options, resources::resource_ptr buffer, fs::path_string dest_path);				
			
			resources::unmanaged_resource_ptr	m_options;
			resources::resource_ptr				m_buffer;
			fs::path_string						m_dest_path;
		};

		struct texture_data_resource : public resources::unmanaged_resource
		{
		public:
			texture_data_resource(mutable_buffer raw_data);
			~texture_data_resource();
			
			mutable_buffer	m_buffer;
		};//struct sound_object_resource

		struct texture_cooker_new : public resources::cook
		{
		public:
			texture_cooker_new 		();
	
			virtual	void			translate_request_path		(pcstr	request, resources::class_id res_class, fs::path_string& new_request, resources::class_id& new_res_class) const;
			virtual result_enum		process						(resources::query_result_for_cook& query, u32& final_managed_resource_size);
			void					on_raw_data_loaded			(resources::queries_result& result);
			void					on_texture_converted		(resources::queries_result& result);
			virtual void			delete_unmanaged			(resources::unmanaged_resource* res);
		}; // struct texture_cooker

		struct texture_converter_cooker : public resources::cook
		{
		public:
			texture_converter_cooker();
	
			virtual result_enum		process						(resources::query_result_for_cook& query, u32& final_managed_resource_size);
			virtual void			delete_unmanaged			(resources::unmanaged_resource* res);
		}; // struct texture_cooker
	} // namespace editor
} // namespace xray

#endif // #ifndef TEXTURE_COOKER_NEW_H_INCLUDED