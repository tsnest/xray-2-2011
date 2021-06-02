////////////////////////////////////////////////////////////////////////////
//	Created		: 22.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_OBJECT_COOK_H_INCLUDED
#define SOUND_OBJECT_COOK_H_INCLUDED

namespace xray
{
	namespace resources
	{
		struct sound_object_resource : public unmanaged_resource
		{
			sound_object_resource(pcbyte data_raw);

			unmanaged_resource_ptr m_options_resource_ptr;
		};//struct sound_object_resource

#pragma message(XRAY_TODO("fix cooks"))
#if 0 // commented by Lain

 		struct ogg_sound_cook_wrapper : public resources::cook
 		{
 		public:
 			ogg_sound_cook_wrapper									();
 	
 			virtual void			translate_query				(resources::query_result& parent);
 			void					on_fs_iterator_ready		(fs::path_string* resource_path, resources::query_result_for_cook* parent, xray::resources::fs_iterator fs_it);
 			void					request_convertion			(fs::path_string* resource_path, resources::query_result_for_cook* parent);
 			void					request_convertion_options	(fs::path_string* resource_path, query_result_for_cook* parent);
 			void					on_sound_converted			(bool is_options_complete, resources::queries_result& result);
 			void					sound_loaded				(queries_result& result);
 		}; // struct ogg_sound_cook_wrapper
 
 		struct ogg_sound_cook : public cook
 		{
 									ogg_sound_cook				();
 
 			virtual void			translate_query				(query_result& parent);
 			void					on_sub_resources_loaded		(queries_result& result);
 		};//struct ogg_sound_cook
 
 		struct ogg_cook : public cook
 		{
 									ogg_cook					();
 
 			virtual result_enum		create_resource						(query_result_for_cook& query, u32& final_managed_resource_size);
 			virtual void			delete_unmanaged			(unmanaged_resource* res);
 		};//struct ogg_cook
 
 		struct ogg_converter : public cook
 		{
 									ogg_converter				();
 
 			virtual result_enum		create_resource						(query_result_for_cook& query, u32& final_managed_resource_size);
 			void					on_raw_properties_loaded	(queries_result& result);
 		};//struct ogg_converter
 
 		struct ogg_options_converter : public cook
 		{
 									ogg_options_converter		();
 
 			virtual result_enum		create_resource						(query_result_for_cook& query, u32& final_managed_resource_size);
 		};//struct ogg_options_converter

#endif // #if 0 // commented by Lain

	}//namespace resources
}//namespace xray

#endif // #ifndef SOUND_OBJECT_COOK_H_INCLUDED