////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.02.2008
//	Author		: Konstantin Slipchenko
//	Description : integration constants
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_MATH_UTILS_H_INCLUDED
#define XRAY_PHYSICS_MATH_UTILS_H_INCLUDED

inline float erp( float k_p,float k_d,float	s )//s=fixed_step		
{
	return ((s*(k_p)) / (((s)*(k_p)) + (k_d)));
}

inline float cfm( float k_p,float k_d,float	s )//s=fixed_step
{	
	return (1.f / (((s)*(k_p)) + (k_d)));
}

inline float spring( float cfm, float erp, float s )//s=fixed_step
{
	return ((erp)/(cfm)/s);
}

inline float damping( float cfm, float erp )
{
	return ((1.f-(erp))/(cfm));
}

inline void	 mul_spring_damping( float &cfm, float	&erp, float mul_spring, float mul_damping )
{
	float factor= 1.f/( mul_spring*erp+mul_damping*( 1-erp ));
	cfm*=factor;
	erp*=( factor*mul_spring );
}

#endif
