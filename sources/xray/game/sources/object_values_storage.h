////////////////////////////////////////////////////////////////////////////
//	Created		: 29.09.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_VALUES_STORAGE_H_INCLUDED
#define OBJECT_VALUES_STORAGE_H_INCLUDED

#include "object.h"
#include "object_behaviour.h"

namespace stalker2 
{
class object_values_storage : public game_object_, public object_controlled
{

typedef game_object_	super;

public:
					object_values_storage			( game_world& w );
	virtual void	load							( configs::binary_config_value const& t );
	virtual	void	on_scene_start					( );
	void			set_value_changed_callback		( const fastdelegate::FastDelegate< void (pcstr) >	&value_changed_callback );
	void			set_value						( pcstr value );
	pcstr			get_value						( ) { return m_current_value; };

private:
	pcstr											m_current_value;
	bool											m_current_value_initialized;
	fastdelegate::FastDelegate< void (pcstr) >		m_value_changed_callback;

};
} // namespace stalker2 


#endif // #ifndef OBJECT_VALUES_STORAGE_H_INCLUDED