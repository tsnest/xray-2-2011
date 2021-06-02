////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INT2_H_INCLUDED
#define INT2_H_INCLUDED

namespace xray {
namespace math {

class int2;

class int2_pod {
public:
	typedef int		type;

public:
#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4201)
#endif // #if defined(_MSC_VER)
	union {
		struct {
			type		x;
			type		y;
		};
		type			elements[2];
	};
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif // #if defined(_MSC_VER)

public:
	inline	int2&		operator -		( );

	inline	int2&		operator +=		( int2_pod const& other );
	inline	int2&		operator +=		( type value );

	inline	int2&		operator -=		( int2_pod const& other );
	inline	int2&		operator -=		( type value );

	inline	int2&		operator *=		( type value );
	inline	int2&		operator /=		( type value );

	inline	type&		operator [ ]	( int index );

	inline	int2&		set				( type vx, type vy);
	inline	bool		similar			( int2_pod const& other ) const;
}; // class int2_pod

class int2 : public int2_pod {
public:
	typedef int2_pod		type_pod;

public:
	inline				int2			( );
	inline				int2			( type const other_x, type const other_y );
	inline				int2			( int2 const& other );
	inline				int2			( int2_pod const& other );
}; // class int2

inline int2			operator +		( int2_pod const& left, int2_pod const& right );
inline int2			operator +		( int2_pod const& left, int2_pod::type const& right );
inline int2			operator -		( int2_pod const& left, int2_pod const& right );
inline int2			operator -		( int2_pod const& left, int2_pod::type const& right );
inline int2			operator *		( int2_pod const& left, int2_pod::type const& right );
inline int2			operator *		( int2_pod::type const& left, int2_pod const& right );
inline int2			operator /		( int2_pod const& left, int2_pod::type const& right );
inline bool			operator ==		( int2_pod const& left, int2_pod const& right );
inline bool			operator !=		( int2_pod const& left, int2_pod const& right );

}// namespace math
}// namespace xray

#include <xray/math_int2_inline.h>

#endif // #ifndef INT2_H_INCLUDED