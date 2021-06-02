////////////////////////////////////////////////////////////////////////////
//	Created		: 25.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef WRITER_H_INCLUDED
#define WRITER_H_INCLUDED

#include <boost/noncopyable.hpp>
#include "writer_base.h"
#include "device_traits_meta_functions.h"

namespace xray
{

template <typename Device>
class writer :
	private boost::noncopyable,
	public	base_if<is_seekable_writer<Device>::result,
				seekable_writer<writer<Device>, Device> >
{
public:
	typedef Device								device_type;
	typedef	typename Device::size_type			size_type;
	typedef	typename Device::pointer_type		pointer_type;
	typedef	typename Device::const_pointer_type	const_pointer_type;

	friend	class seekable_writer<writer, Device>;
	
	explicit		writer		(device_type & device);
					~writer		();
	
	void			w_u64		( u64 const & value );
	void			w_s64		( s64 const & value );
	void			w_u32		( u32 const & value );
	void			w_s32		( s32 const & value );
	void			w_u16		( u16 const value );
	void			w_s16		( s16 const value );
	void			w_u8		( u8 const value );
	void			w_s8		( s8 const value );

	void			w_float		( float const & value );
	void			w_float2	( math::float2 const & value );
	void			w_float3	( math::float3 const & value );
	void			w_float4	( math::float4 const & value );
	void			w_float4x4	( math::float4x4 const & value );

	void			w_float_q8	( float const & value, float const & min, float const & max );
	void			w_float_q16	( float const & value, float const & min, float const & max );

	void			w_angle_q8	( float const & value );
	void			w_angle_q16	( float const & value );

	template <int ArraySize>
	void			w_char_array( char const (&carray)[ArraySize] );

	void			w_string	( buffer_string const & str );

	void			write		( pcvoid buff, size_type const buff_size );
private:
	/*typedef	words_writer<has_direct_access<device_type> >			device_writer_impl_type;
	typedef endian_writer_adaptor<need_invert_endian<device_type>,
								  device_writer_impl_type>			words_writer_impl_type;
	*/
	device_type &				m_device;
	//words_writer_impl_type		m_words_writer;
}; // class writer

} // namespace xray

#include "writer_inline.h"


#endif // #ifndef WRITER_H_INCLUDED