////////////////////////////////////////////////////////////////////////////
//	Created		: 14.03.2011
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource_cooks.h"

#include <xray/resources_queries_result.h>
#include <xray/resources_fs.h>
#include <xray/resources_cook_classes.h>

#include <xray/rtp/base_controller.h>
#include <xray/fs/device_utils.h>

#include "rtp_world.h"
#include "controller.h"

namespace xray {
namespace rtp {

struct cook_controller:
	public resources::translate_query_cook,
	private boost::noncopyable
{

	cook_controller	( rtp_world &w ) 
	:	m_rtp_world( w ),
		translate_query_cook(	resources::animation_controller_class, reuse_true, use_current_thread_id )
	{		
	}
	
	void	on_sub_resources_loaded_learned	( resources::queries_result & result )
	{

		resources::query_result_for_cook* const parent	=	result.get_parent_query();

		if( !result.is_successful() )
		{
			parent->finish_query			( result_error );
			return;
		}
		
		configs::binary_config_ptr config	= static_cast_resource_ptr< xray::configs::binary_config_ptr >(result[0].get_unmanaged_resource());

		controller_resource_user_data data;

		if( parent->user_data() )
			parent->user_data()->try_get<controller_resource_user_data>( data );

		ASSERT( parent );
		pcstr name = fs_new::file_name_from_path( fs_new::virtual_path_string(parent->get_requested_path()) );

		base_controller*	out		= create_controller( name, config, *m_rtp_world.animation_world() );
		
		parent->set_unmanaged_resource	( out, resources::nocache_memory, out->memory_size() );

		parent->finish_query			( result_success );
	}

	void	on_sub_resources_loaded_set	( resources::queries_result & result )
	{
		
		resources::query_result_for_cook* const parent	=	result.get_parent_query();

		if( !result.is_successful() )
		{
			parent->finish_query			( result_error );
			return;
		}

		ASSERT( parent->user_data() );

		controller_resource_user_data data;
		bool const data_obtained = parent->user_data()->try_get<controller_resource_user_data>( data ) ;
		R_ASSERT_U( data_obtained );
		R_ASSERT( data.name );
		configs::binary_config_ptr config	= static_cast_resource_ptr< xray::configs::binary_config_ptr >(result[0].get_unmanaged_resource());
		base_controller*	out				= rtp::create_navigation_controller( data.name, *m_rtp_world.animation_world(), parent->get_requested_path(), config ) ;
		parent->set_unmanaged_resource	( out, resources::nocache_memory, out->memory_size() );
		parent->finish_query			( result_success );
	}


	virtual void				translate_query	( resources::query_result_for_cook& parent )
	{
		
		pcstr config_path = 0; 

		bool	empty_set_queried = false;
		
		if( parent.user_data() )
		{
			controller_resource_user_data data;
			bool const data_obtained = parent.user_data()->try_get<controller_resource_user_data>( data ) ;
			R_ASSERT_U( data_obtained );
			empty_set_queried = data.empty_set;

			LOG_INFO						("query: %s, has user data  ", parent.get_requested_path());

		} else
			LOG_INFO						("query: %s, no user data ", parent.get_requested_path());


		if( empty_set_queried ) //query set only
		{
			STR_JOINA						(config_path, parent.get_requested_path(), ".lua");
			
			LOG_INFO						("query set: %s", config_path );

			resources::query_resource 		( config_path, 
										 resources::binary_config_class, 
										 boost::bind( &cook_controller::on_sub_resources_loaded_set, this, _1 ), 
										 g_allocator, 
										 NULL,
										 &parent );

			return;
		}

		// else query learned controller

		STR_JOINA						(config_path, parent.get_requested_path(), ".controller");
		
		LOG_INFO						("query controller: %s", config_path );

		resources::query_resource 		( config_path, 
										 resources::binary_config_class, 
										 boost::bind( &cook_controller::on_sub_resources_loaded_learned, this, _1 ), 
										 g_allocator, 
										 NULL,
										 &parent );

	}

	virtual void delete_resource ( resources::resource_base* res )
	{
		base_controller *data = static_cast_checked<base_controller *>( res ) ;
		ASSERT( data );
		DELETE( data );
	}
	
	rtp_world	& m_rtp_world;

}; // struct cook_controller


resource_cooks::resource_cooks(  rtp_world &w )
{
	
	{
		static cook_controller cook( w );
		resources::register_cook( &cook );
	}

}

resource_cooks::~resource_cooks( )
{

}

}//namespace rtp
}//namespace xray

