////////////////////////////////////////////////////////////////////////////
//	Created		: 05.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource_cookers.h"
#include "skeleton_animation_data.h"
#include "check_animation_data.h"
#include "skeleton.h"

#include <xray/resources_queries_result.h>
#include <xray/configs_lua_config.h>
#include <xray/resources_fs.h>
#include <xray/resources_cook_classes.h>


namespace xray {
namespace animation {

command_line::key	check_animation_b_spline_approximation("check_animation_b_spline_approximation", "", "animation", "");

struct cook_animation_data:
	public resources::translate_query_cook,
	private boost::noncopyable
{

	cook_animation_data	(  world &w  ) 
		: translate_query_cook(	resources::animation_data_class, reuse_true, use_user_thread_id ),
		  m_world( w )
	{ 
		
	}
	
	void	on_sub_resources_loaded( resources::queries_result & result )
	{
		timing::timer					timer;
		timer.start						( );

		ASSERT							( result.is_successful() );
		resources::query_result_for_cook * const parent	=	result.get_parent_query();
		xray::configs::lua_config_ptr config	= static_cast_resource_ptr<xray::configs::lua_config_ptr>( result[0].get_unmanaged_resource() );
		skeleton_animation_data * out	=	NEW(skeleton_animation_data)(  );
		out->load						( (*config)["splines"] );

		if( check_animation_b_spline_approximation )
		{
			xray::configs::lua_config_ptr config_check_animation	= static_cast_resource_ptr<xray::configs::lua_config_ptr>(result[1].get_unmanaged_resource());
			test_data( *config, *config_check_animation, m_world  );
		}

		parent->set_unmanaged_resource	(out, 
										 resources::memory_type_non_cacheable_resource,
										 sizeof(skeleton_animation_data));

		LOGI_INFO						("resources:cook_animation_data", "on_sub_resources_loaded created resource: %s, address: %d", parent->get_requested_path() );
		parent->finish_query			( result_success );

		LOG_INFO						( "animation creation took time: %f", timer.get_elapsed_sec() );
	}

	virtual void	translate_query( resources::query_result& parent )
	{
		LOGI_INFO("resources:cook_animation_data",	"translate_query called");
		if( !check_animation_b_spline_approximation )
		{

			pcstr config_path				= 0; 
			STR_JOINA						(config_path, parent.get_requested_path(), ".clip");

			resources::query_resource		(config_path, 
											 xray::resources::config_lua_class,
											 boost::bind(&cook_animation_data::on_sub_resources_loaded, this, _1), 
											 parent.get_user_allocator(), 
											 NULL,
											 0,
											 &parent);

		} else {
			
			pcstr check_data_file_path = 0; 
			STR_JOINA( check_data_file_path, parent.get_requested_path(), ".clip_check_data" );
			resources::request arr[] = {	
				{ parent.get_requested_path(),		xray::resources::config_lua_class },
				{ check_data_file_path,				xray::resources::config_lua_class },
			};
		
			resources::query_resources		(arr, 
											 array_size(arr), 
											 boost::bind(&cook_animation_data::on_sub_resources_loaded, this, _1), 
											 parent.get_user_allocator(), 
											 NULL,
											 0,
											 &parent); 

		}
	}

	virtual void delete_resource ( resources::unmanaged_resource* res )
	{
		XRAY_UNREFERENCED_PARAMETER( res );
		i_skeleton_animation_data *data = static_cast_checked<i_skeleton_animation_data*>( res ) ;
		ASSERT( data );
		DELETE( data );
	}

private:
	world	&m_world;

}; // struct cook_animation_data
	
struct cook_skeleton:
	public resources::translate_query_cook,
	private boost::noncopyable
{

	cook_skeleton	( ) 
		: translate_query_cook(	resources::skeleton_class, reuse_true, use_user_thread_id )
	{		
	}
	
	void	on_sub_resources_loaded	( resources::queries_result & result )
	{
		ASSERT						(result.is_successful());
		resources::query_result_for_cook * const parent	=	result.get_parent_query();
		xray::configs::lua_config_ptr config	= static_cast_resource_ptr<xray::configs::lua_config_ptr>(result[0].get_unmanaged_resource());
		skeleton * out	=	NEW(skeleton)(  (*config)  );
		
		parent->set_unmanaged_resource	(out, resources::memory_type_non_cacheable_resource, sizeof(skeleton));
		LOGI_INFO						("resources:cook_skeleton", "on_sub_resources_loaded created resource: %s, address: %d", parent->get_requested_path() );
		parent->finish_query	(result_success);
	}

	virtual void				translate_query	( resources::query_result& parent )
	{
		LOGI_INFO("resources:cook_skeleton",	"translate_query called");
	
		pcstr config_path = 0; 
		STR_JOINA						(config_path, parent.get_requested_path(), ".lua");

		resources::query_resource 		(config_path, 
										 xray::resources::config_lua_class, 
										 boost::bind(&cook_skeleton::on_sub_resources_loaded, this, _1), 
										 parent.get_user_allocator(), 
										 NULL,
										 0,
										 &parent);

	}

	virtual void delete_resource ( resources::unmanaged_resource* res )
	{
		skeleton *data = static_cast_checked<skeleton *>( res ) ;
		ASSERT( data );
		DELETE( data );
	}

}; // struct cook_skeleton

resource_cookers::resource_cookers(  world &w )
{
	
	{
		static cook_animation_data cook( w );
		resources::register_cook( &cook );
	}

	{
		static cook_skeleton cook;
		resources::register_cook( &cook );
	}

}

resource_cookers::~resource_cookers( )
{

}

}	// namespace animation
}	// namespace xray 