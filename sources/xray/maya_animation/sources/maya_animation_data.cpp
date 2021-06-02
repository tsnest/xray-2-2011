////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
//#include "maya_animation_data.h"
//#include "animation_data.h"
//#include <xray/fs_utils.h>
//
//using xray::maya_animation::animation_data;
//using xray::maya_animation::animation_curve_data_header;
//
//bool animation_data::load( const char* fn )
//{
//	XRAY_UNREFERENCED_PARAMETERS	( fn );
//	UNREACHABLE_CODE				( );
//	//FILE*				f;
//	//bool res			= fs_new::open_file( &f, fs::open_file_read, fn );
//	//
//	//// read first track
//	//if ( !res )
//	//	return			false;
//	//
//	//animation_curve_data_header			header;
//	//fread				(&header, sizeof(header), 1, f);
//	//
//	//if( header.type != maya_spline )
//	//{	
//	//		fclose		( f );
//	//		return false;
//	//}
//
//	//u32 count			= 0;
//	//fread				(&count, sizeof(count), 1, f);
//	//m_bone_animations.resize(count);
//	//for( u32 i=0; i<count; ++i )
//	//	m_bone_animations[i].load( f );
//
//	//fclose				( f );
//	return				true;
//}
//
//void	animation_data::clear	( )
//{
//	m_bone_animations.clear();
//}
//
//void	animation_data::bone_pose	( xray::math::float4x4 &pose, u32 id, float time )
//{
//	m_bone_animations[id].evaluate( pose, time );
//}
//
//float	animation_data::min_time	()
//{
//	float ret = math::infinity;
//	const u32 count = m_bone_animations.size();
//	for( u32 i=0; i<count; ++i )
//		ret = math::min( ret, m_bone_animations[i].min_time() );
//	return ret;
//}
//float	animation_data::max_time	()
//{
//	float ret = -math::infinity;
//	const u32 count = m_bone_animations.size();
//	for( u32 i=0; i<count; ++i )
//		ret = math::max( ret, m_bone_animations[i].max_time() );
//	return ret;
//}
//
//float	animation_data::evaluate_mean_time	()
//{
//	const u32 count = m_bone_animations.size();
//	u32 cnt_eval = 0;
//	float time_eval = 0;
//	for( u32 i=0; i<count; ++i )
//	{
//		cnt_eval	+= m_bone_animations[i].m_num_evaluate;
//		time_eval	+= m_bone_animations[i].m_evaluate_time;
//	}
//	if(cnt_eval==0)
//		return 0;
//	return time_eval/cnt_eval;
//}
//
//
//
//
