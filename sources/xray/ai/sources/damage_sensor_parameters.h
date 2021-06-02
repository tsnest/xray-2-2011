////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef DAMAGE_SENSOR_PARAMETERS_H_INCLUDED
#define DAMAGE_SENSOR_PARAMETERS_H_INCLUDED

namespace xray {
namespace ai {

struct damage_sensor_parameters
{
			damage_sensor_parameters	( );

	void	deserialize					( configs::binary_config_value const& options );

	bool	enabled;
}; // struct damage_sensor_parameters

} // namespace ai
} // namespace xray

#endif // #ifndef DAMAGE_SENSOR_PARAMETERS_H_INCLUDED