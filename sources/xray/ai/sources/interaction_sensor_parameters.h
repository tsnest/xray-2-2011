////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef INTERACTION_SENSOR_PARAMETERS_H_INCLUDED
#define INTERACTION_SENSOR_PARAMETERS_H_INCLUDED

namespace xray {
namespace ai {

struct interaction_sensor_parameters
{
			interaction_sensor_parameters	( );

	void	deserialize						( configs::binary_config_value const& options );

	bool	enabled;
}; // struct interaction_sensor_parameters

} // namespace ai
} // namespace xray

#endif // #ifndef INTERACTION_SENSOR_PARAMETERS_H_INCLUDED