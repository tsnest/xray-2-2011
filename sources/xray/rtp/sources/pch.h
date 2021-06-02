////////////////////////////////////////////////////////////////////////////
//	Created		: 23.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_RTP_BUILDING

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#define XRAY_LOG_MODULE_INITIATOR	"rtp"
#include <xray/extensions.h>

#include "rtp_memory.h"

namespace xray {
	namespace render {
		namespace debug {
			class renderer;
		} // namespace debug
	} // namespace render
} //  namespace xray 



#endif // #ifndef PCH_H_INCLUDED