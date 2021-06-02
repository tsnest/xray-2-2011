////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "controller.h"
//#include "test_grasping_controller.h"
#include "animation_grasping_controller.h"
#include <xray/animation/world.h>

namespace xray {
namespace rtp {

void learn_step_controllers(  vector< animation_controller_ptr > &controllers, float stop_residual )
{
		vector< animation_controller_ptr>::iterator b = controllers.begin(), e = controllers.end();
		vector< animation_controller_ptr>::iterator  i = b;	

		for( ; i != e  ; ++i )
		{
			(*i)->step_logic( stop_residual );
		}
}

void	learn_init_controllers(  vector< animation_controller_ptr > &controllers )
{
		vector< animation_controller_ptr>::iterator b = controllers.begin(), e = controllers.end();
		vector< animation_controller_ptr>::iterator  i = b;	

		for( ; i != e  ; ++i )
		{

			(*i)->learn_init();
		
		}
}


void	learn_stop_controllers(  vector< animation_controller_ptr > &controllers )
{
		vector< animation_controller_ptr>::iterator b = controllers.begin(), e = controllers.end();
		vector< animation_controller_ptr>::iterator  i = b;	

		for( ; i != e  ; ++i )
		{

			(*i)->learn_stop();
		
		}
}

//base_controller* create_test_grasping_controller(  )
//{
//	
//	return NEW( test_grasping_controller );
//}


base_controller* create_animation_grasping_controller( pcstr name, animation::world &w, pcstr animation_set_path )
{
	
	animation_grasping_controller *ctrl = NEW( animation_grasping_controller )( name, w, animation_set_path );
	//ctrl->debug_test_add_actions( );
	return ctrl;
}

//base_controller* create_animation_grasping_controller( animation::world &w  )
//{
//	animation_grasping_controller *ctrl = NEW( animation_grasping_controller )( w.dbg_tmp_controller_set() );
//	return ctrl;
//}

base_controller* create_controller( pcstr name, const xray::configs::binary_config_ptr &config_ptr, animation::world &w )
{
	const xray::configs::binary_config_value cfg = config_ptr->get_root(); 

	pcstr type = cfg["controller"]["type"];
	//g_allocator->user_thread_id	(threading::current_thread_id());

	//if( strings::compare( type, "test_grasping" ) == 0 )
	//	return  NEW( test_grasping_controller )(  );
	if( strings::compare( type, "animation_grasping" ) == 0 )
		return NEW( animation_grasping_controller )( name, w, config_ptr );
	else if( strings::compare( type, "navigation" ) == 0 )
		return create_navigation_controller( name, w, config_ptr );

		//return create_animation_grasping_controller( w );

	NODEFAULT(return 0);
}

} // namespace rtp
} // namespace xray