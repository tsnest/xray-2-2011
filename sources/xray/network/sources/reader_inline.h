////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef READER_INLINE_H_INCLUDED
#define READER_INLINE_H_INCLUDED

namespace xray
{

template <typename Device, bool IsSignalling>
reader<Device, IsSignalling>::reader( device_type & device ) :
	implementation_type(device)
{
}

template <typename SignalingReader, bool IsSeekable>
struct safe_buffer_reader_impl
{
	typedef typename SignalingReader::size_type		size_type;
	static void read( SignalingReader & owner, pvoid dst, size_type const read_size)
	{
		bool result = owner.read(dst, read_size);
		R_ASSERT(result);
		XRAY_UNREFERENCED_PARAMETER(status);
	}
}; // struct safe_buffer_reader_impl

template <typename SignalingReader>
struct safe_buffer_reader_impl<SignalingReader, true>
{
	typedef typename SignalingReader::size_type		size_type;
	static void read( SignalingReader & owner, pvoid dst, size_type const read_size)
	{
		size_type	curr_pos	= owner.tell();
		size_type	elapsed		= owner.elapsed();
		size_type	to_read		= read_size;
		R_ASSERT(to_read <= elapsed);
		if (to_read > elapsed)
		{
			to_read	= elapsed;
		}
		bool result = owner.read(dst, to_read);
		R_ASSERT(result);
		XRAY_UNREFERENCED_PARAMETER(status);
	}
}; // struct safe_buffer_reader_impl


template <typename Device, bool IsSignalling>
inline void			reader<Device, IsSignalling>::read( mutable_buffer dst,
													   size_type const size )
{
	typedef safe_buffer_reader_impl<
		is_seekable_reader<Device>::result>	buffer_reader_impl_type;
	R_ASSERT(dst.size() >= size);

	size_type	to_read_size = dst.size() < size ? dst.size() : size;
	buffer_reader_impl_type::read(*this, dst.data(), to_read_size);
}

template <typename Device, bool IsSignalling>
inline u64			reader<Device, IsSignalling>::r_u64( )
{
	u64		result;
	bool	status = implementation_type::r_u64(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline s64			reader<Device, IsSignalling>::r_s64( )
{
	s64		result;
	bool	status = implementation_type::r_s64(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline u32			reader<Device, IsSignalling>::r_u32( )
{
	u32		result;
	bool	status = implementation_type::r_u32(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline s32			reader<Device, IsSignalling>::r_s32( )
{
	s32		result;
	bool	status = implementation_type::r_s32(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline u16			reader<Device, IsSignalling>::r_u16( )
{
	u16		result;
	bool	status = implementation_type::r_u16(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline s16			reader<Device, IsSignalling>::r_s16( )
{
	s16		result;
	bool	status = implementation_type::r_s16(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline u8			reader<Device, IsSignalling>::r_u8( )
{
	u8		result;
	bool	status = implementation_type::r_u8(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline s8			reader<Device, IsSignalling>::r_s8( )
{
	s8		result;
	bool	status = implementation_type::r_s8(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline float		reader<Device, IsSignalling>::r_float( )
{
	float	result;
	bool	status = implementation_type::r_float(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline math::float2		reader<Device, IsSignalling>::r_float2( )
{
	math::float2	result;
	bool			status = implementation_type::r_float2(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline math::float3		reader<Device, IsSignalling>::r_float3( )
{
	math::float3	result;
	bool			status = implementation_type::r_float3(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline math::float4		reader<Device, IsSignalling>::r_float4( )
{
	math::float4	result;
	bool			status = implementation_type::r_float4(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline math::float4x4	reader<Device, IsSignalling>::r_float4x4( )
{
	math::float4x4	result;
	bool			status = implementation_type::r_float4x4(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline float			reader<Device, IsSignalling>::r_float_q8( float min, float max )
{
	float	result;
	bool	status = implementation_type::r_float_q8(result, min, max);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline float			reader<Device, IsSignalling>::r_float_q16( float min, float max )
{
	float	result;
	bool	status = implementation_type::r_float_q16(result, min, max);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline float			reader<Device, IsSignalling>::r_angle_q8( )
{
	float	result;
	bool	status = implementation_type::r_angle_q8(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
inline float			reader<Device, IsSignalling>::r_angle_q16( )
{
	float	result;
	bool	status = implementation_type::r_angle_q16(result);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	result;
}

template <typename Device, bool IsSignalling>
	template <int ArraySize>
pcstr					reader<Device, IsSignalling>::r_char_array( char (&carray)[ArraySize] )
{
	bool	status = implementation_type::r_char_array(carray);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	carray;
}

template <typename Device, bool IsSignalling>
pcstr					reader<Device, IsSignalling>::r_string( buffer_string & str_dest )
{
	bool	status = implementation_type::r_string(str_dest);
	R_ASSERT(status);
	XRAY_UNREFERENCED_PARAMETER(status);
	return	str_dest.c_str();
}


} // namespace xray

#endif // #ifndef READER_INLINE_H_INCLUDED