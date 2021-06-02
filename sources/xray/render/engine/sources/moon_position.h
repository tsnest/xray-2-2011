////////////////////////////////////////////////////////////////////////////
//	Created		: 31.10.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_MOON_POSITION_H_INCLUDED
#define XRAY_RENDER_ENGINE_MOON_POSITION_H_INCLUDED

namespace xray {
namespace render {

struct time_parameters
{
	s32		year;
	s32		month;
	s32		day;
	s32		hours;
	s32		minutes;
	s32		seconds;
}; // struct time_parameters

void calculate_moon_position(time_parameters const& params, double in_latitude, double in_longitude, double& out_azimuth, double& out_zenith);

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_MOON_POSITION_H_INCLUDED
