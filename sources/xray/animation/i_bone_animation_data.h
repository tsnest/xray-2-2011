////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef I_BONE_ANIMATION_DATA_H_INCLUDED
#define I_BONE_ANIMATION_DATA_H_INCLUDED

#include <xray/animation/i_animation_channel_data.h>
#include <xray/animation/anim_track_common.h>

namespace xray {
namespace animation {

class i_animation_channel_data;

struct i_bone_animation_data {
public:
	virtual void							save		( xray::configs::lua_config_value config  )const		= 0;
	virtual void							load		( xray::configs::lua_config_value const& config  )		= 0;

public:
	virtual	i_animation_channel_data		&channel	( xray::animation::enum_channel_id ch )					= 0;
	virtual	const i_animation_channel_data	&channel	( xray::animation::enum_channel_id ch )const			= 0;
	virtual	bool							compare		( i_bone_animation_data const &d )const					= 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( i_bone_animation_data )
}; // class i_bone_animation_data

} // namespace animation
} // namespace xray

#endif // #ifndef I_BONE_ANIMATION_DATA_H_INCLUDED