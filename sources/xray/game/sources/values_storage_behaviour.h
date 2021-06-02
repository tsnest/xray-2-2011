////////////////////////////////////////////////////////////////////////////
//	Created		: 04.10.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VALUES_STORAGE_BEHAVIOUR_H_INCLUDED
#define VALUES_STORAGE_BEHAVIOUR_H_INCLUDED

#include "object_behaviour.h"

namespace stalker2{

class values_storage_behaviour : public object_behaviour
{
	typedef object_behaviour super;
public:
	values_storage_behaviour( object_scene_job* owner, configs::binary_config_value const& data, pcstr name );
	virtual void		attach_to_object		( object_controlled* o );
	virtual void		detach_from_object		( object_controlled* o );	

private:
	void attached_storage_value_changed			( pcstr value );

	object_controlled*			m_object;

}; // class values_storage_behaviour

} // namespace stalker2


#endif // #ifndef VALUES_STORAGE_BEHAVIOUR_H_INCLUDED