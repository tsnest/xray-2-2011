#include "pch.h"

//#include "contact_info_buffer.h"
//#include "island.h"
//#include "contact_joint_info.h"
//#include "contact.h"




/*
bool create_contact_joints( island& isl, const contact_info_buffer &contacts, body& body_0, body& body_1 )
{
	bool result = true;
	physics::vector<contact_info>::const_iterator i = contacts.contacts().begin();
	physics::vector<contact_info>::const_iterator e = contacts.contacts().end();
	for( ; i!=e; ++i )
		result = create_contact( isl, *i, body_0, body_1 ) || result;
	return result;
}

bool create_contact_joints( island& isl, const contact_info_buffer &contacts, body& body )
{
	bool result = true;
	physics::vector<contact_info>::const_iterator i = contacts.contacts().begin();
	physics::vector<contact_info>::const_iterator e = contacts.contacts().end();
	for( ; i!=e; ++i )
		result = create_contact( isl, *i, body ) || result;
	return result;
}
*/