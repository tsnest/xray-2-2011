////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef I_ANIMATION_CONTROLLER_SET_H_INCLUDED
#define I_ANIMATION_CONTROLLER_SET_H_INCLUDED

namespace xray {
namespace animation {

class i_animation_action;

class i_animation_controller_set {

public:
	virtual i_animation_action const	*target_action			()const = 0;
	virtual	void						get_target_transform	( float4x4 &m )const = 0;
	virtual	u32							bone_index				( pcstr bone_name )const = 0;
	virtual i_animation_action const	*action					( u32 id )const = 0;
	virtual u32							num_actions				( )const = 0;
	virtual void						set_callback_on_loaded	( const boost::function< void ( ) >	 &cb  ) = 0;
	virtual	bool						loaded					()const = 0;
	virtual	pcstr						path					() const = 0;
//	
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( i_animation_controller_set )
}; // class i_animation_controller_set


} // namespace animation
} // namespace xray

#endif // #ifndef I_ANIMATION_CONTROLLER_SET_H_INCLUDED