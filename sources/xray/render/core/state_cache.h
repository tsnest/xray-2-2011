////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STATE_CACHE_H_INCLUDED
#define STATE_CACHE_H_INCLUDED

namespace xray {
namespace render {

template <class IDeviceState, class state_desc>
class state_cache
{
public:
	state_cache();
	~state_cache();

	void			clear_state_array();

	IDeviceState*	get_state( state_desc const & desc );

private:
	struct state_record 
	{
		u32						crc;
		IDeviceState*			state;
	};

private:
	void			create_state	( state_desc desc, IDeviceState** ppIState );
	IDeviceState*	find			( state_desc const & desc, u32 CRC );

private:
	//	This must be cleared on device destroy
	render::vector<state_record>	states;
};

} // namespace render
} // namespace xray

#include <xray/render/core/state_cache_inline.h>

#endif // #ifndef STATE_CACHE_H_INCLUDED
