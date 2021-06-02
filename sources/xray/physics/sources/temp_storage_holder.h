////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.02.2008
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
//	all vectors that clear after every island step
////////////////////////////////////////////////////////////////////////////
#ifndef TEMP_STORAGE_HOLDER_H_INCLUDED
#define TEMP_STORAGE_HOLDER_H_INCLUDED

#include <xray/collision/object.h>

class	contact_storage;
class	island_object;
struct	dxJoint;
struct	dxBody;

//class space_query_result_storage;
class temp_storage_holder
{

public:
												temp_storage_holder			( );
												~temp_storage_holder		( );

public:
	inline	vectora< island_object* >&		island_objects_pointers	( ) { return m_island_objects_pointers; }
	inline	contact_storage&				contacts				( ) { return *m_contacts;}
	inline	xray::collision::objects_type&	space_collision_results	( ) { return *m_space_collision_results; }
	inline	vectora<dxJoint*>&				joints					( ) { return m_joints; }
	inline	vectora<dxBody*>&				bodies					( ) { return m_bodies; }
	//island_objects_pointers_storage		*m_island_objects_pointers;
	//space_query_result_storage			*m_space_query_result;

private:
	contact_storage*						m_contacts;
	vectora< island_object* >				m_island_objects_pointers;
	xray::collision::objects_type*			m_space_collision_results;
	vectora< dxJoint* >						m_joints;
	vectora< dxBody* >						m_bodies;

}; // class temp_storage_holder


#endif