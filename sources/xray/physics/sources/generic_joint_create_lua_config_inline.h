////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2011
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GENERIC_JOINT_CREATE_LUA_CONFIG_INLINE_H_INCLUDED
#define GENERIC_JOINT_CREATE_LUA_CONFIG_INLINE_H_INCLUDED

#include "joint_fixed.h"
#include "joint_ball.h"
#include "joint_euler_hinge.h"
#include "joint_hinge.h"
#include <xray/math_utils.h>
#include <xray/configs.h>

template<class ConfigValueType>
struct s_euler_axis;

template<class ConfigValueType>
inline void	read_r_axis( const ConfigValueType &cfg, s_euler_axis<ConfigValueType> &ax );

template<class ConfigValueType>
struct s_euler_axis
{
	axis	ax;
	float3	direction;
	bool	enabled;
	bool	limits_enabled;
	float	low_limit;
	float	hi_limit;
	void	clamp_limits()
	{
		math::clamp( low_limit, -math::pi, 0.f );
		math::clamp( hi_limit, 0.f, math::pi );
	}
	bool	is_free()const
	{
		return enabled && !limits_enabled;
	}
	float gap ()
	{
		return hi_limit - low_limit;
	}

	s_euler_axis( axis _ax, const float3 &_d, const ConfigValueType &cfg ): ax( _ax ), direction( _d ),  enabled( false ), limits_enabled( false ), low_limit(0), hi_limit(0)
	{
		read_r_axis( cfg, *this );

		clamp_limits();

		if( enabled && limits_enabled && ( low_limit <= math::pi && hi_limit >= math::pi ) )
			limits_enabled = false;
		if(enabled && limits_enabled && ( low_limit >= 0 && hi_limit <= 0 ) )
			enabled = false;

		if( !enabled )
				limits_enabled = false;

		if( !limits_enabled )
		{
			low_limit = 0;
			hi_limit = 0;
		}

		ASSERT( low_limit <= 0.f );
		ASSERT( low_limit >= -math::pi );

		ASSERT( hi_limit >= 0.f );
		ASSERT( hi_limit <= math::pi );

	}
}; // struct s_euler_axis

template<class ConfigValueType>
void set_axis( joint_hinge &j, const s_euler_axis<ConfigValueType> &ax )
{
	j.set_axis( ax.direction );
	j.set_high_limit( ax.hi_limit );
	j.set_low_limit( ax.low_limit );
}

template<class ConfigValueType>
void set_axis( euler_hinge &j, const s_euler_axis<ConfigValueType> &ax )
{
	j.set_axis( ax.ax, ax.direction );
	j.set_high_limit( ax.ax, ax.hi_limit );
	j.set_low_limit( ax.ax, ax.low_limit );
}

template<class ConfigValueType>
struct s_generic_joint_proccess_result
{

	joint	*result;
	body *b0, *b1; 
	s_euler_axis<ConfigValueType> x,y,z;

	void axes_reset_to_order()
	{
		x.ax = x_axis;
		y.ax = y_axis;
		z.ax = z_axis;
	}

	void create_euler()
	{
		euler_hinge *j = XRAY_NEW_IMPL( xray::physics::g_ph_allocator, euler_hinge )();
		j->connect( b0, b1 );
		axes_reset_to_order();
		set_axis( *j, x );
		set_axis( *j, y );
		set_axis( *j, z );
		result = j;
	}

