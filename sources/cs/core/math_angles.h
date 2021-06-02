////////////////////////////////////////////////////////////////////////////
//	Module 		: math_angles.h
//	Created 	: 28.08.2006
//  Modified 	: 28.08.2006
//	Author		: Dmitriy Iassenev
//	Description : angles functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_MATH_ANGLES_H_INCLUDED
#define CS_CORE_MATH_ANGLES_H_INCLUDED

#ifdef CS_CORE_BUILDING
#	include <cs/gtl/script_export_macroses.h>
#endif // #ifdef CS_CORE_BUILDING

inline float	angle_normalize_always	(float const angle);
inline float	angle_normalize			(float const angle);
inline float	angle_normalize_signed	(float const angle);
inline bool		angle_lerp				(float& current, float const target, float const speed, float const time_delta);
inline float	angle_difference		(float const _0, float const _1);

#ifdef CS_CORE_BUILDING
	struct dummy;
	typedef gtl::class_script_exporter<dummy>	gtl__class_script_exporter_dummy_;
	CS_TYPE_LIST_PUSH_FRONT				(gtl__class_script_exporter_dummy_)
#	undef CS_SCRIPT_EXPORT_TYPE_LIST
#	define CS_SCRIPT_EXPORT_TYPE_LIST		CS_SAVE_TYPE_LIST(gtl__class_script_exporter_dummy_)
#endif // #ifdef CS_CORE_BUILDING

#include <cs/core/math_angles_inline.h>

#endif // #ifndef CS_CORE_MATH_ANGLES_H_INCLUDED