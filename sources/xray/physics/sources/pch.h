////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Description : precompiled header
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_PHYSICS_BUILDING

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#define XRAY_LOG_MODULE_INITIATOR	"physics"
#include <xray/extensions.h>

#include "physics_memory.h"

using namespace xray;

namespace xray {
namespace physics {

	enum axis
	{
		x_axis = 0,
		y_axis,
		z_axis,
		none_axis	
	};

}
}

using  xray::physics::axis;
using  xray::physics::x_axis;
using  xray::physics::y_axis;
using  xray::physics::z_axis;
using  xray::physics::none_axis;
#endif // #ifndef PCH_H_INCLUDED