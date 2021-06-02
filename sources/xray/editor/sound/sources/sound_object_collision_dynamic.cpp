////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "sound_object_collision_dynamic.h"

using xray::sound_editor::sound_object_collision_dynamic;
using xray::sound_editor::sound_object_instance;
using namespace xray::collision;

sound_object_collision_dynamic::sound_object_collision_dynamic(geometry_instance* geometry, sound_object_instance^ obj, object_type t)
:super(g_allocator, t, geometry),
m_owner(obj)
{ 	
}