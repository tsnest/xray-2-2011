////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef I_ANIMATION_ACTION_H_INCLUDED
#define I_ANIMATION_ACTION_H_INCLUDED

namespace xray {

namespace config {

}

namespace render {
	class scene_renderer;

namespace debug {
	class renderer;
} // namespace debug
} // namespace render

namespace animation {

enum e_gait
{	
	gait_stand	=0, 
	gait_walk,
	gait_walk_fast, 
	gait_run, 
	gait_walk_jump,
	gait_take,
	
	gait_enum_end
};


enum e_group_flag
{	
	group_flag_idle			=	1 << 0,
	group_flag_take			=	1 << 1,
	group_flag_strafe		=	1 << 2,
	group_flag_step_left	=	1 << 3,
	group_flag_step_right	=	1 << 4,

	group_flag_enum_end		=	1 << 5
};

class i_animation_controller;

class i_animation_action {

public:
	//virtual void	get_displacement_transform	( float4x4 &m,  const buffer_vector< float > &weights )const = 0;
	virtual void	get_displacement_transform	( float4x4 &m,  const buffer_vector< float > &weights, u32 from_action_id, const buffer_vector< float > &from_weights ) const = 0;
	virtual	float	mean_velocity				( const buffer_vector< float > &weights )const = 0;
	virtual	float	max_weight					( u32 id_animation ) const = 0;
	virtual	u32		animations_number			( )const = 0;
	virtual float	duration					( )const = 0;
	virtual	float	run							( i_animation_controller *set, const buffer_vector< float > &from_weights, const buffer_vector< float > &weights, u32 step, float time )const =0;
	virtual	u32		get_id						( )const = 0;
	virtual	e_gait	gait						( )const = 0;
	virtual u32		flags						( )const = 0;
	virtual	u32		bone_index					( pcstr bone_name )const =0;
	virtual void	local_bone_matrix			( float4x4 &m,  u32 bone_idx,  const buffer_vector< float > &weights )const =0;
	virtual pcstr	name						( )const = 0;

	virtual	configs::binary_config_ptr&			config( )const = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( i_animation_action )

}; // class i_animation_action

} // namespace animation
} // namespace xray

#endif // #ifndef I_ANIMATION_ACTION_H_INCLUDED