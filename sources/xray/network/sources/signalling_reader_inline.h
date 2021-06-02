////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SIGNALLING_READER_INLINE_H_INCLUDED
#define SIGNALLING_READER_INLINE_H_INCLUDED

namespace xray
{

template <typename Device>
signalling_reader<Device>::signalling_reader( device_type & device ) :
	m_device(device)
{
}

template <typename Device>
signalling_reader<Device>::~signalling_reader( )
{
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_u64( u64 & dest )
{
	return m_device.read(pointer_cast<pointer_type>(&dest), sizeof(dest)) ? true : false;
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_s64( s64 & dest )
{
	return m_device.read(pointer_cast<pointer_type>(&dest), sizeof(dest)) ? true : false;
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_u32( u32 & dest )
{
	return m_device.read(pointer_cast<pointer_type>(&dest), sizeof(dest)) ? true : false;
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_s32( s32 & dest )
{
	return m_device.read(pointer_cast<pointer_type>(&dest), sizeof(dest)) ? true : false;
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_u16( u16 & dest )
{
	return m_device.read(pointer_cast<pointer_type>(&dest), sizeof(dest)) ? true : false;
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_s16( s16 & dest )
{
	return m_device.read(pointer_cast<pointer_type>(&dest), sizeof(dest)) ? true : false;
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_u8( u8 & dest )
{
	return m_device.read(pointer_cast<pointer_type>(&dest), sizeof(dest)) ? true : false;
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_s8( s8 & dest )
{
	return m_device.read(pointer_cast<pointer_type>(&dest), sizeof(dest)) ? true : false;
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_float( float & dest )
{
	return m_device.read(pointer_cast<pointer_type>(&dest), sizeof(dest)) ? true : false;
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_float2( math::float2 & dest )
{
	return	r_float(dest.x) &&
			r_float(dest.y);
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_float3( math::float3 & dest )
{
	return	r_float(dest.x) &&
			r_float(dest.y) &&
			r_float(dest.z);
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_float4( math::float4 & dest )
{
	return	r_float(dest.x) &&
			r_float(dest.y) &&
			r_float(dest.z) &&
			r_float(dest.w);
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_float4x4( math::float4x4 & dest )
{
	return	r_float4(dest.i) &&
			r_float4(dest.j) &&
			r_float4(dest.k) &&
			r_float4(dest.c);
}


template <typename Device>
signalling_bool	signalling_reader<Device>::r_float_q8( float & dest, float min, float max )
{
	u8	tmp_value;
	if (!r_u8(tmp_value))
		return false;

	dest = ( float( tmp_value )/255.0f ) *( max - min ) + min;
	if	((dest < min) || (dest > max))
		return false;
    
	return true;
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_float_q16( float & dest, float min, float max )
{
	u16	tmp_value;
	if (!r_u16(tmp_value))
		return false;

	dest = (float(tmp_value)/65535.0f) *(max - min) + min;
	if	((dest < min) || (dest > max))
		return false;
    
	return true;	
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_angle_q8( float & dest )
{
	return r_float_q8(dest, 0.0f, math::pi_x2);
}

template <typename Device>
signalling_bool	signalling_reader<Device>::r_angle_q16( float & dest )
{
	return r_float_q16(dest, 0.0f, math::pi_x2);
}

template <typename Device>
	template <int ArraySize>
signalling_bool	signalling_reader<Device>::r_char_array( char (&carray)[ArraySize] )
{
	mutable_buffer tmp_dst_buff(pointer_cast<pvoid>(carray), ArraySize);
	return read(tmp_dst_buff, ArraySize) ? true : false;
}

template <bool IsBufferDevice = false>
struct string_reader_impl
{
	template <typename Reader>
	static signalling_bool	read	( Reader & owner, buffer_string & str_dest )
	{
		ASSERT(str_dest.max_length() > 0);
	
		u8			tmp_char;
		u32			tmp_strsize = 0;
		u32	const	max_strsize = str_dest.max_length() - 1;
		while (owner.r_u8(tmp_char))
		{
			if (tmp_char == 0)
				return true;

			str_dest.append(static_cast<char>(tmp_char));
			++tmp_strsize;

			if (tmp_strsize > max_strsize)
				return false;
		}
		
		return false;
	}
};

template <>
struct string_reader_impl<true>
{
	template <typename Reader>
	static signalling_bool	read	( Reader & owner, buffer_string & str_dest )
	{
		typedef typename Reader::size_type	size_type;
		size_type tmp_length	= xray::strings::length(
			pointer_cast<pcstr>(owner.pointer()));

		if (owner.elapsed() < tmp_length)
			return false;
		
		if (str_dest.max_length() < tmp_length)
			return false;

		str_dest.assign			(owner.pointer(), owner.pointer() + tmp_length);
		
		return owner.advance	(tmp_length + 1) ? true : false;
	}
};


template <typename Device>
signalling_bool	signalling_reader<Device>::r_string( buffer_string & str )
{
	typedef string_reader_impl<
		is_seekable_reader<Device>::result &&
		has_direct_read_access<Device>::result>		str_reader_type;
	
	return str_reader_type::read(*this, str) ? true : false;
}

template <typename Device>
signalling_bool	signalling_reader<Device>::read( mutable_buffer dst, size_type const read_size )
{
	if (dst.size() < read_size)
		return false;

	return m_device.read(pointer_cast<pointer_type>(dst.c_ptr()), read_size) ? true : false;
}

} // namespace xray

#endif // #ifndef SIGNALLING_READER_INLINE_H_INCLUDED