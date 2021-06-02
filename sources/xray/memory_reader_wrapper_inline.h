////////////////////////////////////////////////////////////////////////////
//	Created 	: 18.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_READER_WRAPPER_INLINE_H_INCLUDED
#define XRAY_MEMORY_READER_WRAPPER_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE	template < typename implementation >
#define READER_WRAPPER		xray::memory::reader_wrapper< implementation >

TEMPLATE_SIGNATURE
inline READER_WRAPPER::reader_wrapper							( )
{
}

TEMPLATE_SIGNATURE
inline void READER_WRAPPER::r									( pvoid result, u32 destination_size, u32 const size )
{
	impl( ).r			( result, destination_size, size );
}

TEMPLATE_SIGNATURE
inline u64 READER_WRAPPER::r_u64								( )
{
	return				( r< u64 >( ) );
}

TEMPLATE_SIGNATURE
inline s64 READER_WRAPPER::r_s64								( )
{
	return				( r< s64 >( ) );
}

TEMPLATE_SIGNATURE
inline u32 READER_WRAPPER::r_u32								( )
{
	return				( r< u32 >( ) );
}

TEMPLATE_SIGNATURE
inline s32 READER_WRAPPER::r_s32								( )
{
	return				( r< s32 >( ) );
}

TEMPLATE_SIGNATURE
inline u16 READER_WRAPPER::r_u16								( )
{
	return				( r< u16 >( ) );
}

TEMPLATE_SIGNATURE
inline s16 READER_WRAPPER::r_s16								( )
{
	return				( r< s16 >( ) );
}

TEMPLATE_SIGNATURE
inline u8 READER_WRAPPER::r_u8									( )
{
	return				( r< u8 >( ) );
}

TEMPLATE_SIGNATURE
inline s8 READER_WRAPPER::r_s8									( )
{
	return				( r< s8 >( ) );
}

TEMPLATE_SIGNATURE
inline float READER_WRAPPER::r_float							( )
{
	return				( r< float >( ) );
}

TEMPLATE_SIGNATURE
inline xray::math::float2	READER_WRAPPER::r_float2			( )
{
	return				( r< math::float2 >( ) );
}

TEMPLATE_SIGNATURE
inline xray::math::float3	READER_WRAPPER::r_float3			( )
{
	return				( r< math::float3 >( ) );
}

TEMPLATE_SIGNATURE
inline xray::math::float4	READER_WRAPPER::r_float4			( )
{
	return				( r< math::float4 >( ) );
}

TEMPLATE_SIGNATURE
inline xray::math::float4x4 READER_WRAPPER::r_float4x4			( )
{
	return				( r< math::float4x4 >( ) );
}

TEMPLATE_SIGNATURE
inline float READER_WRAPPER::r_float_q8							( float const min, float const max )
{
	u8 const value		= r_u8( );
	float const result	= ( float( value )/255.0001f ) *( max - min ) + min;
	ASSERT				( ( result >= min ) && ( result <= max ) );
    return				( result );
}

TEMPLATE_SIGNATURE
inline float READER_WRAPPER::r_float_q16						( float const min, float const max )
{
	u16 const value		= r_u16( );
	float const result	= ( float( value )/65535.f) *( max - min ) + min;
	ASSERT				( ( result >= min ) && ( result <= max ) );
    return				( result );
}

TEMPLATE_SIGNATURE
inline float READER_WRAPPER::r_angle_q8							( )
{
	return				( r_float_q8( 0.f, math::pi_x2) );
}

TEMPLATE_SIGNATURE
inline float READER_WRAPPER::r_angle_q16						( )
{
	return				( r_float_q16( 0.f, math::pi_x2) );
}

TEMPLATE_SIGNATURE
template < typename T >
inline T READER_WRAPPER::r										( )
{
	T					result;
	r					( &result, sizeof( result ), sizeof( result ) );
	return				( result );
}

TEMPLATE_SIGNATURE
inline implementation& READER_WRAPPER::impl						( )
{
	return				( ( implementation& )*this );
}

TEMPLATE_SIGNATURE
inline implementation const& READER_WRAPPER::impl				( ) const
{
	return				( ( implementation const& )*this );
}

#if 0
TEMPLATE_SIGNATURE
inline xray::math::float3 READER_WRAPPER::r_direction_q16		( )
{
	return				( math::float3::decompress( r_u16( ) ) );
}

TEMPLATE_SIGNATURE
inline xray::math::float3 READER_WRAPPER::r_position_q48		( )
{
	return				( r_direction_q16( )*r_float( ) );
}
#endif // #if 0

#undef READER_WRAPPER
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_MEMORY_READER_WRAPPER_INLINE_H_INCLUDED