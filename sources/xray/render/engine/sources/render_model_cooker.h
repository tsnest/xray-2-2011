////////////////////////////////////////////////////////////////////////////
//	Created		: 05.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RENDER_MODEL_COOKER_H_INCLUDED
#define RENDER_MODEL_COOKER_H_INCLUDED

#include <xray/resources_cook_classes.h>
#include "render_model.h"

namespace xray {
namespace render {

struct model_asset
{
	~model_asset()
	{
	}
	
	resources::managed_resource_ptr		converted_model_buffer;
	resources::unmanaged_resource_ptr	material;
	configs::binary_config_ptr			export_properties_config;
	fs_new::virtual_path_string			m_surface_name;
};

struct cook_intermediate_data
{
							cook_intermediate_data	(	fs_new::virtual_path_string const& in_resource_path, 
														resources::query_result_for_cook* parent_query );

	fs_new::virtual_path_string			root_model_path;
	resources::query_result_for_cook*	parent_query;

	bool								status_failed;
	bool								render_model_data_ready;
	bool								material_data_ready;
	bool								material_settings_valid;
	
	render_model_ptr					result_model;
	model_asset*						assets;
	u32									m_num_render_models;
	vector<pcstr>						m_surface_materials;
	int				find_material_index( pcstr surface_name );
	void			register_models		( vfs::vfs_iterator const & fs_it );
};

struct composite_cook_intermediate_data
{
	struct item{
		render_model_ptr		m_model;
		float4x4				m_matrix_offset;

	};
	vector<item>				m_contents;
};

struct render_model_cook : public resources::translate_query_cook
{
	typedef resources::translate_query_cook		super;

								render_model_cook		( resources::class_id_enum model_type );
	virtual	void				translate_query			( resources::query_result_for_cook&	parent );

	virtual void				delete_resource			( resources::resource_base* resource );
private:
			
			void				on_subresources_loaded	( resources::queries_result& data, cook_intermediate_data* cook_data );
			void				on_materials_loaded		( resources::queries_result& data, cook_intermediate_data* cook_data);
			
			void				on_properties_loaded			( resources::queries_result& data, cook_intermediate_data* cook_data );
			void				on_material_settings_loaded		( resources::queries_result& data, cook_intermediate_data* cook_data );
			void				finish_model_creation			( resources::queries_result& data_material_effects, cook_intermediate_data* cook_data);
			
			void				query_materail_effects			( cook_intermediate_data* cook_data );

			void				on_fs_iterator_ready_submeshes	( cook_intermediate_data* data, xray::vfs::vfs_locked_iterator const & fs_it );
			
			u32					count_models					( vfs::vfs_iterator const & fs_it );
};

struct user_mesh_cook : public resources::unmanaged_cook
{
	typedef resources::unmanaged_cook		super;

								user_mesh_cook		( );

	virtual	mutable_buffer	allocate_resource		( resources::query_result_for_cook& in_query, 
													 const_buffer raw_file_data, 
													 bool file_exist );

	virtual void			deallocate_resource		( pvoid buffer );

	virtual void			create_resource			( resources::query_result_for_cook& in_out_query, 
										 			 const_buffer raw_file_data, 
													 mutable_buffer in_out_unmanaged_resource_buffer );

	virtual void			destroy_resource		( resources::unmanaged_resource* resource );

private:
};

class static_render_model_instance_cook : public resources::translate_query_cook 
{
private:
	typedef resources::translate_query_cook	super;

public:
					static_render_model_instance_cook( );

	virtual	void	translate_query			( resources::query_result_for_cook& parent );
	virtual void	delete_resource			( resources::resource_base* resource );
	
private:
	void			on_sub_resources_loaded	( resources::queries_result& data );
}; // class static_render_model_instance_cook

class skeleton_render_model_instance_cook : public resources::translate_query_cook 
{
private:
	typedef resources::translate_query_cook	super;

public:
					skeleton_render_model_instance_cook( );

	virtual	void	translate_query			( resources::query_result_for_cook& parent );
	virtual void	delete_resource			( resources::resource_base* resource );

private:
	void			on_sub_resources_loaded	( resources::queries_result& data );
}; // class skeleton_render_model_instance_cook

class composite_render_model_cook : public resources::translate_query_cook 
{
private:
	typedef resources::translate_query_cook	super;

public:
					composite_render_model_cook( );

	virtual	void	translate_query			( resources::query_result_for_cook& parent );
	virtual void	delete_resource			( resources::resource_base* resource );

private:
	void			on_composite_config_loaded		( resources::queries_result& data, resources::query_result_for_cook* parent );
	void			on_composite_contents_loaded	( resources::queries_result& data, composite_cook_intermediate_data* intermediate_data, resources::query_result_for_cook* parent );

}; // class composite_render_model_cook

class composite_render_model_instance_cook : public resources::translate_query_cook 
{
private:
	typedef resources::translate_query_cook	super;

public:
					composite_render_model_instance_cook( );

	virtual	void	translate_query			( resources::query_result_for_cook& parent );
	virtual void	delete_resource			( resources::resource_base* resource );

private:
	void			on_sub_resources_loaded	( resources::queries_result& data );
}; // class composite_render_model_instance_cook


//class converted_model_cook : public resources::translate_query_cook 
//{
//private:
//	typedef resources::translate_query_cook	super;
//
//public:
//					converted_model_cook	( );
//
//	virtual	void	translate_query			( resources::query_result_for_cook& parent );
//	virtual void	delete_resource			( resources::resource_base* resource );
//
//private:
//	void			request_convertation	( resources::query_result_for_cook& parent );
//	void			request_converted_file	( resources::query_result_for_cook& parent );
//	void			on_sources_loaded		( resources::queries_result& data, resources::query_result_for_cook* parent );
//	void			on_converted_loaded		( resources::queries_result& data, resources::query_result_for_cook* parent );
//}; // class converted_model_cook

} // namespace render
} // namespace xray

#endif // #ifndef RENDER_MODEL_COOKER_H_INCLUDED