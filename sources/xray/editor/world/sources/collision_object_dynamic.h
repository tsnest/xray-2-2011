////////////////////////////////////////////////////////////////////////////
//	Created		: 23.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLISION_OBJECT_DYNAMIC_H_INCLUDED
#define COLLISION_OBJECT_DYNAMIC_H_INCLUDED

#include <xray/collision/collision_object.h>
namespace xray {
namespace editor {
ref class object_base;

class collision_object_dynamic : public collision::collision_object
{
	typedef collision::collision_object	super;
public:
							collision_object_dynamic(	collision::geometry_instance* geometry,
														object_base^ dynamic, collision::object_type t);
	object_base^			get_owner_object		( ) const;
	virtual bool			touch					( ) const;

private:
	gcroot<object_base^>	m_owner;
}; //collision_object_dynamic

}// namespace editor
}// namespace xray
#endif // #ifndef COLLISION_OBJECT_DYNAMIC_H_INCLUDED