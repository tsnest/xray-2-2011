////////////////////////////////////////////////////////////////////////////
//	Created		: 23.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RTP_ENGINE_H_INCLUDED
#define XRAY_RTP_ENGINE_H_INCLUDED

namespace xray {

namespace animation {
	struct world;
} // namespace animation 

namespace rtp {

struct DECLSPEC_NOVTABLE engine {
		virtual	animation::world&	get_animation_world		( ) = 0;
}; // class engine

} // namespace rtp
} // namespace xray

#endif // #ifndef XRAY_RTP_ENGINE_H_INCLUDED