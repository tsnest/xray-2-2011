////////////////////////////////////////////////////////////////////////////
//	Created		: 19.05.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_SPL_H_INCLUDED
#define SOUND_SPL_H_INCLUDED

#include <xray/math_curve.h>
#include <xray/sound/sound.h>

namespace xray {
namespace sound {

class XRAY_SOUND_API sound_spl :	public resources::unmanaged_resource,
									private noncopyable
{
public:
										sound_spl				( );
										~sound_spl				( );

	inline	math::float_curve* const	get_curve				( ) { return &m_curve_line; }

	inline	float						get_spl					( float distance ) const { return m_curve_line.evaluate( distance, 0.0f ); }
	inline	float						get_unheard_distance	( ) const { return m_curve_line.curve_time_max; }
	// 1.0f - max loudness, 0.0f - min
	inline	float						get_loudness			( float distance ) const;

#ifndef MASTER_GOLD
			void						save					( xray::configs::lua_config_value t_root );
			void						load					( xray::configs::lua_config_value const& t_root );
#endif // #ifndef MASTER_GOLD
			void						load					( xray::configs::binary_config_value const& t_root );
private:
	math::float_curve				m_curve_line;
}; // class sound_spl

typedef	resources::resource_ptr < sound_spl, resources::unmanaged_intrusive_base > sound_spl_ptr;

inline
float sound_spl::get_loudness	( float distance ) const
{
	float max_spl				= m_curve_line.curve_value_max - m_curve_line.curve_value_min;
	float normalized_level		= m_curve_line.evaluate( distance, 0.0f ) - m_curve_line.curve_value_min;
	if ( normalized_level < 0.0f )
		return					0.0f;
	else
		return					normalized_level / max_spl; 

}

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_SPL_H_INCLUDED