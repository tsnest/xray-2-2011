////////////////////////////////////////////////////////////////////////////
//	Created		: 05.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MODEL_COOKER_H_INCLUDED
#define MODEL_COOKER_H_INCLUDED

#include <xray/resources_cook_classes.h>
#include <xray/render/facade/model.h>

namespace xray {

namespace animation {
	class skeleton;
	typedef	resources::resource_ptr <
		skeleton,
		resources::unmanaged_intrusive_base
	> skeleton_ptr;
} // namespace animation

namespace render {

class render_model_instance;

typedef	resources::resource_ptr<
	render_model_instance,
	resources::unmanaged_intrusive_base
> rrender_model_instance_ptr;

struct static_model_instance_cook : public resources::translate_query_cook
{
	typedef resources::translate_query_cook		super;

								static_model_instance_cook	( );
	virtual	void				translate_query			( resources::query_result_for_cook&	parent );

	virtual void				delete_resource			( resources::resource_base* resource );
private:
			void				on_subresources_loaded	( resources::queries_result& data, resources::query_result_for_cook* parent_query );
};

struct skeleton_model_instance_cook_data
{
	skeleton_model_instance_cook_data(resources::query_result_for_cook* in_parent_query)
		:parent_query		( in_parent_query ),
		render_model_ready	( false ),
		skeleton_ready		( false )
	{}
	bool											render_model_ready;
	bool											skeleton_ready;
	resources::query_result_for_cook*				parent_query;
	xray::render::render_model_instance_ptr			render_model;
	xray::animation::skeleton_ptr					skeleton;
};

struct skeleton_model_instance_cook : public resources::translate_query_cook
{
	typedef resources::translate_query_cook		super;
	
						skeleton_model_instance_cook	( );
	virtual	void		translate_query					( resources::query_result_for_cook&	parent );
	
	virtual void		delete_resource					( resources::resource_base* resource );
	
private:
			void		on_render_model_loaded			( resources::queries_result& result, skeleton_model_instance_cook_data* cook_data );
			void		on_skeleton_config_loaded		( resources::queries_result& result, skeleton_model_instance_cook_data* cook_data );
			void		on_skeleton_loaded				( resources::queries_result& result, skeleton_model_instance_cook_data* cook_data );
			void		on_all_subresources_ready		( skeleton_model_instance_cook_data* cook_data );
};


} // namespace render
} // namespace xray

#endif // #ifndef MODEL_COOKER_H_INCLUDED