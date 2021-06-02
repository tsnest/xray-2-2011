////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_SPEEDTREE_COOK_H_INCLUDED
#define XRAY_RENDER_SPEEDTREE_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>
#include <xray/resources_fs.h>
#include "speedtree_tree.h"

namespace xray {
namespace render {

static u32 const num_speedtree_components			= 5;
struct material_effects_instance;
typedef	resources::resource_ptr<material_effects_instance, resources::unmanaged_intrusive_base> material_effects_instance_ptr;

struct speedtree_data
{
	resources::query_result_for_cook*	m_parent_query;
	material_ptr						m_materials[5];
	material_effects_instance_ptr		m_material_effects[5];
	fs_new::virtual_path_string			m_material_path[5];
	speedtree_tree_ptr					m_model;
	bool								m_failed;
	bool						all_assests_ready();
};

struct speedtree_cook : public resources::translate_query_cook
{
	typedef resources::translate_query_cook		super;
	
	speedtree_cook								( );
	
	virtual	void translate_query				( resources::query_result_for_cook&	parent );
	virtual void delete_resource				( resources::resource_base* resource );
	
private:
			void on_speedtree_raw_data_loaded	( resources::queries_result& data, speedtree_data* d );
			void on_model_materials_loaded		( resources::queries_result& data, speedtree_data* d );
			void on_material_effects_created	( resources::queries_result& data, speedtree_data* d );
			void query_materail_effects			( speedtree_data* cook_data );
			void finish_model_creation			( speedtree_data* d );
}; // struct speedtree_cook


struct speedtree_instance_cook : public resources::translate_query_cook
{
	typedef resources::translate_query_cook		super;
	
	speedtree_instance_cook						();
	
	virtual	void translate_query				(resources::query_result_for_cook&	parent);
	virtual void delete_resource				(resources::resource_base* resource);
	
private:
			void on_speedtree_loaded			(resources::queries_result& data, resources::query_result_for_cook* parent_query);
}; // struct speedtree_instance_cook

} // namespace render 
} // namespace xray 

#endif // #ifndef XRAY_RENDER_SPEEDTREE_RENDER_INSTANCE_H_INCLUDED
