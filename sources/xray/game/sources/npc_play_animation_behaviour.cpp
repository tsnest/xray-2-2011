////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "npc_play_animation_behaviour.h"
#include "human_npc.h"
#include "object_scene.h"
#include <xray/ai/world.h>

namespace stalker2 {

npc_play_animation_behaviour::npc_play_animation_behaviour	(
		object_scene_job* owner, 
		configs::binary_config_value const& data, 
		pcstr name
	) :
	super								( owner, data, name ),
	m_object							( 0 ),
	m_behaviour							( 0 )
{	
	xray::ai::behaviour_cook_params		cook_params( &m_data["behaviour_config"] );
	resources::user_data_variant		user_data_out;
	user_data_out.set					( cook_params );

	query_resource					(
		full_name().c_str(),
		resources::behaviour_class,
		boost::bind( &npc_play_animation_behaviour::on_behaviour_loaded, this, _1 ),
		g_allocator,
		&user_data_out
	);
}

void npc_play_animation_behaviour::attach_to_object			( object_controlled* o )
{
	m_object							 = o;
	super::attach_to_object				( o );	

	if ( m_behaviour )
	{
		if ( m_object )
		{	
			human_npc* npc = dynamic_cast< human_npc* >( m_object );
			R_ASSERT( npc != NULL );
			npc->set_behaviour( m_behaviour );
		}		
	}
}

void npc_play_animation_behaviour::detach_from_object		( object_controlled* o )
{
	m_object							= 0;
	super::detach_from_object			( o );
}

void npc_play_animation_behaviour::on_behaviour_loaded		( resources::queries_result& data )
{
	R_ASSERT												( data.is_successful() );

	m_behaviour							= data[0].get_unmanaged_resource();
	if ( m_object )
	{	
		human_npc* npc = dynamic_cast< human_npc* >( m_object );
		R_ASSERT( npc != NULL );
		npc->set_behaviour( m_behaviour );
	}
}

} // namespace stalker2