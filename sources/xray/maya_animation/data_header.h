////////////////////////////////////////////////////////////////////////////
//	Created		: 22.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DATA_HEADER_H_INCLUDED
#define DATA_HEADER_H_INCLUDED

namespace xray {
namespace maya_animation {

enum animation_curve_data_type
{
	maya_spline,
	channels_equidistant_frames,
	equidistant_frames
};

struct animation_curve_data_header
{
	
	animation_curve_data_header(): type(maya_spline), equidistant_frames_frequency(1/30)
	{}

	animation_curve_data_type	type;
	float						equidistant_frames_frequency;

};

} // namespace maya_animation
} // namespace xray

#endif // #ifndef DATA_HEADER_H_INCLUDED