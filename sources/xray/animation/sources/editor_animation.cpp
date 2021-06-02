////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"

#ifndef MASTER_GOLD

#include "editor_animation.h"
#include <xray/animation/skeleton_animation.h>
#include <xray/animation/bone_animation.h>
#include <xray/animation/editable_animation_event_channels.h>

#include <xray/resources_queries_result.h>
#include <xray/resources_fs.h>

#include <xray/animation/cubic_spline_skeleton_animation.h>

using xray::animation::editor_animation;
using xray::animation::skeleton_animation;
using xray::animation::skeleton_animation_ptr;
using xray::animation::cubic_spline_skeleton_animation_pinned;
using xray::animation::frame;

editor_animation::editor_animation( skeleton_animation_ptr  anim ):
m_animation( anim ),
m_load_failed( false )
{
}

void editor_animation::on_animation_loaded( xray::resources::queries_result& resource )
{
	if( !resource.is_successful() )
	{
		m_load_failed = true;
		return;
	}
	ASSERT( resource.is_successful() );

	ASSERT( resource.size() == 1 );


	m_animation =   static_cast_resource_ptr< skeleton_animation_ptr > ( resource[0].get_managed_resource() );


}


editor_animation::editor_animation( pcstr  anim_file ):
m_load_failed( false )
{
	
	xray::resources::request arr[] = {
		{ anim_file, xray::resources::animation_class },
	};

	
	xray::resources::query_resources(
		arr,
		array_size(arr),
		boost::bind( &editor_animation::on_animation_loaded, this, _1 ),
		g_allocator
	);

	 for ( ;; ) {
			xray::resources::dispatch_callbacks	( );
			xray::threading::yield	( 10 );
			
			if( m_animation || m_load_failed )
				break;

	}

}


u32 editor_animation::bones_count() const
{
	cubic_spline_skeleton_animation_pinned pinned_anim( m_animation );
	
	return pinned_anim.c_ptr()->bones_count();
}

void editor_animation::evaluate(u32 bone, u32 ch, float time, float& point, u32& domain) const
{
	cubic_spline_skeleton_animation_pinned pinned_anim( m_animation );
	pinned_anim.c_ptr()->bone(bone).channel(enum_channel_id(ch)).evaluate(time, point, domain);
}

void	editor_animation::evaluate_frame		( pcstr bone,  float time, frame &f ) const
{
	cubic_spline_skeleton_animation_pinned pinned_anim( m_animation );
	pinned_anim.c_ptr()->bone(bone).get_frame( time, f, m_last_frame_position );
}


pcstr editor_animation::bone_name(u32 index) const
{
	cubic_spline_skeleton_animation_pinned pinned_anim( m_animation );
	return pinned_anim.c_ptr()->get_bone_names().bone_name(index);
}

u32 editor_animation::bone_index(pcstr bone_name) const
{
	cubic_spline_skeleton_animation_pinned pinned_anim( m_animation );
	return pinned_anim.c_ptr()->get_bone_names().bone_index(bone_name);
}

xray::configs::lua_config_value editor_animation::config_value( ) const
{
	cubic_spline_skeleton_animation_pinned pinned_anim( m_animation );
	return pinned_anim.c_ptr()->get_editable_animation_event_channels()->config_value();
}

void editor_animation::save_config_value( pcstr path ) const
{
	cubic_spline_skeleton_animation_pinned pinned_anim( m_animation );
	pinned_anim.c_ptr()->get_editable_animation_event_channels()->save(path);
}

float editor_animation::length( ) const
{
	xray::animation::cubic_spline_skeleton_animation_pinned animation(m_animation);
	return animation.c_ptr()->length_in_frames() / xray::animation::default_fps;
}

#endif//#ifndef MASTER_GOLD