	s_generic_joint_proccess_result( body *_b0, body  *_b1, const s_euler_axis<ConfigValueType> &_x, const s_euler_axis<ConfigValueType> &_y, const s_euler_axis<ConfigValueType> &_z ):
		result(0), b0(_b0), b1(_b1),  x( _x ), y( _y ), z( _z )
	{
		if( !x.enabled && !y.enabled && !z.enabled )
		{
			joint_fixed *j = XRAY_NEW_IMPL(xray::physics::g_ph_allocator, joint_fixed )();
			j->connect( b0, b1 );
			result = j;
			return;
		}
		if( x.is_free() && y.is_free() && z.is_free() )
		{
			joint_ball *j = XRAY_NEW_IMPL(xray::physics::g_ph_allocator, joint_ball )();
			j->connect( b0, b1 );
			result = j;
			return;
		}
		u8 num_enabled = u8(x.enabled) + u8(y.enabled) + u8(z.enabled);
		ASSERT( num_enabled <= 3 );
		math::float_sorted< s_euler_axis<ConfigValueType> > sorted( x, y, z, x.gap(), y.gap(), z.gap() );
		if( num_enabled == 1 )
		{
			ASSERT( sorted.r0().enabled );
			ASSERT( !sorted.r1().enabled );
			ASSERT( !sorted.r2().enabled );
			joint_hinge *j = XRAY_NEW_IMPL(xray::physics::g_ph_allocator, joint_hinge )();
			j->connect( b0, b1 );
			set_axis( *j, sorted.r0() );
			result = j;
			return;
		}

		if( num_enabled == 2 )
		{
			if( !x.enabled )		
				x = y;
			else if( !z.enabled )
				z = y;
			//ASSERT( !y.enabled );
			ASSERT( x.enabled );
			ASSERT( z.enabled );
			euler_hinge *j = XRAY_NEW_IMPL(xray::physics::g_ph_allocator, euler_hinge )();
			j->connect( b0, b1 );
			create_euler();
			return;
		}

		ASSERT( num_enabled == 3 );
		s_euler_axis<ConfigValueType> tmp_x = x, tmp_y = y, tmp_z = z;
		if( sorted.r2().ax == x_axis )
		{
			y = tmp_x;
			z = tmp_y;
			x = tmp_z;
		} else if( sorted.r2().ax == z_axis )
		{
			y = tmp_z;
			x = tmp_y;
			z = tmp_x;
		}

		math::clamp( y.hi_limit, 0.f, math::pi_d2 );
		math::clamp( y.low_limit, -math::pi_d2, 0.f );
		create_euler();
		return;

	}
}; // struct s_generic_joint_proccess_result

template<class ConfigValueType>
inline void	read_r_axis( const ConfigValueType &cfg, s_euler_axis<ConfigValueType> &ax )
{
	//ax.ax = num
	ASSERT( ax.ax != none_axis );

	ax.enabled			= cfg["enable_state"];
	ax.limits_enabled	= cfg["limits_enable_state"];
	if( !ax.enabled || !ax.limits_enabled )
		return;
	if(ax.ax == z_axis )
	{
		ax.low_limit = cfg["low_limit"] ;
		ax.hi_limit  = cfg["hi_limit"];
	} else {
		ax.low_limit = -float( cfg["hi_limit"] ) ;
		ax.hi_limit  = -float( cfg["low_limit"] ) ;
	}

}


//static void	set_r_axis(const xray::configs::lua_config_value &cfg, euler_hinge &joint, axis num )
//{
//	//joint.set_axis( num, transform.i.xyz() );
//	if( !cfg["enable_state"] )
//	{
//		joint.set_low_limit( num, 0 );
//		joint.set_high_limit( num, 0 );
//		return;
//	}
//
//	if( cfg["limits_enable_state"] )
//	{
//		if( num==z_axis )
//		{
//			joint.set_low_limit( num, cfg["low_limit"] );
//			joint.set_high_limit( num, cfg["hi_limit"] );
//		}else{
//			joint.set_low_limit( num, -float( cfg["hi_limit"] ) );
//			joint.set_high_limit( num, -float( cfg["low_limit"] ) );
//		}
//	}
//
//}

template<class ConfigValueType>
inline joint* new_generic_joint( const ConfigValueType &cfg, body* b0, body* b1, const float4x4 &transform  )
{
	//euler_hinge*	res =  NEW(euler_hinge)();
	//res->connect( b0, b1 );
	s_generic_joint_proccess_result<ConfigValueType> res ( b0, b1, 
		s_euler_axis<ConfigValueType> ( x_axis, transform.i.xyz(), cfg["x_rotate"] ),
		s_euler_axis<ConfigValueType> ( y_axis, transform.j.xyz(), cfg["y_rotate"] ),
		s_euler_axis<ConfigValueType> ( z_axis, transform.k.xyz(), cfg["z_rotate"] )
	);
	
	//res->set_axis( x_axis, transform.i.xyz() );
	//res->set_axis( z_axis, transform.k.xyz() );

	//set_r_axis( cfg["x_rotate"], *res, x_axis );
	//set_r_axis( cfg["y_rotate"], *res, y_axis );
	//set_r_axis( cfg["z_rotate"], *res, z_axis );
	//res->set_low_limit( 0, 

	return	res.result;

}

#endif // #ifndef GENERIC_JOINT_CREATE_LUA_CONFIG_INLINE_H_INCLUDED