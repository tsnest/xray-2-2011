////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_GRASPING_PARAMS_INLINE_H_INCLUDED
#define ANIMATION_GRASPING_PARAMS_INLINE_H_INCLUDED

namespace xray {

namespace rtp {

	inline  animation_grasping_world_params::animation_grasping_world_params(  animation::i_animation_controller &controller ): 
		animation_world_params( controller ),
		target_pos		( float4x4().identity() )
	{

	}
	inline  animation_grasping_world_params::animation_grasping_world_params( ):
		animation_world_params	(),
		target_pos ( float4x4().identity() )
	{

	}

#ifndef	MASTER_GOLD
	inline void	animation_grasping_world_params::save( xray::configs::lua_config_value config )const
	{


		config[0] = target_pos.i;
		config[1] = target_pos.j;
		config[2] = target_pos.k;
		config[3] = target_pos.c;
		
		//config[0] = controller_pos.i;
		//config[1] = controller_pos.j;
		//config[2] = controller_pos.k;
		//config[3] = controller_pos.c;



	}
#endif // MASTER_GOLD

	inline void	animation_grasping_world_params::load( const xray::configs::binary_config_value &config )
	{
 		target_pos.i = config[0] ;
 		target_pos.j = config[1] ;
 		target_pos.k = config[2] ;
 		target_pos.c = config[3] ;

	
		//controller_pos.i	=config[0];
		//controller_pos.j	=config[1];
		//controller_pos.k	=config[2];
		//controller_pos.c	=config[3];

	}


	inline void	transform_to_param	( const float4x4 &transform,  animation_grasping_space_params& param )
	{
		float4x4 itransform; 
		//ASSERT_U(  );
		itransform.try_invert( transform );
		float3 tp ( param.separate_params(0).target_position().x, 0,  param.separate_params(0).target_position().y );
		tp = itransform.transform_position( tp );
		param.separate_params(0).target_position()= float2( tp.x, tp.z );
	}

} // namespace rtp
} // namespace xray

#endif // #ifndef ANIMATION_GARSPING_PARAMS_INLINE_H_INCLUDED