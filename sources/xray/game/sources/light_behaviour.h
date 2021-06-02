////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LIGHT_BEHAVIOUR_H_INCLUDED
#define LIGHT_BEHAVIOUR_H_INCLUDED

#include "object_behaviour.h"
#include <xray/math_curve.h>
#include "object_light.h"

namespace stalker2{


class light_static_behaviour : public object_behaviour
{
	typedef object_behaviour super;

public:
						light_static_behaviour	( object_scene_job* owner, configs::binary_config_value const& data, pcstr name );
	virtual void		attach_to_object		( object_controlled* o );
	virtual void		detach_from_object		( object_controlled* o );
private:
	object_light_ptr	m_light;
	bool				m_state;
	float				m_light_range;
	float				m_light_intensity;
	u32					m_light_color;
}; // class light_static_behaviour

class light_anim_behaviour : public object_behaviour
{
	typedef object_behaviour super;

public:
						light_anim_behaviour	( object_scene_job* owner, configs::binary_config_value const& data, pcstr name );
	virtual void		attach_to_object		( object_controlled* o );
	virtual void		detach_from_object		( object_controlled* o );
	virtual void		tick					( );
private:
	object_light_ptr			m_light;
	math::curve_line_float		m_track_intensity;
	math::curve_line_float		m_track_range;
	math::curve_line_color_base	m_color_anim;
	timing::timer				m_timer;
	bool						m_bcyclic;
	float						m_track_length;
	bool						m_animation_end_raised;
	render::light_props			m_initial_light_props;
}; // class light_anim_behaviour


} // namespace stalker2

#endif // #ifndef LIGHT_BEHAVIOUR_H_INCLUDED