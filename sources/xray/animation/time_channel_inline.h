////////////////////////////////////////////////////////////////////////////
//	Created		: 27.09.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_TIME_CHANNEL_INLINE_H_INCLUDED
#define XRAY_ANIMATION_TIME_CHANNEL_INLINE_H_INCLUDED

namespace xray {
namespace animation {

template < class DomainType >
inline time_channel< DomainType >::time_channel					( ) :
	m_knots_count				( u32(-1) ), 
	m_internal_memory_position	( size_t( -1 ) )
{
}

template < class DomainType >
inline time_channel< DomainType >::~time_channel				( )
{
	ASSERT						( m_knots_count !=  u32(-1) );
	ASSERT						( m_internal_memory_position !=  size_t( -1 ) );
	for ( u32 i = 0; i < domains_count(); ++i )
		domains()[ i ].~DomainType	( );
}

template < class DomainType >
inline	DomainType const& time_channel< DomainType >::domain	( u32 const id ) const
{
	ASSERT						( id < domains_count() );
	return						domains()[ id ];
}

template < class DomainType >
inline DomainType& time_channel< DomainType >::domain			( u32 const id )
{
	ASSERT						( id < domains_count() );
	return						domains()[ id ];
}

template < class DomainType >
inline float time_channel< DomainType >::knot					( u32 const id ) const
{
	ASSERT						( id < knots_count() );
	return						knots()[ id ];
}

template < class DomainType >
inline float& time_channel< DomainType >::knot					( u32 const id )
{
	ASSERT						( id < knots_count() );
	return						knots()[ id ];
}

template < class DomainType >
size_t time_channel< DomainType >::count_internal_memory_size	( u32 const knots_count ) 
{ 
	ASSERT						( knots_count > 0 );
	return						sizeof(DomainType) * ( knots_count - 1 ) + sizeof( float ) * knots_count; 
}

template < class DomainType >
inline void time_channel< DomainType >::create_in_place			( pvoid const memory_buffer, u32 const knots_count )
{
	m_internal_memory_position	= size_t ( pbyte( memory_buffer ) - pbyte( this ) );
	m_knots_count				= knots_count ;

	ASSERT						( m_knots_count !=  u32(-1) );
	ASSERT						( m_internal_memory_position !=  size_t( -1 ) );

	for ( u32 i = 0, n = domains_count(); i < n; ++i )
		new ( &domains()[ i ] ) DomainType;
}

template < class DomainType >
u32	time_channel< DomainType >::domain( float t, u32& current_domain  ) const
{
	u32 const knots_count		= m_knots_count;
	
	float const* l_knots		= knots();
	R_ASSERT_CMP_U				( l_knots[ m_knots_count - 1 ], >, l_knots[ 0 ] );

	math::clamp					( t, knots()[0], knots()[knots_count-1] );
	CURE_ASSERT					( t >= knots()[0], t = knots()[0] );
	CURE_ASSERT					( t <= knots()[knots_count-1], t = knots()[knots_count-1] );

	for ( u32 i= 0; i < knots_count - 1 ; ++i ) {
		u32 const j				= ( i + current_domain ) %(knots_count-1);
		if ( (knots()[j] <= t) && (knots()[j+1] >= t) ) {
			current_domain		= j;
			return				j;
		}
	}
	
	UNREACHABLE_CODE			( return u32(-1) );
}

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_TIME_CHANNEL_INLINE_H_INCLUDED