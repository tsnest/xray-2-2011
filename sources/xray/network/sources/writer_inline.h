////////////////////////////////////////////////////////////////////////////
//	Created		: 25.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef WRITER_INLINE_H_INCLUDED
#define WRITER_INLINE_H_INCLUDED

namespace xray
{

template <typename Device>
writer<Device>::writer(device_type & device) :
	m_device(device)
{
}

template <typename Device>
writer<Device>::~writer()
{
}

template <typename Device>
void writer<Device>::w_u64( u64 const & value )
{
	m_device.write(pointer_cast<pcbyte>(&value), sizeof(value));
}

template <typename Device>
void writer<Device>::w_s64( s64 const & value )
{
	m_device.write(pointer_cast<pcbyte>(&value), sizeof(value));
}


template <typename Device>
void writer<Device>::w_u32( u32 const & value )
{
	m_device.write(pointer_cast<pcbyte>(&value), sizeof(value));
}

template <typename Device>
void writer<Device>::w_s32( s32 const & value )
{
	m_device.write(pointer_cast<pcbyte>(&value), sizeof(value));
}

template <typename Device>
void writer<Device>::w_u16( u16 const value )
{
	m_device.write(pointer_cast<pcbyte>(&value), sizeof(value));
}

template <typename Device>
void writer<Device>::w_s16( s16 const value )
{
	m_device.write(pointer_cast<pcbyte>(&value), sizeof(value));
}


template <typename Device>
void writer<Device>::w_u8( u8 const value )
{
	m_device.write(pointer_cast<pcbyte>(&value), sizeof(value));
}

template <typename Device>
void writer<Device>::w_s8( s8 const value )
{
	m_device.write(pointer_cast<pcbyte>(&value), sizeof(value));
}

template <typename Device>
void writer<Device>::w_float( float const & value )
{
	m_device.write(pointer_cast<pcbyte>(&value), sizeof(value));
}

template <typename Device>
void writer<Device>::w_float2( math::float2 const & value )
{
	w_float(value.x);
	w_float(value.y);
}

template <typename Device>
void writer<Device>::w_float3( math::float3 const & value )
{
	w_float(value.x);
	w_float(value.y);
	w_float(value.z);
}

template <typename Device>
void writer<Device>::w_float4( math::float4 const & value )
{
	w_float(value.x);
	w_float(value.y);
	w_float(value.z);
	w_float(value.w);
}

template <typename Device>
void writer<Device>::w_float4x4( math::float4x4 const & value )
{
	w_float4(value.i);
	w_float4(value.j);
	w_float4(value.k);
	w_float4(value.c);
}

template <typename Device>
void writer<Device>::w_float_q8( float const & value,
								float const & min,
								float const & max )
{
	ASSERT((value >= min) && (value <= max));
	float q = (value - min) / (max - min);
	w_u8(static_cast<u8>(math::floor(q * 255.0f + 0.5f))); 
}

template <typename Device>
void writer<Device>::w_float_q16( float const & value,
								 float const & min,
								 float const & max )
{
	ASSERT((value >= min) && (value <= max));
	float q = (value - min) / (max - min);
	w_u16(static_cast<u16>(math::floor(q * 65535.0f + 0.5f)));
}

template <typename Device>
void writer<Device>::w_angle_q8( float const & value )
{
	w_float_q8(value, 0.0f, math::pi_x2);
}

template <typename Device>
void writer<Device>::w_angle_q16( float const & value )
{
	w_float_q16(value, 0.0f, math::pi_x2);
}

template <typename Device>
	template <int ArraySize>
void writer<Device>::w_char_array( char const (&carray)[ArraySize] )
{
	m_device.write(pointer_cast<pcbyte>(carray), ArraySize);
}

template <typename Device>
void writer<Device>::w_string( buffer_string const & str )
{
	m_device.write(pointer_cast<pcbyte>(str.c_str()), str.length() + 1);
}

template <typename Device>
void writer<Device>::write( pcvoid buff, size_type const buff_size )
{
	m_device.write(xray::pointer_cast<const_pointer_type>(buff), buff_size);
}

} // namespace xray

#endif // #ifndef WRITER_INLINE_H_INCLUDED