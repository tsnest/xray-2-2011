////////////////////////////////////////////////////////////////////////////
//	Created		: 31.10.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

// This file is available in electronic form at http://www.psa.es/sdg/sunpos.htm

#ifndef XRAY_RENDER_ENGINE_SUN_POSITION_H_INCLUDED
#define XRAY_RENDER_ENGINE_SUN_POSITION_H_INCLUDED

namespace xray {
namespace render {

// Declaration of some constants 
#define pi    3.14159265358979323846
#define twopi (2*pi)
#define rad   (pi/180)
#define dEarthMeanRadius     6371.01	// In km
#define dAstronomicalUnit    149597890	// In km

struct cTime
{
	int iYear;
	int iMonth;
	int iDay;
	double dHours;
	double dMinutes;
	double dSeconds;
};

struct cLocation
{
	double dLongitude;
	double dLatitude;
};

struct cSunCoordinates
{
	double dZenithAngle;
	double dAzimuth;
};

void calculate_sun_position(cTime udtTime, cLocation udtLocation, cSunCoordinates *udtSunCoordinates);

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_SUN_POSITION_H_INCLUDED