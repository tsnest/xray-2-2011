////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_OBJECT_COLLISION_DYNAMIC_H_INCLUDED
#define SOUND_OBJECT_COLLISION_DYNAMIC_H_INCLUDED

#include <xray/collision/collision_object.h>
#include "sound_object_instance.h"

namespace xray {
namespace sound_editor {

	ref class sound_object_instance;

	class sound_object_collision_dynamic : public collision::collision_object
	{
		typedef collision::collision_object super;
	public:
		sound_object_collision_dynamic(collision::geometry_instance* geometry, sound_object_instance^ inst, collision::object_type t);
		sound_object_instance^ get_owner_object() const {return m_owner;};
		virtual bool touch() const {return false;};

	private:
		gcroot<sound_object_instance^> m_owner;
	}; //sound_object_collision_dynamic
}// namespace sound_editor
}// namespace xray
#endif // #ifndef SOUND_OBJECT_COLLISION_DYNAMIC_H_INCLUDED