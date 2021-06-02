////////////////////////////////////////////////////////////////////////////
//	Created		: 31.05.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef NAVIGATION_PARAMS_H_INCLUDED
#define NAVIGATION_PARAMS_H_INCLUDED
#include "space_params.h"
#include "animation_action_params.h"
#include <xray/animation/i_animation_action.h>

namespace xray {
namespace rtp {

struct navigation_params;
struct navigation_tree_params;

class navigation_action;
class navigation_theta_action;

struct navigation_tree_params:
	public tree_space_param< 2, navigation_tree_params >
{
	
	typedef	tree_space_param< 2, navigation_tree_params > super;
	typedef	navigation_action	action_type;

			navigation_tree_params	(float x,  float theta );
			navigation_tree_params	( ){};
			void	render					( const navigation_tree_params&, xray::render::scene_ptr const&, xray::render::debug::renderer& , math::color&   )const;
	
	float			&x		()										{ return (*this)[0]; }
	float			&theta	()										{ return (*this)[1]; }
	const	float	x		()								const	{ return (*this)[0]; }
	const	float	theta	()								const	{ return (*this)[1]; }
			bool	similar	( const navigation_tree_params& r )	const	{ return super::similar( r ); }

}; // class navigation_params

struct navigation_params:
		public space_param< 1, 1,  navigation_tree_params, navigation_params >
{

	typedef space_param< 1, 1,  navigation_tree_params, navigation_params > super;

	void	render				( const navigation_params&,  xray::render::debug::renderer& , u32   )const;
	
	float			&x		()										{ return separate_params( 0 ).x(); }
	float			&theta	()										{ return separate_params( 0 ).theta(); }
	const	float	x		()								const	{ return separate_params( 0 ).x(); }
	const	float	theta	()								const	{ return separate_params( 0 ).theta(); }
			bool	similar	( const navigation_params& r )	const	{ return super::similar( r ); }

};

struct navigation_tree_theta_params;
struct navigation_theta_params;

struct navigation_tree_theta_params:
	public tree_space_param< 1, navigation_tree_theta_params >
{
	
	typedef	tree_space_param< 1, navigation_tree_theta_params > super;

	typedef	navigation_theta_action								action_type;

			navigation_tree_theta_params	( float theta );
			navigation_tree_theta_params	( ){};
			void	render					( const navigation_theta_params&, xray::render::scene_ptr const&, xray::render::debug::renderer&, math::color const&   )const{};
	
	float				&angle	()													{ return (*this)[0]; }
	const	float		angle	()											const	{ return (*this)[0]; }
			bool		similar	( const navigation_tree_theta_params& r )	const	{ return super::similar( r ); }

}; // class navigation_params

struct navigation_theta_params:
		public space_param< 2, 2,  navigation_tree_theta_params, navigation_theta_params >
{
	typedef space_param< 2, 2,  navigation_tree_theta_params, navigation_theta_params > super;


	navigation_theta_params( float theta, float tau );
	navigation_theta_params( );

	void	render					( const navigation_theta_params&, xray::render::scene_ptr const&, xray::render::debug::renderer& , math::color const&   )const;

	animation::e_gait	gait	()										const	{ return (animation::e_gait )(discrete_value	( )); }
	void				set_gait( animation::e_gait v)							{  discrete_value	( ) = v; }

	//animation::e_gait	gait	()										const	{ return animation::gait_walk; }
	//void				set_gait( animation::e_gait	 )							{ discrete_value	( ) = animation::gait_walk;  }

	float				&theta_ro()												{ return separate_params( 0 ).angle(); }
	const	float		theta_ro()										const	{ return separate_params( 0 ).angle(); }

	float				&tau	()												{ return separate_params( 1 ).angle(); }
	const	float		tau		()										const	{ return separate_params( 1 ).angle(); }

//	float				&phy	()												{ return separate_params( 1 ).angle(); }
//	const	float		phy		()										const	{ return separate_params( 1 ).angle(); }

			bool		similar	( const navigation_theta_params& r )	const	{ return super::similar( r ); }
};

struct navigation_world_base_params:
	public animation_world_params
{
	navigation_world_base_params	(): animation_world_params(), m_path( 1, 0 ), m_torso( 1, 0 ), m_gait( animation::gait_walk ), m_jump( false )  {}
	navigation_world_base_params	( animation::i_animation_controller& controller ): animation_world_params( controller ), m_path( 1, 0 ), m_torso( 1, 0 ), m_gait( animation::gait_walk ), m_jump( false ){}
	
	void		get_local_params	( const navigation_world_base_params& action_start_params, float &p, float &angle, float &tau )const;
	float4x4	head_matrix			()const;

	inline	 bool				jump		()const						{ return m_jump; }
	inline 	animation::e_gait	gait		()const						{ return m_gait; }
	inline 	float2 const &		path		()const						{ return m_path; }
	inline 	float2 const &		torso		()const						{ return m_torso; }

	inline void	set_jump	( bool v )					{ m_jump = v; }
	inline void	set_gait	( animation::e_gait v )		{ m_gait = v; }
	inline void	set_torso	( float2 const& v )			{ m_torso = v; }
	inline void	set_path	( float2 const& v )			{ m_path = v; }



public:
	static		animation::e_gait path_length_to_gait( float2 const & path, bool jump );

private:
	
	float2				m_path;
	float2				m_torso;
	
	bool				m_jump;
	
	animation::e_gait	m_gait;
};

struct navigation_world_params:
	public navigation_world_base_params
{
		navigation_world_params(): vel( 0 ){};
		navigation_world_params	( animation::i_animation_controller& controller ): navigation_world_base_params( controller ), vel( 0 ){}

		void	from_world		( const navigation_world_params& action_start_params, navigation_params &p )const;

		float vel;
};

struct navigation_theta_world_params:
	public navigation_world_base_params
{
		navigation_theta_world_params	() {}
		navigation_theta_world_params	( animation::i_animation_controller& controller ): navigation_world_base_params( controller ){}
		
		void	from_world		( const navigation_theta_world_params& action_start_params, navigation_theta_params &p )const;

};

inline float look_angle_in_root( float4x4 const&  head_in_root )
{
	float3 look_dir_in_root = head_in_root.k.xyz(); //-
	look_dir_in_root.y = 0; //ignore lookup / lookdown
	look_dir_in_root = look_dir_in_root.normalize();
	float sin = look_dir_in_root.x;
	float cos = look_dir_in_root.z;
	float local_look_angle =  math::angle_normalize_signed( atan2( sin, -cos ) );
	return local_look_angle;
}

} // namespace rtp
} // namespace xray

#endif // #ifndef NAVIGATION_PARAMS_H_INCLUDED
