////////////////////////////////////////////////////////////////////////////
//	Module 		: math_angles_inline.h
//	Created 	: 28.08.2006
//  Modified 	: 28.08.2006
//	Author		: Dmitriy Iassenev
//	Description : angles functions inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_MATH_ANGLES_INLINE_H_INCLUDED
#define CS_CORE_MATH_ANGLES_INLINE_H_INCLUDED

inline float angle_normalize_always	(float const angle)
{
	float			division = angle/PI_MUL_2;
	int				round = (division > 0) ? i_floor(division) : i_ceil(division);
	float			fraction = division - round;
	if (fraction < 0)
		fraction	+=	1.f;

	return			fraction*PI_MUL_2;
}

inline float angle_normalize		(float const angle)
{
	if ((angle >= 0) && (angle <= PI_MUL_2))
		return		angle;

	return			angle_normalize_always(angle);
}

inline float angle_normalize_signed	(float const angle)
{
	if ((angle >= -PI) && (angle <= PI))
		return		angle;

	float			result = angle_normalize_always(angle);
	if (result > PI)
		result		-= PI_MUL_2;

	return			result;
}

inline bool angle_lerp				(float& current, float const target, float const speed, float const time_delta)
{
	float			difference = angle_normalize_signed(target - current);
	if (fp_zero(difference))
		return		true;

	float			motion = speed*time_delta;
	float			absolute_difference = _abs(difference);
	if (motion > absolute_difference)
		motion		= absolute_difference;

	if (difference > 0)
		current		+= motion;
	else
		current		-= motion;

	if (current < 0.f) {
		current		+= PI_MUL_2;
		return		false;
	}

	if (current < PI_MUL_2)
		return		false;
	
	current			-= PI_MUL_2;
	return			false;
}

inline float angle_difference		(float const _0, float const _1)
{
	float			difference = angle_normalize(_0 - _1);
	if (difference <= PI)
		return		difference;

	return			PI_MUL_2 - difference;
}

#endif // #ifndef CS_CORE_MATH_ANGLES_INLINE_H_INCLUDED