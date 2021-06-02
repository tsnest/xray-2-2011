////////////////////////////////////////////////////////////////////////////
//	Created 	: 18.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_READER_WRAPPER_H_INCLUDED
#define XRAY_MEMORY_READER_WRAPPER_H_INCLUDED

namespace xray {
namespace memory {

template < typename implementation >
class XRAY_CORE_API reader_wrapper {
public:
	inline					reader_wrapper	( );
	inline	void			r				( pvoid result, u32 destination_size, u32 size );
	inline	u64				r_u64			( );
	inline	s64				r_s64			( );
	inline	u32				r_u32			( );
	inline	s32				r_s32			( );
	inline	u16				r_u16			( );
	inline	s16				r_s16			( );
	inline	u8				r_u8			( );
	inline	s8				r_s8			( );

	inline	float			r_float			( );
	inline	math::float2	r_float2		( );
	inline	math::float3	r_float3		( );
	inline	math::float4	r_float4		( );
	inline	math::float4x4	r_float4x4		( );

	inline	float			r_float_q8		( float min, float max );
	inline	float			r_float_q16		( float min, float max );

	inline	float			r_angle_q8		( );
	inline	float			r_angle_q16		( );

//	inline	math::float3	r_direction_q16	( );
//	inline	math::float3	r_position_q48	( );

private:
	template < typename T >
	inline	T				r				( );
	inline	implementation&	impl			( );
	inline	implementation const& impl		( ) const;
}; // class reader_wrapper

} // namespace memory
} // namespace xray

#include <xray/memory_reader_wrapper_inline.h>

#endif // #ifndef XRAY_MEMORY_READER_WRAPPER_H_INCLUDED