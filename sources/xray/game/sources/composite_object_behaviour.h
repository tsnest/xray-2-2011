////////////////////////////////////////////////////////////////////////////
//	Created		: 04.10.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef COMPOSITE_OBJECT_BEHAVIOUR_H_INCLUDED
#define COMPOSITE_OBJECT_BEHAVIOUR_H_INCLUDED

#include "object_behaviour.h"

namespace stalker2{

struct storage_item
{
	storage_item( pcstr name,	pcstr value ) :
	name	( name ),
	value	( value ) 
	{
	}
	pcstr name;
	pcstr value;
};

class composite_object_behaviour : public object_behaviour
{
	typedef object_behaviour super;
public:
	composite_object_behaviour( object_scene_job* owner, configs::binary_config_value const& data, pcstr name );
	virtual void		attach_to_object		( object_controlled* o );
	virtual void		detach_from_object		( object_controlled* o );	

private:
	object_controlled*			m_object;
	vector< storage_item >		m_storage_items_list;


}; // class composite_object_behaviour

} // namespace stalker2

#endif // #ifndef COMPOSITE_OBJECT_BEHAVIOUR_H_INCLUDED