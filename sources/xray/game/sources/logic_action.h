////////////////////////////////////////////////////////////////////////////
//	Created		: 10.05.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGIC_ACTION_H_INCLUDED
#define LOGIC_ACTION_H_INCLUDED

#include "logic_fwd_defines.h"

namespace stalker2 {

class action_emit_event : public logic_action 
{
	typedef logic_action super;
public:
							action_emit_event	( event_manager* event_manager, game_event const& e, object_controlled* o = NULL );
	virtual					~action_emit_event	( ) { }
	virtual void			execute						( game_event const& e );

private:
	game_event				m_event;
	object_controlled*		m_object_controlled;
};

class action_set_object_behaviour : public logic_action 
{
	typedef logic_action super;
public:
							action_set_object_behaviour ( event_manager* event_manager, object_controlled* o, object_behaviour* b, object_scene_job* job );
	virtual					~action_set_object_behaviour( ) { }
	virtual void			execute						( game_event const& e );

	object_controlled*		m_object;
	object_behaviour*		m_behaviour;
	object_scene_job*		m_job;
};

class action_filtered_set_object_behaviour : public logic_action
{
	typedef logic_action super;
public:
	action_filtered_set_object_behaviour ( event_manager* event_manager, object_controlled* o, configs::binary_config_value const& action_data, object_scene_job* job );
	virtual					~action_filtered_set_object_behaviour( ) { }
	virtual void			execute						( game_event const& e );

	object_controlled*		m_object;
	configs::binary_config_value const& m_action_data;
	object_scene_job*		m_job;
};

class action_start_scene : public logic_action 
{
	typedef logic_action super;
public:
						action_start_scene			( event_manager* event_manager, object_scene_ptr scene );
	virtual				~action_start_scene			( ) { }
	virtual void		execute						( game_event const& e );

	object_scene_ptr	m_scene_to_start;
};

} // namespace stalker2

#endif // #ifndef LOGIC_ACTION_H_INCLUDED