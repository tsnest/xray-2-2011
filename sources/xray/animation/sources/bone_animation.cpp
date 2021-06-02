////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/bone_animation.h>
#include "bi_spline_bone_animation.h"

using namespace xray::math;

namespace xray {
namespace animation {

u32		bone_animation::count_internal_memory_size( const bi_spline_bone_animation	&bd )
{
	u32 size = 0;

	for ( u32  ch = channel_translation_x; ch < channel_max; ++ch )
	{
		bi_spline_bone_animation::animation_channel_data const& channel = bd.channel( enum_channel_id ( ch ) );
		
		size += animation_curve_type::count_internal_memory_size( channel );
	}
	return size;
}

void	bone_animation::create_internals_in_place( const bi_spline_bone_animation	&bd, void* memory )
{
					////const bi_spline_data< float > &b_spline = data.bone( idx[bone] ).channel( enum_channel_id (ch) ).bi_spline();
	
	for ( u32 ch = channel_translation_x; ch < channel_max; ++ch )
	{
		////const bi_spline_data< float > &b_spline = data.bone( idx[bone] ).channel( enum_channel_id (ch) ).bi_spline();

		const bi_spline_data< float > &b_spline = bd.channel( enum_channel_id (ch) );
		
		channel( enum_channel_id (ch) ).create_in_place_internals( b_spline, memory );

		memory = get_shift_ptr ( memory, animation_curve_type::count_internal_memory_size( b_spline ) );
	}

}

frame	bone_animation::bone_frame	( float const time, current_frame_position &frame_position ) const
{
	frame					result;
	evaluate_frame			( time, m_channels, result, frame_position );
	return					result;
}

void	bone_animation::bone_matrix( float time, const float3 &parent_scale_, float4x4 &matrix, float3 &scale_, current_frame_position &frame_pos )const
{
	frame f;
	evaluate_frame( time, m_channels, f, frame_pos );
	frame_matrix( f, matrix, parent_scale_, scale_ );
}

} // namespace animation
} // namespace xray