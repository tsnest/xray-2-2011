////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef READER_H_INCLUDED
#define READER_H_INCLUDED

#include "device_traits_meta_functions.h"
#include "signalling_reader.h"


namespace xray
{

template <typename Device, bool IsSignalling = false>
class reader :
	private signalling_reader<Device>,
	public	base_if<is_seekable_reader<Device>::result,
				strict_seekable_reader<signalling_reader<Device> > >
{
public:
	typedef Device								device_type;
	typedef	typename Device::size_type			size_type;
	typedef	typename Device::pointer_type		pointer_type;
	typedef	typename Device::const_pointer_type	const_pointer_type;

	inline					reader			( device_type & device );
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

	template <int ArraySize>
	pcstr					r_char_array	( char (&carray)[ArraySize] );
	pcstr					r_string		( buffer_string & str_dest );
	inline	void			read			( mutable_buffer dst, size_type const size );
private:
	typedef signalling_reader<Device>		implementation_type;
}; // class reader

template <typename Device>
class reader<Device, true> :
	public signalling_reader<Device>
{
public:
	typedef signalling_reader<Device> implementation_type;

	explicit	reader	(Device & device) : implementation_type(device) {};
				~reader	() {};
};

} // namespace xray

#include "reader_inline.h"

#endif // #ifndef READER_H_INCLUDED