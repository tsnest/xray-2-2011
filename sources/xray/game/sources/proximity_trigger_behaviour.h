////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PROXIMITY_TRIGGER_BEHAVIOUR_H_INCLUDED
#define PROXIMITY_TRIGGER_BEHAVIOUR_H_INCLUDED

#include "object_behaviour.h"

#ifndef MASTER_GOLD
#	include "proximity_trigger_input_handler.h"
#	include <xray/collision/primitives.h>
#endif //#ifndef MASTER_GOLD

namespace xray{ namespace collision{ struct primitive; class geometry_instance; class geometry; } }

namespace stalker2
{
	class object_collision_geometry;
	class object_proximity_trigger;

	class proximity_trigger_behaviour: public object_behaviour
	{
		typedef object_behaviour super;

	public:
									proximity_trigger_behaviour	( object_scene_job* owner, configs::binary_config_value const& data, pcstr name );
									~proximity_trigger_behaviour( );
		virtual void				attach_to_object			( object_controlled* o );
		virtual void				detach_from_object			( object_controlled* o );
		virtual void				tick						( );
		object_proximity_trigger&	get_proximity_trigger		( ){ return *m_proximity_trigger; }

	private:
		object_proximity_trigger*		m_proximity_trigger;
		collision::geometry_instance *	m_testee;

#ifndef MASTER_GOLD

	public:
		float4x4 const&				get_testee_transform			( );
		void						set_testee_transform			( float4x4 const& matrix );

	private:
		void						input_handler_enabled_changed	( pcstr args );

	public:
		collision::primitive				m_testee_primitive;
		collision::primitive_type			m_last_testee_primitive_type;

	private:
		proximity_trigger_input_handler*	m_input_handler;
		bool								m_is_input_handler_enabled;

		static bool							m_is_testee_volume_enabled;
		static bool							m_is_input_handler_enabled_catched;

#endif //#ifndef MASTER_GOLD

	}; // class proximity_trigger_behaviour

} // namespace stalker2

#endif // #ifndef PROXIMITY_TRIGGER_BEHAVIOUR_H_INCLUDED