////////////////////////////////////////////////////////////////////////////
//	Module 		: matrix4f_inline.h
//	Created 	: 24.08.2006
//  Modified 	: 24.08.2006
//	Author		: Dmitriy Iassenev
//	Description : float matrix with 4x4 components inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_MATRIX4F_INLINE_H_INCLUDED
#define CS_CORE_MATRIX4F_INLINE_H_INCLUDED

inline matrix4f& matrix4f::identity			()
{
	i.set		(1.f,0.f,0.f,0.f);
	j.set		(0.f,1.f,0.f,0.f);
	k.set		(0.f,0.f,1.f,0.f);
	c.set		(0.f,0.f,0.f,1.f);

	return		*this;
}

inline matrix4f& matrix4f::set_angles_xyz	(float const _x, float const _y, float const _z)
{
	sine_cosine	x(-_x), y(-_y), z(-_z);

#if 0
	i.set		(y.cosine*z.cosine - x.sine*y.sine*z.sine,	-x.cosine*z.sine,	x.sine*y.cosine*z.sine + y.sine*z.cosine,	0.f);
	j.set		(x.sine*y.sine*z.cosine + y.cosine*z.sine,	x.cosine*z.cosine,	y.sine*z.sine - x.sine*y.cosine*z.cosine,	0.f);
	k.set		(-x.cosine*y.sine,							x.sine,				x.cosine*y.cosine,							0.f);
	c.set		(0.f,										0.f,				0.f,										1.f);
#else // 0
	// the same as above, but less work for optimizer
	float		ysXzs = y.sine*z.sine;
	float		ysXzc = y.sine*z.cosine;
	float		ycXzs = y.cosine*z.sine;
	float		ycXzc = y.cosine*z.cosine;

	i.set		(ycXzc - x.sine*ysXzs,						-x.cosine*z.sine,	x.sine*ycXzs + ysXzc,						0.f);
	j.set		(x.sine*ysXzc + ycXzs,						x.cosine*z.cosine,	ysXzs - x.sine*ycXzc,						0.f);
	k.set		(-x.cosine*y.sine,							x.sine,				x.cosine*y.cosine,							0.f);
	c.set		(0.f,										0.f,				0.f,										1.f);
#endif // 0

	return		*this;
}

inline matrix4f& matrix4f::set_angles_xyz	(vec3f const& angles)
{
	return		set_angles_xyz(angles.x,angles.y,angles.z);
}

inline matrix4f& matrix4f::get_angles_xyz	(float& x, float& y, float& z)
{
    float		horde = _sqrt(_sqr(j.y) + _sqr(i.y));
    if (horde > 16.f*flt_eps) {
        x		= atan2(-k.y, horde);
        y		= atan2(k.x, k.z);
        z		= atan2(i.y, j.y);

		return	*this;
    }

	x			= atan2(-k.y, horde);
	y			= atan2(-i.z, i.x);
	z			= 0.f;

	return		*this;
}

inline matrix4f& matrix4f::make_scale		(float const x, float const y, float const z)
{
	identity	();

	_11			= x;
	_22			= y;
	_33			= z;

	return		*this;
}

inline matrix4f& matrix4f::make_scale		(vec3f const& scale)
{
	return		make_scale(scale.x,scale.y,scale.z);
}

inline matrix4f& matrix4f::make_translate	(float const x, float const y, float const z)
{
	identity	();

	c.set		(x,y,z,1.f);

	return		*this;
}

inline matrix4f& matrix4f::make_translate	(vec3f const& translation)
{
	return		make_translate(translation.x,translation.y,translation.z);
}

inline vec3f& matrix4f::transform_vector	(vec3f& result, vec3f const& origin) const
{
	result.x	= origin.x*_11 + origin.y*_21 + origin.z*_31 + _41;
	result.y	= origin.x*_12 + origin.y*_22 + origin.z*_32 + _42;
	result.z	= origin.x*_13 + origin.y*_23 + origin.z*_33 + _43;

	return		result;
}

inline vec3f& matrix4f::transform_direction	(vec3f& result, vec3f const& origin) const
{
	result.x	= origin.x*_11 + origin.y*_21 + origin.z*_31;
	result.y	= origin.x*_12 + origin.y*_22 + origin.z*_32;
	result.z	= origin.x*_13 + origin.y*_23 + origin.z*_33;

	return		result;
}

