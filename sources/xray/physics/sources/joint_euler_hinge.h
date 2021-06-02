////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef JOINT_EULER_HINGE_H_INCLUDED
#define JOINT_EULER_HINGE_H_INCLUDED

#include "joint_double_base_impl.h"

struct dxJoint;
class euler_hinge:
	public joint_double_base_impl
{

inline	dxJoint*			angular_motor	( )			{ return m_joint0; }
inline	dxJoint*			ball			( )			{ return m_joint1; }
inline	const dxJoint*		angular_motor	( ) const	{ return m_joint0; }
inline	const dxJoint*		ball			( ) const	{ return m_joint1; }

public:
					euler_hinge		( );
		virtual		~euler_hinge	( );

			void	set_axis		( axis axis_num, const float3 &axis );
			void	set_angle		( axis axis_num, float angle );

			void	set_low_limit	( axis axis_num, float limit );
			void	set_high_limit	( axis axis_num, float limit );
			float	get_low_limit	( axis axis_num )const;
			float	get_high_limit	( axis axis_num )const;
			
			void	set_stop_spring	( axis axis_num, float spring );
			void	set_stop_damping( axis axis_num , float damping );

			float	get_stop_spring	( axis axis_num );
			float	get_stop_damping( axis axis_num );

			void	set_normal_spring( axis axis_num, float spring );
			void	set_normal_damping( axis axis_num , float dumping );

			float	get_normal_spring( axis axis_num );
			float	get_normal_damping( axis axis_num );

private:
	virtual	void	set_anchor  ( const float3	&anchor );
	virtual	void	get_anchor  ( float3	&anchor )const;

}; // class euler_hinge

#endif // #ifndef JOINT_EULER_HINGE_H_INCLUDED