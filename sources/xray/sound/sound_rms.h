////////////////////////////////////////////////////////////////////////////
//	Created		: 12.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_RMS_H_INCLUDED
#define SOUND_RMS_H_INCLUDED

namespace xray {
namespace sound {


class sound_rms : private boost::noncopyable
{
public:
	typedef	float rms_value_type;
public:
								sound_rms		( 
									resources::managed_resource_ptr const& ogg_raw_file,
									float discr
								);
								~sound_rms		( );



			rms_value_type		get_rms_by_time							( u64 msec )	const;
	inline	rms_value_type		get_rms_by_index						( u32 index )	const	{ R_ASSERT (index < m_count); return m_data[index]; }
			u64					find_min_value_time_in_interval			( u64 start_time, u64 end_time, rms_value_type min_value = 0.0f ) const;

	inline	u32					size									( )				const	{ return m_count; }
private:
	float			m_samples_discr;
	u32				m_count;
	u64				m_length_in_msec;
#pragma warning(disable:4200)
	rms_value_type	m_data[];
#pragma warning(default:4200)
}; // class sound_rms

typedef resources::managed_resource_ptr sound_rms_ptr;

struct sound_rms_pinned: public resources::pinned_ptr_base< sound_rms >
{
	sound_rms_pinned( sound_rms_ptr res ): resources::pinned_ptr_base< sound_rms >( res )
	{ }
};

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_RMS_H_INCLUDED