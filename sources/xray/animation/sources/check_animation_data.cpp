////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_PLATFORM_WINDOWS && !defined(MASTER_GOLD)

#include "check_animation_data.h"
#include "bi_spline_skeleton_animation.h"
#include <xray/animation/cubic_spline_skeleton_animation.h>
#include <xray/animation/world.h>

#if XRAY_USE_MAYA_ANIMATION
#	include <xray/maya_animation/world.h>
#	include <xray/maya_animation/api.h>
#	include <xray/maya_animation/discrete_data.h>
#	include <xray/maya_animation/engine.h>
#endif // #if XRAY_USE_MAYA_ANIMATION

namespace xray {

namespace maya_animation
{
	class discrete_data;
	struct world;
} // namespace maya_animation

namespace animation {

bool	compare(	const maya_animation::discrete_data& disc_data, 
					const cubic_spline_skeleton_animation& anim );

bool	compare(	configs::lua_config_ptr					check_data,
					const cubic_spline_skeleton_animation&	anim )
{
	xray::maya_animation::engine maya_animation_engine;
	xray::maya_animation::world *maw = maya_animation::create_world( maya_animation_engine );
	ASSERT( maw );

	maya_animation::discrete_data* disc_data = maw->create_discrete_data();

	disc_data->load					( check_data->get_root()["discrete_data"] );

	bool result						= compare ( *disc_data, anim );

	maw->destroy					( disc_data );
	maya_animation::destroy_world	( maw );

	return result;
}

bool	compare(	const xray::maya_animation::discrete_data &disc_data, 
					const cubic_spline_skeleton_animation &anim )
{

	XRAY_UNREFERENCED_PARAMETER( anim );
	XRAY_UNREFERENCED_PARAMETER( disc_data );

	bool check_result = true;

#if ( XRAY_USE_MAYA_ANIMATION )
	const u32 bones_count = disc_data.bone_count();
	if ( anim.bones_count() != bones_count ) 
		return false;
	
	const u32 frame_count = disc_data.frame_count();
	

	for ( u32 bone = 0; bone < bones_count; ++bone )
	{

			for (u32 ch = channel_translation_x; ch  < channel_max; ++ch )
			{
				for (u32 frame_number = 0; frame_number < frame_count; ++frame_number )	
				{
					xray::animation::frame frame; float time = xray::math::QNaN;
					disc_data.get_frame( bone, frame_number, time, frame ) ;

					float v = xray::math::QNaN; u32 frame_current = 0;
					anim.bone( bone ).channel( enum_channel_id(ch) ).evaluate( time, v, frame_current );
				
					pcstr bone_name = anim.get_bone_names().bone_name(bone);
					if (! xray::math::is_similar(v,frame.channels[ch], 2*disc_data.precision( bone, enum_channel_id(ch) ) ) )
					{
						float difference = math::abs( math::abs(v) - math::abs(frame.channels[ch]));
						check_result = false;
						LOG_ERROR("check animation failed for bone [%s] at channel[%d] frame[%d] delta=[%f]", bone_name, ch, frame_number, difference );
					}
				}
			}
	}
#endif // #if XRAY_USE_MAYA_ANIMATION

	return check_result;
}

void test_move ( cubic_spline_skeleton_animation const*& anim, u32 size )
{
	 cubic_spline_skeleton_animation *anim_new_place = (cubic_spline_skeleton_animation*) XRAY_MALLOC_IMPL(debug::g_mt_allocator, size, "test_move - cubic_spline_skeleton_animation" );
	 memory::copy( anim_new_place, size, anim, size );

	 XRAY_FREE_IMPL(debug::g_mt_allocator, anim );
	 anim = anim_new_place;
}


void test_data	(
		xray::configs::binary_config_value const &animation_data, 
		xray::configs::lua_config_value	const &check_data,
		animation::world &world
	)
{

	XRAY_UNREFERENCED_PARAMETER( animation_data );
	XRAY_UNREFERENCED_PARAMETER( check_data );
	XRAY_UNREFERENCED_PARAMETER( world );

#if XRAY_USE_MAYA_ANIMATION
	xray::maya_animation::engine maya_animation_engine;
	xray::maya_animation::world *maw = xray::maya_animation::create_world( maya_animation_engine );
	ASSERT( maw );

	xray::maya_animation::discrete_data *disc_data = maw->create_discrete_data();

	disc_data->load( check_data["discrete_data"] );

	xray::animation::bi_spline_skeleton_animation *ianim_data_builded	= world.create_skeleton_animation_data( 0 );
	xray::animation::bi_spline_skeleton_animation *ianim_data_loaded	= world.create_skeleton_animation_data( 0 );
	
	xray::animation::bi_spline_skeleton_animation *anim_data_builded =  static_cast_checked<xray::animation::bi_spline_skeleton_animation*> ( ianim_data_builded  );
	xray::animation::bi_spline_skeleton_animation *anim_data_loaded =  static_cast_checked<xray::animation::bi_spline_skeleton_animation*> ( ianim_data_loaded );

	maw->build_animation_data( *disc_data, *anim_data_builded );

	
	anim_data_loaded->load( animation_data["splines"] );

	anim_data_builded->set_bone_names( anim_data_loaded->bone_names() );



	cubic_spline_skeleton_animation const *anim_builded = cubic_spline_skeleton_animation::new_animation( debug::g_mt_allocator, *anim_data_builded );
	cubic_spline_skeleton_animation const *anim_loaded =  cubic_spline_skeleton_animation::new_animation( debug::g_mt_allocator, *anim_data_loaded );

	compare( *disc_data, *anim_loaded );
	test_move( anim_loaded, cubic_spline_skeleton_animation::count_memory_size(  *anim_data_loaded ) );
	compare( *disc_data, *anim_loaded );

	anim_data_builded->compare( *anim_data_loaded );
	
	maw->destroy( disc_data );
	world.destroy( ianim_data_builded );
	world.destroy( ianim_data_loaded );

	cubic_spline_skeleton_animation::delete_animation( anim_builded );
	cubic_spline_skeleton_animation::delete_animation( anim_loaded );

	xray::maya_animation::destroy_world( maw );
#endif // #if XRAY_USE_MAYA_ANIMATION
}

} // namespace animation
} // namespace xray

#endif // #if XRAY_PLATFORM_WINDOWS && !defined(MASTER_GOLD)