inline matrix4f& matrix4f::mul_43			(matrix4f const& A, matrix4f const& B)
{
	_11 		= A._11*  B._11 + A._12*  B._21 + A._13*  B._31;
	_12 		= A._11*  B._12 + A._12*  B._22 + A._13*  B._32;
	_13 		= A._11*  B._13 + A._12*  B._23 + A._13*  B._33;
	_14			= 0.f;

	_21 		= A._21*  B._11 + A._22*  B._21 + A._23*  B._31;
	_22 		= A._21*  B._12 + A._22*  B._22 + A._23*  B._32;
	_23 		= A._21*  B._13 + A._22*  B._23 + A._23*  B._33;
	_24 		= 0.f;

	_31 		= A._31*  B._11 + A._32*  B._21 + A._33*  B._31;
	_32 		= A._31*  B._12 + A._32*  B._22 + A._33*  B._32;
	_33 		= A._31*  B._13 + A._32*  B._23 + A._33*  B._33;
	_34 		= 0.f;

	_41 		= A._41*  B._11 + A._42*  B._21 + A._43*  B._31 + B._41;
	_42 		= A._41*  B._12 + A._42*  B._22 + A._43*  B._32 + B._42;
	_43 		= A._41*  B._13 + A._42*  B._23 + A._43*  B._33 + B._43;
	_44 		= 1.f;

	return		*this;
}

inline matrix4f& matrix4f::mul_44			(matrix4f const& A, matrix4f const& B)
{
	_11 		= A._11*  B._11 + A._12*  B._21 + A._13*  B._31 + A._14*  B._41;
	_12 		= A._11*  B._12 + A._12*  B._22 + A._13*  B._32 + A._14*  B._42;
	_13 		= A._11*  B._13 + A._12*  B._23 + A._13*  B._33 + A._14*  B._43;
	_14			= A._11*  B._14 + A._12*  B._24 + A._13*  B._34 + A._14*  B._44;

	_21 		= A._21*  B._11 + A._22*  B._21 + A._23*  B._31 + A._24*  B._41;
	_22 		= A._21*  B._12 + A._22*  B._22 + A._23*  B._32 + A._24*  B._42;
	_23 		= A._21*  B._13 + A._22*  B._23 + A._23*  B._33 + A._24*  B._43;
	_24 		= A._21*  B._14 + A._22*  B._24 + A._23*  B._34 + A._24*  B._44;

	_31 		= A._31*  B._11 + A._32*  B._21 + A._33*  B._31 + A._34*  B._41;
	_32 		= A._31*  B._12 + A._32*  B._22 + A._33*  B._32 + A._34*  B._42;
	_33 		= A._31*  B._13 + A._32*  B._23 + A._33*  B._33 + A._34*  B._43;
	_34 		= A._31*  B._14 + A._32*  B._24 + A._33*  B._34 + A._34*  B._44;

	_41 		= A._41*  B._11 + A._42*  B._21 + A._43*  B._31 + A._44*  B._41;
	_42 		= A._41*  B._12 + A._42*  B._22 + A._43*  B._32 + A._44*  B._42;
	_43 		= A._41*  B._13 + A._42*  B._23 + A._43*  B._33 + A._44*  B._43;
	_44 		= A._41*  B._14 + A._42*  B._24 + A._43*  B._34 + A._44*  B._44;

	return		*this;
}

inline float matrix4f::determinant_33		() const
{
	return		(
		_11*(_22*_33 - _23*_32) - 
		_12*(_21*_33 - _23*_31) +
		_13*(_21*_32 - _22*_31)
	);
}

inline float matrix4f::invert_determinant_33() const
{
	float		result = determinant_33();
	ASSERT		(!fp_zero(result), "division by zero");
	return		1.f/result;
}

inline matrix4f& matrix4f::invert_43		(matrix4f const& matrix)
{
	float		invert_determinant = matrix.invert_determinant_33();

	_11			=  invert_determinant*(matrix._22*matrix._33 - matrix._23*matrix._32);
	_12 		= -invert_determinant*(matrix._12*matrix._33 - matrix._13*matrix._32);
	_13 		=  invert_determinant*(matrix._12*matrix._23 - matrix._13*matrix._22);
	_14 		=  0.f;

	_21 		= -invert_determinant*(matrix._21*matrix._33 - matrix._23*matrix._31);
	_22 		=  invert_determinant*(matrix._11*matrix._33 - matrix._13*matrix._31);
	_23 		= -invert_determinant*(matrix._11*matrix._23 - matrix._13*matrix._21);
	_24 		=  0.f;

	_31 		=  invert_determinant*(matrix._21*matrix._32 - matrix._22*matrix._31);
	_32 		= -invert_determinant*(matrix._11*matrix._32 - matrix._12*matrix._31);
	_33 		=  invert_determinant*(matrix._11*matrix._22 - matrix._12*matrix._21);
	_34 		=  0.f;

	_41 		= -(matrix._41*_11 + matrix._42*_21 + matrix._43*_31);
	_42 		= -(matrix._41*_12 + matrix._42*_22 + matrix._43*_32);
	_43 		= -(matrix._41*_13 + matrix._42*_23 + matrix._43*_33);
	_44 		=  1.0f;

	return		*this;
}

#endif // #ifndef CS_CORE_MATRIX4F_INLINE_H_INCLUDED