////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PROXIMITY_TRIGGER_H_INCLUDED
#define OBJECT_PROXIMITY_TRIGGER_H_INCLUDED

#include "object.h"
#include "object_behaviour.h"

namespace xray{ namespace collision{ struct primitive; class geometry_instance; } }

namespace stalker2
{
	class object_collision_geometry;

	enum proximity_trigger_testee_status
	{
		proximity_trigger_testee_status_none,
		proximity_trigger_testee_status_enter,
		proximity_trigger_testee_status_leave,
		proximity_trigger_testee_status_inside,
		proximity_trigger_testee_status_outside
	};

	class object_proximity_trigger: public game_object_static, public object_controlled
	{
		typedef game_object_static									super;
		typedef buffer_vector< collision::geometry_instance* >		instances_vector;
	public:
						object_proximity_trigger	( game_world& w );
		virtual			~object_proximity_trigger	( );

	public:
				u32									get_inside_objects_count	( );
		virtual void								load						( configs::binary_config_value const& t );
				void								collision_object_loaded		( game_object_ptr_ const& object );
				bool								is_object_inside			( collision::geometry_instance* testee );
				proximity_trigger_testee_status		test_object					( collision::geometry_instance* testee );
				//void								test_objects				( instances_vector testees, instances_vector entered, instances_vector gone, instances_vector inside );

	public:
		boost::function< void ( collision::geometry_instance const& ) >		enter;
		boost::function< void ( collision::geometry_instance const& ) >		leave;


	private:
		instances_vector							m_inside_objects;
		object_collision_geometry*					m_collision_geometry;
		
	}; // class object_proximity_trigger

} // namespace stalker2

#endif // #ifndef OBJECT_PROXIMITY_TRIGGER_H_INCLUDED