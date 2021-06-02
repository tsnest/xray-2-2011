////////////////////////////////////////////////////////////////////////////
//	Created		: 24.04.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef JOINT_HINGE_H_INCLUDED
#define JOINT_HINGE_H_INCLUDED

#include "joint_single_base_impl.h"

class joint_hinge :
	public joint_single_base_impl
{
public:
					joint_hinge	( );
	virtual			~joint_hinge( );	
		
			void	set_axis		( const float3 &axis );
			void	set_angle		( float angle );

			void	set_low_limit	( float limit );
			void	set_high_limit	( float limit );
			float	get_low_limit	( )const;
			float	get_high_limit	( )const;
			
			void	set_stop_spring	( float spring );
			void	set_stop_damping( float damping );

			float	get_stop_spring	( );
			float	get_stop_damping( );

			void	set_normal_spring( float spring );
			void	set_normal_damping( float dumping );

			float	get_normal_spring( );
			float	get_normal_damping( );

private:
	virtual	void	set_anchor  ( const float3	&anchor );
	virtual	void	get_anchor  ( float3	&anchor )const;


private:

}; // class joint_hinge

#endif // #ifndef JOINT_HINGE_H_INCLUDED