////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/spl_utils.h>

namespace xray {
namespace sound {

#ifndef MASTER_GOLD

configs::lua_config_ptr create_default_spl_config	( fs_new::virtual_path_string file_name )
{
	configs::lua_config_ptr config				= configs::create_lua_config( file_name.c_str() );
	configs::lua_config_value config_options	= config->get_root()["spl"];
	configs::lua_config_value keys				= config->get_root()["keys"];


	int i = 0;
	float La = 0.0f;
	float spl = 60.0f;
	do
	{
		float dist = 1.0f  * ++i;
		// Inverse Square Law
		float coeff = 0.0f;
		if ( math::is_zero( dist ))
		{
			coeff = 1.0f;
		}
		else
		{
			coeff = 1.0f / ( dist * dist );
		}
		La = 20 * log10( coeff ) + spl;

		if ( La > 0.0f )
		{
			keys[i - 1]["position"]["x"]			= i + 1;
			keys[i - 1]["position"]["y"]			= La;
		}
			
	} while ( La > 0.0f );

	config->save( configs::target_sources );

	return config;
}


configs::lua_config_ptr create_default_spl_config	(	fs_new::virtual_path_string file_name,
										memory::base_allocator* allocator
									)
{
		vectora< math::curve_point<float> > vec( allocator );
		float La					= 0.0f;
		float spl					= 60.0f;
		float unheareble_spl		= 20.0f;
		
		float unheareble_distance	= math::pow( 10, ( spl - unheareble_spl ) / 20 );
		float distance				= 1.0f;

		do
		{
			math::curve_point<float> pt;
			La								= spl - 20 * log10( distance );
			pt.time							= distance;
			pt.upper_value					= La;
			pt.lower_value					= La;
			vec.push_back					( pt );
			distance						+= 10.0f;
		} while ( distance < unheareble_distance );
		

		configs::lua_config_ptr cfg				= configs::create_lua_config( file_name.c_str( ) );
		configs::lua_config_value val			= cfg->get_root( )["spl"];

		for ( u32 i = 0; i < vec.size( ); ++i )
		{

			xray::fixed_string<8> key_name;
			key_name.assignf				("key%d", i);
			pcstr str						= key_name.c_str();

			val[str]["position"]		= math::float2 ( vec[i].time, vec[i].upper_value );
			val[str]["key_type"]		= 0;
			if ( i == 0 )
			{

				val[str]["right_tangent"]	= math::float2( vec[i + 1].time, vec[i + 1].upper_value );
				val[str]["left_tangent"]	= math::float2( 0.0f, 0.0f );
			}
			else if ( i == vec.size( ) - 1 )
			{
				val[str]["left_tangent"]	= math::float2( vec[i - 1].time, vec[i - 1].upper_value );
				val[str]["right_tangent"]	= math::float2( 0.0f, 0.0f );
			}
			else
			{
				math::float2 tg_left		( vec[i + 1].time - vec[i - 1].time, vec[i + 1].upper_value - vec[i - 1].upper_value );
				val[str]["left_tangent"]	= math::float2( vec[i].time - tg_left[0], vec[i].upper_value - tg_left[1] );
				val[str]["right_tangent"]	= math::float2( vec[i].time + tg_left[0], vec[i].upper_value + tg_left[1] );
			}

		}

		cfg->save				( configs::target_sources );
		return cfg;
}

//void create_default_spl_config	( fs_new::native_path_string file_name, memory::base_allocator* allocator )
//{
//	animation::EtCurve curve				( allocator );
//	int i = 0;
//	float La = 0.0f;
//	float spl = 60.0f;
////	curve.insert					( 0.0f, 0.0f ); 
////	curve.preInfinity				= animation::kInfinityCycle;
//	do
//	{
//		float dist = 1.0f  * i;
//		// Inverse Square Law
//		float coeff = 0.0f;
//		if ( math::is_zero( dist ))
//		{
//			coeff = 1.0f;
//		}
//		else
//		{
//			coeff = 1.0f / ( dist * dist );
//		}
//		La = 20 * log10( coeff ) + spl;
//
//		if ( La > 0.0f )
//		{
//			curve.insert					( i, La ); 
//			curve.keyList[i].type			= 3;
//		}
//		
//		i += 10;
//	} while ( La > 0.0f );
//
//
//	curve.keyList[0].type			= 3;
//	curve.calc_smooth_tangents		( );
//	
//
//	configs::lua_config_ptr config	= configs::create_lua_config( file_name.c_str() );
//	configs::lua_config_value val	= config->get_root( )["spl"];
//
//	xray::fixed_string<8> key_name;
//	for ( u32 i = 0; i < curve.keyList.size(); ++i )
//	{
//		key_name.assignf				("key%d", i);
//		pcstr str						= key_name.c_str();
//		val[str]["key_type"]			= curve.keyList[i].type;
//		val[str]["position"]			= math::float2( curve.keyList[i].time, curve.keyList[i].value );
//		val[str]["left_tangent"]		= math::float2 ( curve.keyList[i].inTanX, curve.keyList[i].inTanY );
//		val[str]["right_tangent"]		= math::float2 ( curve.keyList[i].outTanX, curve.keyList[i].outTanY );
//	}
//	//curve.save						( val );
//	config->save					( );
//}

#endif // #ifndef MASTER_GOLD

} // namespace sound
} // namespace xray
