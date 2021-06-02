////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DISCRETE_DATA_H_INCLUDED
#define DISCRETE_DATA_H_INCLUDED

namespace xray {

namespace animation {
	struct frame;
	enum enum_channel_id;
} // namespace animation

namespace configs{
	class lua_config_value;
} // namespace configs

namespace maya_animation {

class discrete_data {

public:
	virtual			~discrete_data(){}

public:
	virtual	void	save	( xray::configs::lua_config_value & cfg ) const									= 0;
	virtual	void	load	( xray::configs::lua_config_value const& cfg, bool bone_names_only = false )	= 0;
	virtual	void	check	( xray::configs::lua_config_value &discrete_data ) const						= 0;

public:
	virtual void	add_frame		( u32 bone, animation::frame const& frm, animation::frame const& base_frm )	= 0;
	virtual	void	calculate		( )																			= 0;

	virtual void	get_frame		( u32 bone, u32 frame_number, float &time, animation::frame& frame )const	= 0;
	virtual	float	*channel		( u32 bone,  animation::enum_channel_id ch )								= 0;
	virtual u32		channel_size	( u32 bone,  animation::enum_channel_id ch )								= 0;
	virtual u32		bone_count		( )const																	= 0;
	virtual u32		frame_count		( )const																	= 0;
	virtual void	set_bone_count	( u32 bc )																	= 0;
	virtual float	precision		( u32 bone, animation::enum_channel_id channel )const						= 0;
	virtual float	time_scale		( )const																	= 0;
	virtual void	set_precision	( float const t, float const r, float const s )								= 0;

}; // class discrete_data

} // namespace maya_animation
} // namespace xray

#endif // #ifndef DISCRETE_DATA_H_INCLUDED