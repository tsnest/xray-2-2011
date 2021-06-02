////////////////////////////////////////////////////////////////////////////
//	Module 		: math_extensions.h
//	Created 	: 27.08.2006
//  Modified 	: 27.08.2006
//	Author		: Dmitriy Iassenev
//	Description : math extensions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_MATH_EXTENSIONS_H_INCLUDED
#define CS_CORE_MATH_EXTENSIONS_H_INCLUDED

#include <cs/core/math_constants.h>
using cs::core::math::EPS_3;
using cs::core::math::EPS_5;
using cs::core::math::EPS_7;
using cs::core::math::PI;
using cs::core::math::PI_MUL_2;
using cs::core::math::PI_MUL_4;
using cs::core::math::PI_DIV_2;
using cs::core::math::PI_DIV_4;
using cs::core::math::PI_DIV_180;
using cs::core::math::_180_DIV_PI;

#include <cs/core/math_functions.h>
using cs::core::math::fp_zero;
using cs::core::math::fp_similar;
using cs::core::math::_abs;
using cs::core::math::_abs;
using cs::core::math::_sqrt;
using cs::core::math::_sin;
using cs::core::math::_cos;
using cs::core::math::_sincos;
using cs::core::math::_sqr;
using cs::core::math::_min;
using cs::core::math::_min;
using cs::core::math::_max;
using cs::core::math::_max;
using cs::core::math::i_floor;
using cs::core::math::i_ceil;
using cs::core::math::deg2rad;
using cs::core::math::rad2deg;
using cs::core::math::clampr;
using cs::core::math::clamp;
using cs::core::math::sine_cosine;

#include <cs/core/math_angles.h>
#include <cs/core/vec2f.h>

#include <cs/core/vec3f.h>
using cs::core::math::vec3f;

#include <cs/core/vec4f.h>
using cs::core::math::vec4f;

#include <cs/core/sphere3f.h>
using cs::core::math::sphere3f;

#include <cs/core/matrix4f.h>
#include <cs/core/aabb2f.h>
#include <cs/core/aabb3f.h>
#include <cs/core/plane3f.h>

#endif // #ifndef CS_CORE_MATH_EXTENSIONS_H_INCLUDED