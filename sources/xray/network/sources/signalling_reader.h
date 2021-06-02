////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SIGNALLING_READER_H_INCLUDED
#define SIGNALLING_READER_H_INCLUDED

#include <boost/noncopyable.hpp>
#include <xray/debug/signalling_bool.h>
#include "reader_base.h"
#include "device_traits_meta_functions.h"

namespace xray
{

template <typename Device>
class signalling_reader : 
	private boost::noncopyable,
	public	base_if<is_seekable_reader<Device>::result,
				seekable_reader<signalling_reader<Device>, Device> >,
	public	base_if<has_direct_read_access<Device>::result,
				direct_accessible_reader<signalling_reader<Device>, Device> >
{
public:
	typedef Device								device_type;
	typedef	typename Device::size_type			size_type;
	typedef	typename Device::pointer_type		pointer_type;
	typedef	typename Device::const_pointer_type	const_pointer_type;

	friend	class seekable_reader			<signalling_reader, Device>;
	friend	class direct_accessible_reader	<signalling_reader, Device>;

	explicit			signalling_reader			( device_type & device );
						~signalling_reader			( );

	signalling_bool		r_u64			( u64 & dest );
	signalling_bool		r_s64			( s64 & dest );
	signalling_bool		r_u32			( u32 & dest );
	signalling_bool		r_s32			( s32 & dest );
	signalling_bool		r_u16			( u16 & dest );
	signalling_bool		r_s16			( s16 & dest );
	signalling_bool		r_u8			( u8 & dest );
	signalling_bool		r_s8			( s8 & dest );

	signalling_bool		r_float			( float & dest );
	signalling_bool		r_float2		( math::float2 & dest );
	signalling_bool		r_float3		( math::float3 & dest );
	signalling_bool		r_float4		( math::float4 & dest );
	signalling_bool		r_float4x4		( math::float4x4 & dest );

	signalling_bool		r_float_q8		( float & dest, float min, float max );
	signalling_bool		r_float_q16		( float & dest, float min, float max );

	signalling_bool		r_angle_q8		( float & dest );
	signalling_bool		r_angle_q16		( float & dest );

	template <int ArraySize>
	signalling_bool		r_char_array	( char (&carray)[ArraySize] );
	signalling_bool		r_string		( buffer_string & str_dest );
	signalling_bool		read			( mutable_buffer dst, size_type const size );
private:
	device_type	&		m_device;
}; // class signalling_reader

} // namespace xray

#include "signalling_reader_inline.h"

#endif // #ifndef SIGNALLING_READER_H_INCLUDED