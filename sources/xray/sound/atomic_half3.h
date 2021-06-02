////////////////////////////////////////////////////////////////////////////
//	Created		: 18.10.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_SOUND_ATOMIC_HALF3_H_INCLUDED
#define XRAY_SOUND_ATOMIC_HALF3_H_INCLUDED

#include <xray/math_half.h>

namespace xray {
namespace sound {

class atomic_half3
{
public:
									atomic_half3( );
									atomic_half3( atomic_half3 const& rhs );
			atomic_half3 const&		operator=	( atomic_half3 const& rhs );

	inline	void					set			( math::half3 const& val );
	inline	math::half3_pod const&	get			( ) const;
private:
	union
	{
		math::half3_pod				m_val;
		threading::atomic64_type	m_atomic;
	}								m_data;
}; // class atomic_half3

inline
atomic_half3::atomic_half3					( )
{
	m_data.m_val.set_x		( math::half( 0.0f ) );
	m_data.m_val.set_y		( math::half( 0.0f ) );
	m_data.m_val.set_z		( math::half( 0.0f ) );
}

inline
atomic_half3::atomic_half3					( atomic_half3 const& rhs )
{
	set				( rhs.m_data.m_val );
}

inline
atomic_half3 const& atomic_half3::operator=	( atomic_half3 const& rhs )
{
	set				( rhs.m_data.m_val );
	return			*this;
}

inline
void atomic_half3::set						( math::half3 const& val )
{
	atomic_half3 tmp;
	tmp.m_data.m_val 						= val;
	threading::interlocked_exchange			( m_data.m_atomic, tmp.m_data.m_atomic );
}

inline
math::half3_pod const& atomic_half3::get	( ) const
{
	return m_data.m_val;
}

} // namespace sound
} // namespace xray

#endif // #ifndef XRAY_SOUND_ATOMIC_HALF3_H_INCLUDED