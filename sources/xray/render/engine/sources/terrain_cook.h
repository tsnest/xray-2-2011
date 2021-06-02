////////////////////////////////////////////////////////////////////////////
//	Created		: 13.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_TERRAIN_COOK_H_INCLUDED
#define XRAY_RENDER_ENGINE_TERRAIN_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace render {

struct terrain_mesh_cook : public resources::unmanaged_cook {
	typedef resources::unmanaged_cook	super;

								terrain_mesh_cook		( );

	virtual	mutable_buffer		allocate_resource		(resources::query_result_for_cook &	in_query, 
														 const_buffer						raw_file_data, 
														 bool								file_exist)
	{
		XRAY_UNREFERENCED_PARAMETERS					(& in_query, & raw_file_data, file_exist);
		NOT_IMPLEMENTED									(return mutable_buffer::zero());
	}

	virtual void				deallocate_resource		(pvoid) { NOT_IMPLEMENTED(); }

	virtual create_resource_if_no_file_delegate_type	get_create_resource_if_no_file_delegate	() 
	{ 
		return					create_resource_if_no_file_delegate_type(this, & terrain_mesh_cook::create_resource_if_no_file);
	}
	
	void						create_resource_if_no_file	(resources::query_result_for_cook &	in_out_query, 
															 mutable_buffer						in_out_unmanaged_resource_buffer);

	virtual void				destroy_resource		(resources::unmanaged_resource *		resource);

	virtual void				create_resource			(resources::query_result_for_cook &	in_out_query, 
											 			 const_buffer				raw_file_data, 
														 mutable_buffer				in_out_unmanaged_resource_buffer);

	void						raw_data_loaded				( resources::queries_result &	raw_file_results );
};

struct terrain_instance_cook : public resources::translate_query_cook
{
	typedef resources::translate_query_cook		super;

								terrain_instance_cook	( );
	virtual	void				translate_query			( resources::query_result_for_cook&	parent );

	virtual void				delete_resource			( resources::resource_base* resource );
private:
			void				on_subresources_loaded	( resources::queries_result& data, 
														  resources::query_result_for_cook* parent_query );
};

struct terrain_model_cook : public resources::translate_query_cook
{
	typedef resources::translate_query_cook		super;

								terrain_model_cook	( );
	virtual	void				translate_query			( resources::query_result_for_cook&	parent );

	virtual void				delete_resource			( resources::resource_base* resource );
private:
			void				on_subresources_loaded	( resources::queries_result& data, 
														  resources::query_result_for_cook* parent_query );
};

} // namespace render
} //namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_TERRAIN_COOK_H_INCLUDED