////////////////////////////////////////////////////////////////////////////
//	Created		: 04.10.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_COMPOSITE_H_INCLUDED
#define OBJECT_COMPOSITE_H_INCLUDED


#include "object.h"
#include "object_behaviour.h"

namespace stalker2 
{
	class object_composite : public game_object_, public object_controlled
	{

		typedef game_object_	super;

	public:
		object_composite								( game_world& w )
		:super( w )
		{
		}
	};
} // namespace stalker2 

#endif // #ifndef OBJECT_COMPOSITE_H_INCLUDED